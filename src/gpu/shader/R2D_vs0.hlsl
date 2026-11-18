#include "common_vs.hlsli"

//2D Vertex Shaders handle menu, hud, full screen quad for render target swap etc.

struct VS_INPUT
{
    float4 v[16] : TEXCOORD;
};


struct VS_OUTPUT // Declared identical to pixel shader input (see PS_INPUT)
{
    float4 oPos : SV_Position; // Homogeneous clip space position
    float4 oD0 : COLOR0; // Primary color (front-facing)
    float4 oD1 : COLOR1; // Secondary color (front-facing)
    float oFog : FOG; // Fog coordinate
    float oPts : PSIZE; // Point size
    float4 oB0 : TEXCOORD4; // Back-facing primary color
    float4 oB1 : TEXCOORD5; // Back-facing secondary color
    float4 oT0 : TEXCOORD0; // Texture coordinate set 0
    float4 oT1 : TEXCOORD1; // Texture coordinate set 1
    float4 oT2 : TEXCOORD2; // Texture coordinate set 2
    float4 oT3 : TEXCOORD3; // Texture coordinate set 3
};

VS_OUTPUT main(VS_INPUT xIn)
{
	// Output variables
    float4 oPos, oD0, oD1, oB0, oB1, oT0, oT1, oT2, oT3;
    oPos = oD0 = oD1 = oB0 = oB1 = oT0 = oT1 = oT2 = oT3 = float4(0, 0, 0, 1); // Pre-initialize w component of outputs to 1

	// Single component outputs
    float4 oFog, oPts; // x is write-only on Xbox. Use float4 as some games use incorrect masks
    oFog = 1; // Default to no fog. 
    oPts = 0;

	// Address (index) register
    int1 a0 = 0;

	// Temporary registers
    float4 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11;
    r0 = r1 = r2 = r3 = r4 = r5 = r6 = r7 = r8 = r9 = r10 = r11 = float4(0, 0, 0, 0);
#define r12 oPos // oPos and r12 are two ways of accessing the same register on Xbox

	// Input registers
    float4 v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15;

	// Unpack 16 flags from 4 float4 constant registers
    float vRegisterDefaultFlags[16];
    UnpackRegisterDefaultFlags(vRegisterDefaultFlags);


	// Initialize input registers from the vertex buffer data
	// Or use the register's default value (which can be changed by the title)
#define init_v(i) v##i = lerp(xIn.v[i], vRegisterDefaultValues(i), vRegisterDefaultFlags[i]);
	// Note : unroll manually instead of for-loop, because of the ## concatenation
	init_v(0);init_v(1);init_v(2);init_v(3);
	init_v(4);init_v(5);init_v(6);init_v(7);
	init_v(8);init_v(9);init_v(10);init_v(11);
    
	init_v(12);init_v(13);init_v(14);init_v(15);

	// Temp variable for paired VS instruction
    float4 temp;
    
    
  



    // For some reason, some vertex for 2D elements have their position w set to an absurdly high number
    // (This is the result of RenderWare doing the math: 1 / RwIm2DGetNearScreenZ())
	// while Xbox may not care about this, modern api definitely do, so we have to fix this
	// this isn't a recomp instruction bug, I checked on CXBX Reloaded and logged the values, pos.w is supposed to be high.
	// for what I saw, message box have it set to inf, while fadeout and credits text have their entry around 10.f.

   // if (isinf(v0.w) || isnan(v0.w) || v0.w > 1.f)

	if ((asuint(v0.w) & 0x7F800000) == 0x7F800000)
    {
        v0.w = 1.f;
    }


	// Xbox shader program starts here
	
  x_mov(r0, xyz, c(1));
  x_rcp(oPos, w, v0.w);

  x_mov(oD0, xyzw, v1);

  x_mov(oT0, xy, v2);

  x_mad(oPos, xyz, v0.xyzz, c(0).xyzz, r0.xyzz);
	// End Xbox shader program

	// Copy variables to output struct
    VS_OUTPUT xOut;
         
 
    xOut.oPos = reverseScreenspaceTransform(oPos);

    
    xOut.oD0 = saturate(oD0);
    xOut.oD1 = saturate(oD1);
    xOut.oFog = oFog.x; // Note : Xbox clamps fog in pixel shader -> *NEEDS TESTING* /was oFog.x 
    xOut.oPts = oPts.x;
    xOut.oB0 = saturate(oB0);
    xOut.oB1 = saturate(oB1);
	// Scale textures (TODO : or should we apply this to the input register values?)
    xOut.oT0 = oT0 / xboxTextureScale(0);
    xOut.oT1 = oT1 / xboxTextureScale(1);
    xOut.oT2 = oT2 / xboxTextureScale(2);
    xOut.oT3 = oT3 / xboxTextureScale(3);

    return xOut;
}