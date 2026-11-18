
#define X_D3DSCM_CORRECTION                 96 // Add 96 to arrive at the range 0..191 (instead of -96..95)
#define X_D3DVS_CONSTREG_COUNT              192

#ifdef __spirv__

struct PushConstants
{
    uint64_t VertexShaderConstants;
    uint64_t PixelShaderConstants;
    uint64_t SharedConstants;
};

[[vk::push_constant]] ConstantBuffer<PushConstants> g_PushConstants;

// Sewer note: Read 16-byte aligned for perf.
#define REGISTER_ALIGNMENT 16
#define VS_REG_F(cN) vk::RawBufferLoad<float4>(g_PushConstants.VertexShaderConstants + (cN) * 16, REGISTER_ALIGNMENT)
#define VS_SC_REG_U(cN) vk::RawBufferLoad<uint4>(g_PushConstants.SharedConstants + (cN) * 16, REGISTER_ALIGNMENT)

#define vs_consts(i) VS_REG_F(i)
#define vRegisterDefaultValues(i) VS_REG_F(192 + (i))
#define vRegisterDefaultFlagsPacked(i) VS_REG_F(208 + (i))

#define xboxScreenspaceScale  VS_REG_F(212)
#define xboxScreenspaceOffset  VS_REG_F(213)
#define xboxTextureScale(i) VS_REG_F(214 + (i))
#define UserInfo  VS_REG_F(219)


#define FogTableMode VS_SC_REG_U(81).z
#define FogDensity VS_SC_REG_U(81).w
#define FogStart VS_SC_REG_U(82).x
#define FogEnd VS_SC_REG_U(82).y


#else

cbuffer VertexShaderConstants : register(b0, space4)
{
    float4 vs_constants[X_D3DVS_CONSTREG_COUNT] : packoffset(c0); // Xbox constant registers (// c0 -> c191) 
    float4 vRegisterDefaultVal[16] : packoffset(c192); //vertex attribute (it seems to work if those are empty see SetFixedFunctionDefaultVertexAttributes)
    float4 vRegisterDefaultFlagsPack[4] : packoffset(c208); // Default values for vertex registers, and whether to use them (see UpdateHostVertexDeclaration function)
    float4 xboxScreenspaceScale : packoffset(c212);
    float4 xboxScreenspaceOffset : packoffset(c213);
    float4 xboxTextureScl[4] : packoffset(c214);
    float4 FogInfo : packoffset(c218); //Parameters for mapping the shader's fog output value to a fog factor (D3DVS_CONSTREG_FOGINFO) 
    float4 UserInfo_ : packoffset(c219); // res width, height and Config AspectRatio
}

cbuffer SharedConstants : register(b2, space4)
{
    uint FogTableMode : packoffset(c81.z);
    uint FogDensity : packoffset(c81.w);   
    uint FogStart : packoffset(c82.x);
    uint FogEnd : packoffset(c82.y);

    float alphaThreshold : packoffset(c88.z);
}

#define vs_consts(i) vs_constants[i]
#define vRegisterDefaultValues(i) vRegisterDefaultVal[i]
#define vRegisterDefaultFlagsPacked(i) vRegisterDefaultFlagsPack[i]
#define xboxTextureScale(i) xboxTextureScl[i]
#define UserInfo UserInfo_
#endif

void UnpackRegisterDefaultFlags(out float flags[16])
{
    float4 packed0 = vRegisterDefaultFlagsPacked(0);
    float4 packed1 = vRegisterDefaultFlagsPacked(1);
    float4 packed2 = vRegisterDefaultFlagsPacked(2);
    float4 packed3 = vRegisterDefaultFlagsPacked(3);

    flags[0] = packed0.x;
    flags[1] = packed0.y;
    flags[2] = packed0.z;
    flags[3] = packed0.w;

    flags[4] = packed1.x;
    flags[5] = packed1.y;
    flags[6] = packed1.z;
    flags[7] = packed1.w;

    flags[8] = packed2.x;
    flags[9] = packed2.y;
    flags[10] = packed2.z;
    flags[11] = packed2.w;

    flags[12] = packed3.x;
    flags[13] = packed3.y;
    flags[14] = packed3.z;
    flags[15] = packed3.w;
}

float4 _tof4(float src)
{
    return float4(src, src, src, src);
}

float4 _tof4(float2 src)
{
    return src.xyyy;
}

float4 _tof4(float3 src)
{
    return src.xyzz;
}

float4 _tof4(float4 src)
{
    return src;
}

float4 _ssss(float s)
{
    return float4(s, s, s, s);
} // a scalar output replicated across a 4-component vector

#define _scalar(src) _tof4(src).x /* a scalar input */

float4 c(int register_number)
{
	// Map Xbox [-96, 95] to Host [0, 191]
	// Account for Xbox's negative constant indexes
    register_number += X_D3DSCM_CORRECTION;

    // If the index is too large, set it to -1 so 0 is returned
    // Note: returning 0 directly requires many more instructions
    if (register_number >= X_D3DVS_CONSTREG_COUNT) // X_D3DVS_CONSTREG_COUNT
        register_number = -1;
    

    return vs_consts(register_number);
}

// Due to rounding differences with the Xbox (and increased precision on PC?)
// some titles produce values just below the threshold of the next integer.
// We can add a small bias to make sure it's bumped over the threshold
#define BIAS 0.001

// 2.14.1.11  Vertex Program Floating Point Requirements
// The floor operations used by the ARL and EXP instructions must
// operate identically.  Specifically, the EXP instruction's floor(t.x)
// intermediate result must exactly match the integer stored in the
// address register by the ARL instruction.
float x_floor(float src)
{
    return floor(src + BIAS);
}

float x_floor_scalar(float src)
{
    return floor(src + BIAS);
}

float4 x_floor(float4 src)
{
    return floor(src + BIAS);
}

// https://xboxdevwiki.net/NV2A/Vertex_Shader
// https://www.khronos.org/registry/OpenGL/extensions/NV/NV_vertex_program.txt
// https://www.khronos.org/registry/OpenGL/extensions/NV/NV_vertex_program1_1.txt

// Functions for MAC ('Multiply And Accumulate') opcodes

// 2.14.1.10.1  ARL: Address Register Load
// The address register should be floored
//#define x_arl(dest, mask, src0) dest.mask = x_floor(_tof4(src0).x).mask
#define x_arl(dest, mask, src0) dest.mask = (int)x_floor_scalar(_tof4(src0).x)

// 2.14.1.10.2  MOV: Move
#define x_mov(dest, mask, src0) dest.mask = (_tof4(src0)).mask

// 2.14.1.10.3  MUL: Multiply
#define x_mul(dest, mask, src0, src1) dest.mask = (_tof4(src0) * _tof4(src1)).mask

// 2.14.1.10.4  ADD: Add
#define x_add(dest, mask, src0, src1) dest.mask = (_tof4(src0) + _tof4(src1)).mask

// 2.14.1.10.5  MAD: Multiply and Add
#define x_mad(dest, mask, src0, src1, src2) dest.mask = (_tof4(src0) * _tof4(src1) + _tof4(src2)).mask

// 2.14.1.10.8  DP3: Three-Component Dot Product
#define x_dp3(dest, mask, src0, src1) dest.mask = _ssss(dot(_tof4(src0).xyz, _tof4(src1).xyz)).mask

//  2.14.1.10.9  DP4: Four-Component Dot Product
#define x_dp4(dest, mask, src0, src1) dest.mask = _ssss(dot(_tof4(src0), _tof4(src1))).mask

// 2.14.1.10.10  DST: Distance Vector
#define x_dst(dest, mask, src0, src1) dest.mask = dst(_tof4(src0), _tof4(src1)).mask /* equals { dest.x = 1; dest.y = src0.y * src1.y; dest.z = src0.z; dest.w = src1.w; } */

// 2.14.1.10.11  MIN: Minimum
#define x_min(dest, mask, src0, src1) dest.mask = min(_tof4(src0), _tof4(src1)).mask

// 2.14.1.10.12  MAX: Maximum
#define x_max(dest, mask, src0, src1) dest.mask = max(_tof4(src0), _tof4(src1)).mask

// 2.14.1.10.13  SLT: Set On Less Than
#define x_slt(dest, mask, src0, src1) dest.mask = _slt(_tof4(src0), _tof4(src1)).mask
float4 _slt(float4 src0, float4 src1)
{
    float4 dest;
    dest.x = (src0.x < src1.x) ? 1 : 0;
    dest.y = (src0.y < src1.y) ? 1 : 0;
    dest.z = (src0.z < src1.z) ? 1 : 0;
    dest.w = (src0.w < src1.w) ? 1 : 0;
    return dest;
}

// 2.14.1.10.14  SGE: Set On Greater or Equal Than
#define x_sge(dest, mask, src0, src1) dest.mask = _sge(_tof4(src0), _tof4(src1)).mask
float4 _sge(float4 src0, float4 src1)
{
    float4 dest;
    dest.x = (src0.x >= src1.x) ? 1 : 0;
    dest.y = (src0.y >= src1.y) ? 1 : 0;
    dest.z = (src0.z >= src1.z) ? 1 : 0;
    dest.w = (src0.w >= src1.w) ? 1 : 0;
    return dest;
}

// 2.14.1.10.18  DPH: Homogeneous Dot Product
#define x_dph(dest, mask, src0, src1) dest.mask = _ssss(_dph(_tof4(src0), _tof4(src1))).mask
float _dph(float4 src0, float4 src1)
{
    return dot(src0.xyz, src1.xyz) + src1.w;
}

// Xbox ILU Functions

// 2.14.1.10.7  RSQ: Reciprocal Square Root
#define x_rsq(dest, mask, src0) dest.mask = _ssss(_rsq(_scalar(src0))).mask
float _rsq(float src)
{
    float a = abs(src);
#if 0 // TODO : Enable
	if (a == 1) return 1;
	if (a == 0) return 1.#INF;
#endif
    return rsqrt(a);
}

// 2.14.1.10.15  EXP: Exponential Base 2
#define x_expp(dest, mask, src0) dest.mask = _expp(_scalar(src0)).mask
float4 _expp(float src)
{
    float floor_src = x_floor(src);

    float4 dest;
    dest.x = exp2(floor_src);
    dest.y = src - floor_src;
    dest.z = exp2(src);
    dest.w = 1;

    return dest;
}

// 2.14.1.10.16  LOG: Logarithm Base 2
#define x_logp(dest, mask, src0) dest.mask = _logp(_scalar(src0)).mask
float4 _logp(float src)
{
    float4 dest;
#if 0 // TODO : Enable
	float t = abs(src);
	if (t != 0) {
		if (t == 1.#INF) {
			dest.x = 1.#INF;
			dest.y = 1;
			dest.z = 1.#INF;
		} else {
#endif
    float exponent;
    float mantissa = frexp(src /* + BIAS*/, /*out*/exponent);
    float z = log2(src);
    dest.x = exponent;
    dest.y = mantissa;
    dest.z = z;
#if 0
		}
	} else {
		dest.x = -1.#INF;
		dest.y = 1;
		dest.z = -1.#INF;
	}
#endif
    dest.w = 1;
    return dest;
}

// 2.14.1.10.17  LIT: Light Coefficients
#define x_lit(dest, mask, src) dest.mask = _lit(_tof4(src)).mask
float4 _lit(float4 src0)
{
    const float epsilon = 1.0f / 256.0f;

    float diffuse = src0.x;
    float blinn = src0.y;
    float specPower = clamp(src0.w, -(128 - epsilon), (128 - epsilon));

    float4 dest;
    dest.x = 1;
    dest.y = max(0, diffuse);
    dest.z = (diffuse > 0) && (blinn > 0) ? pow(blinn, specPower) : 0;
    dest.w = 1;

    return dest;
}

// 2.14.1.10.19  RCC: Reciprocal Clamped
#define x_rcc(dest, mask, src0) dest.mask = _ssss(_rcc(_scalar(src0))).mask
float _rcc(float src)
{
	// Calculate the reciprocal
    float r = 1 / src;

	// Clamp
    return (r >= 0)
		? clamp(r, 5.42101e-020f, 1.84467e+019f) // the IEEE 32-bit binary values 0x1F800000 and 0x5F800000
		: clamp(r, -1.84467e+019f, -5.42101e-020f); // the IEEE 32-bit binary values 0xDF800000 and 0x9F800000
}

// 2.14.1.10.6  RCP: Reciprocal
#define x_rcp(dest, mask, src0) dest.mask = _ssss(_rcp(_scalar(src0))).mask
float _rcp(float src)
{
	// OpenGL/NVidia extension definition
#if 0 // TODO : Enable?
	if (src == 1) return 1;
	if (src == 0) return 1.#INF;
	return 1 / src;
#endif
	// Forward to Xbox clamped reciprocal
	// So we have defined behaviour with rcp(0)
	// This prevents issues with XYZRHW modes
	// where the w component may be 0
    return _rcc(src);
}

float4 reverseScreenspaceTransform(float4 oPos)
{
	// On Xbox, oPos should contain the vertex position in screenspace
	// We need to reverse this transformation
	// Conventionally, each Xbox Vertex Shader includes instructions like this
	// mul oPos.xyz, r12, c-38
	// +rcc r1.x, r12.w
	// mad oPos.xyz, r12, r1.x, c-37
	// where c-37 and c-38 are reserved transform values

	// oPos.w and xboxViewportScale.z might be VERY big when a D24 depth buffer is used
	// and multiplying oPos.xyz by oPos.w may cause precision issues.
     
	// Reverse screenspace offset
    oPos -= xboxScreenspaceOffset;  
	// Reverse screenspace scale
    oPos /= xboxScreenspaceScale;

	// Ensure w is nonzero
    if (oPos.w == 0)
        oPos.w = 1;
	// Reverse perspective divide
    oPos.xyz *= oPos.w;
    
    return oPos;
}