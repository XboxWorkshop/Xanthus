#include "video.h"
#include "renderstate.h"
#include "FixedFunction.h"
#include "nv2a.h"
#include <kernel/function.h>
#include <xanthus/utility.hpp>

//Since Xbox has a lot of arbitrary rules with RenderState, we can't just pass the state as they are for the host
//Different methods exist to set a render state and some require conversion, this page will handle that
//heavy inspired by CXBXR code

uint32_t* g_renderState = nullptr; 
std::array<uint64_t, X_D3DRS_LAST + 1> g_dirty_render_states;
std::array<int32_t, X_D3DRS_LAST + 1>  g_RenderStateOffsets;
static std::array<uint32_t, X_D3DRS_LAST + 1> g_renderStateSnapshot;
static uint32_t g_renderStateCount;
static bool g_renderStateSnapshotValid;

const RenderStateInfo DxbxRenderStateInfo[1 + X_D3DRS_DONOTCULLUNCOMPRESSED] = {

	// String                                 Ord  Version Type                   Method              Native
	{ "D3DRS_PSALPHAINPUTS0"              /*=   0*/, 3424, xtDWORD,               NV2A_RC_IN_ALPHA(0) },
	{ "D3DRS_PSALPHAINPUTS1"              /*=   1*/, 3424, xtDWORD,               NV2A_RC_IN_ALPHA(1) },
	{ "D3DRS_PSALPHAINPUTS2"              /*=   2*/, 3424, xtDWORD,               NV2A_RC_IN_ALPHA(2) },
	{ "D3DRS_PSALPHAINPUTS3"              /*=   3*/, 3424, xtDWORD,               NV2A_RC_IN_ALPHA(3) },
	{ "D3DRS_PSALPHAINPUTS4"              /*=   4*/, 3424, xtDWORD,               NV2A_RC_IN_ALPHA(4) },
	{ "D3DRS_PSALPHAINPUTS5"              /*=   5*/, 3424, xtDWORD,               NV2A_RC_IN_ALPHA(5) },
	{ "D3DRS_PSALPHAINPUTS6"              /*=   6*/, 3424, xtDWORD,               NV2A_RC_IN_ALPHA(6) },
	{ "D3DRS_PSALPHAINPUTS7"              /*=   7*/, 3424, xtDWORD,               NV2A_RC_IN_ALPHA(7) },
	{ "D3DRS_PSFINALCOMBINERINPUTSABCD"   /*=   8*/, 3424, xtDWORD,               NV2A_RC_FINAL0 },
	{ "D3DRS_PSFINALCOMBINERINPUTSEFG"    /*=   9*/, 3424, xtDWORD,               NV2A_RC_FINAL1 },
	{ "D3DRS_PSCONSTANT0_0"               /*=  10*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(0) },
	{ "D3DRS_PSCONSTANT0_1"               /*=  11*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(1) },
	{ "D3DRS_PSCONSTANT0_2"               /*=  12*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(2) },
	{ "D3DRS_PSCONSTANT0_3"               /*=  13*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(3) },
	{ "D3DRS_PSCONSTANT0_4"               /*=  14*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(4) },
	{ "D3DRS_PSCONSTANT0_5"               /*=  15*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(5) },
	{ "D3DRS_PSCONSTANT0_6"               /*=  16*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(6) },
	{ "D3DRS_PSCONSTANT0_7"               /*=  17*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(7) },
	{ "D3DRS_PSCONSTANT1_0"               /*=  18*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR1(0) },
	{ "D3DRS_PSCONSTANT1_1"               /*=  19*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR1(1) },
	{ "D3DRS_PSCONSTANT1_2"               /*=  20*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR1(2) },
	{ "D3DRS_PSCONSTANT1_3"               /*=  21*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR1(3) },
	{ "D3DRS_PSCONSTANT1_4"               /*=  22*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR1(4) },
	{ "D3DRS_PSCONSTANT1_5"               /*=  23*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR1(5) },
	{ "D3DRS_PSCONSTANT1_6"               /*=  24*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR1(6) },
	{ "D3DRS_PSCONSTANT1_7"               /*=  25*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR1(7) },
	{ "D3DRS_PSALPHAOUTPUTS0"             /*=  26*/, 3424, xtDWORD,               NV2A_RC_OUT_ALPHA(0) },
	{ "D3DRS_PSALPHAOUTPUTS1"             /*=  27*/, 3424, xtDWORD,               NV2A_RC_OUT_ALPHA(1) },
	{ "D3DRS_PSALPHAOUTPUTS2"             /*=  28*/, 3424, xtDWORD,               NV2A_RC_OUT_ALPHA(2) },
	{ "D3DRS_PSALPHAOUTPUTS3"             /*=  29*/, 3424, xtDWORD,               NV2A_RC_OUT_ALPHA(3) },
	{ "D3DRS_PSALPHAOUTPUTS4"             /*=  30*/, 3424, xtDWORD,               NV2A_RC_OUT_ALPHA(4) },
	{ "D3DRS_PSALPHAOUTPUTS5"             /*=  31*/, 3424, xtDWORD,               NV2A_RC_OUT_ALPHA(5) },
	{ "D3DRS_PSALPHAOUTPUTS6"             /*=  32*/, 3424, xtDWORD,               NV2A_RC_OUT_ALPHA(6) },
	{ "D3DRS_PSALPHAOUTPUTS7"             /*=  33*/, 3424, xtDWORD,               NV2A_RC_OUT_ALPHA(7) },
	{ "D3DRS_PSRGBINPUTS0"                /*=  34*/, 3424, xtDWORD,               NV2A_RC_IN_RGB(0) },
	{ "D3DRS_PSRGBINPUTS1"                /*=  35*/, 3424, xtDWORD,               NV2A_RC_IN_RGB(1) },
	{ "D3DRS_PSRGBINPUTS2"                /*=  36*/, 3424, xtDWORD,               NV2A_RC_IN_RGB(2) },
	{ "D3DRS_PSRGBINPUTS3"                /*=  37*/, 3424, xtDWORD,               NV2A_RC_IN_RGB(3) },
	{ "D3DRS_PSRGBINPUTS4"                /*=  38*/, 3424, xtDWORD,               NV2A_RC_IN_RGB(4) },
	{ "D3DRS_PSRGBINPUTS5"                /*=  39*/, 3424, xtDWORD,               NV2A_RC_IN_RGB(5) },
	{ "D3DRS_PSRGBINPUTS6"                /*=  40*/, 3424, xtDWORD,               NV2A_RC_IN_RGB(6) },
	{ "D3DRS_PSRGBINPUTS7"                /*=  41*/, 3424, xtDWORD,               NV2A_RC_IN_RGB(7) },
	{ "D3DRS_PSCOMPAREMODE"               /*=  42*/, 3424, xtDWORD,               NV2A_TX_SHADER_CULL_MODE },
	{ "D3DRS_PSFINALCOMBINERCONSTANT0"    /*=  43*/, 3424, xtDWORD,               NV2A_RC_COLOR0 },
	{ "D3DRS_PSFINALCOMBINERCONSTANT1"    /*=  44*/, 3424, xtDWORD,               NV2A_RC_COLOR1 },
	{ "D3DRS_PSRGBOUTPUTS0"               /*=  45*/, 3424, xtDWORD,               NV2A_RC_OUT_RGB(0) },
	{ "D3DRS_PSRGBOUTPUTS1"               /*=  46*/, 3424, xtDWORD,               NV2A_RC_OUT_RGB(1) },
	{ "D3DRS_PSRGBOUTPUTS2"               /*=  47*/, 3424, xtDWORD,               NV2A_RC_OUT_RGB(2) },
	{ "D3DRS_PSRGBOUTPUTS3"               /*=  48*/, 3424, xtDWORD,               NV2A_RC_OUT_RGB(3) },
	{ "D3DRS_PSRGBOUTPUTS4"               /*=  49*/, 3424, xtDWORD,               NV2A_RC_OUT_RGB(4) },
	{ "D3DRS_PSRGBOUTPUTS5"               /*=  50*/, 3424, xtDWORD,               NV2A_RC_OUT_RGB(5) },
	{ "D3DRS_PSRGBOUTPUTS6"               /*=  51*/, 3424, xtDWORD,               NV2A_RC_OUT_RGB(6) },
	{ "D3DRS_PSRGBOUTPUTS7"               /*=  52*/, 3424, xtDWORD,               NV2A_RC_OUT_RGB(7) },
	{ "D3DRS_PSCOMBINERCOUNT"             /*=  53*/, 3424, xtDWORD,               NV2A_RC_ENABLE },
	{ "D3DRS_PSTEXTUREMODES_RESERVED"     /*=  54*/, 3424, xtDWORD,               NV2A_CLEAR_VALUE }, // Dxbx note : This takes the slot of X_D3DPIXELSHADERDEF.PSTextureModes, set by D3DDevice_SetRenderState_LogicOp?
	{ "D3DRS_PSDOTMAPPING"                /*=  55*/, 3424, xtDWORD,               NV2A_TX_SHADER_DOTMAPPING },
	{ "D3DRS_PSINPUTTEXTURE"              /*=  56*/, 3424, xtDWORD,               NV2A_TX_SHADER_PREVIOUS },
	// End of "pixel-shader" render states, continuing with "simple" render states :
	{ "D3DRS_ZFUNC"                       /*=  57*/, 3424, xtD3DCMPFUNC,          NV2A_DEPTH_FUNC, D3DRS_ZFUNC },
	{ "D3DRS_ALPHAFUNC"                   /*=  58*/, 3424, xtD3DCMPFUNC,          NV2A_ALPHA_FUNC_FUNC, D3DRS_ALPHAFUNC },
	{ "D3DRS_ALPHABLENDENABLE"            /*=  59*/, 3424, xtBOOL,                NV2A_BLEND_FUNC_ENABLE, D3DRS_ALPHABLENDENABLE, "TRUE to enable alpha blending" },
	{ "D3DRS_ALPHATESTENABLE"             /*=  60*/, 3424, xtBOOL,                NV2A_ALPHA_FUNC_ENABLE, D3DRS_ALPHATESTENABLE, "TRUE to enable alpha tests" },
	{ "D3DRS_ALPHAREF"                    /*=  61*/, 3424, xtBYTE,                NV2A_ALPHA_FUNC_REF, D3DRS_ALPHAREF },
	{ "D3DRS_SRCBLEND"                    /*=  62*/, 3424, xtD3DBLEND,            NV2A_BLEND_FUNC_SRC, D3DRS_SRCBLEND },
	{ "D3DRS_DESTBLEND"                   /*=  63*/, 3424, xtD3DBLEND,            NV2A_BLEND_FUNC_DST, D3DRS_DESTBLEND },
	{ "D3DRS_ZWRITEENABLE"                /*=  64*/, 3424, xtBOOL,                NV2A_DEPTH_WRITE_ENABLE, D3DRS_ZWRITEENABLE, "TRUE to enable Z writes" },
	{ "D3DRS_DITHERENABLE"                /*=  65*/, 3424, xtBOOL,                NV2A_DITHER_ENABLE, D3DRS_DITHERENABLE, "TRUE to enable dithering" },
	{ "D3DRS_SHADEMODE"                   /*=  66*/, 3424, xtD3DSHADEMODE,        NV2A_SHADE_MODEL, D3DRS_SHADEMODE },
	{ "D3DRS_COLORWRITEENABLE"            /*=  67*/, 3424, xtD3DCOLORWRITEENABLE, NV2A_COLOR_MASK, D3DRS_COLORWRITEENABLE }, // *_ALPHA, etc. per-channel write enable
	{ "D3DRS_STENCILZFAIL"                /*=  68*/, 3424, xtD3DSTENCILOP,        NV2A_STENCIL_OP_ZFAIL, D3DRS_STENCILZFAIL, "Operation to do if stencil test passes and Z test fails" },
	{ "D3DRS_STENCILPASS"                 /*=  69*/, 3424, xtD3DSTENCILOP,        NV2A_STENCIL_OP_ZPASS, D3DRS_STENCILPASS, "Operation to do if both stencil and Z tests pass" },
	{ "D3DRS_STENCILFUNC"                 /*=  70*/, 3424, xtD3DCMPFUNC,          NV2A_STENCIL_FUNC_FUNC, D3DRS_STENCILFUNC },
	{ "D3DRS_STENCILREF"                  /*=  71*/, 3424, xtBYTE,                NV2A_STENCIL_FUNC_REF, D3DRS_STENCILREF, "BYTE reference value used in stencil test" },
	{ "D3DRS_STENCILMASK"                 /*=  72*/, 3424, xtBYTE,                NV2A_STENCIL_FUNC_MASK, D3DRS_STENCILMASK, "BYTE mask value used in stencil test" },
	{ "D3DRS_STENCILWRITEMASK"            /*=  73*/, 3424, xtBYTE,                NV2A_STENCIL_MASK, D3DRS_STENCILWRITEMASK, "BYTE write mask applied to values written to stencil buffer" },
	{ "D3DRS_BLENDOP"                     /*=  74*/, 3424, xtD3DBLENDOP,          NV2A_BLEND_EQUATION, D3DRS_BLENDOP },
	{ "D3DRS_BLENDCOLOR"                  /*=  75*/, 3424, xtD3DCOLOR,            NV2A_BLEND_COLOR, D3DRS_BLENDFACTOR, "D3DCOLOR for D3DBLEND_CONSTANTCOLOR" },
	// D3D9 D3DRS_BLENDFACTOR : D3DCOLOR used for a constant blend factor during alpha blending for devices that support D3DPBLENDCAPS_BLENDFACTOR
	{ "D3DRS_SWATHWIDTH"                  /*=  76*/, 3424, xtD3DSWATH,            NV2A_SWATH_WIDTH },
	{ "D3DRS_POLYGONOFFSETZSLOPESCALE"    /*=  77*/, 3424, xtFloat,               NV2A_POLYGON_OFFSET_FACTOR, D3DRS_UNSUPPORTED, "float Z factor for shadow maps" },
	{ "D3DRS_POLYGONOFFSETZOFFSET"        /*=  78*/, 3424, xtFloat,               NV2A_POLYGON_OFFSET_UNITS },
	{ "D3DRS_POINTOFFSETENABLE"           /*=  79*/, 3424, xtBOOL,                NV2A_POLYGON_OFFSET_POINT_ENABLE },
	{ "D3DRS_WIREFRAMEOFFSETENABLE"       /*=  80*/, 3424, xtBOOL,                NV2A_POLYGON_OFFSET_LINE_ENABLE },
	{ "D3DRS_SOLIDOFFSETENABLE"           /*=  81*/, 3424, xtBOOL,                NV2A_POLYGON_OFFSET_FILL_ENABLE },
	{ "D3DRS_DEPTHCLIPCONTROL"            /*=  82*/, 4432, xtD3DDCC,              NV2A_DEPTHCLIPCONTROL }, // Verified absent in 4361, present in 4432  TODO : Might be introduced around 4400?
	{ "D3DRS_STIPPLEENABLE"               /*=  83*/, 4627, xtBOOL,                NV2A_POLYGON_STIPPLE_ENABLE }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_SIMPLE_UNUSED8"              /*=  84*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_SIMPLE_UNUSED7"              /*=  85*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_SIMPLE_UNUSED6"              /*=  86*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_SIMPLE_UNUSED5"              /*=  87*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_SIMPLE_UNUSED4"              /*=  88*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_SIMPLE_UNUSED3"              /*=  89*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_SIMPLE_UNUSED2"              /*=  90*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_SIMPLE_UNUSED1"              /*=  91*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	// End of "simple" render states, continuing with "deferred" render states :
	// Verified as XDK 3911 Deferred RenderStates (3424 yet to do)
	{ "D3DRS_FOGENABLE"                   /*=  92*/, 3424, xtBOOL,                NV2A_FOG_ENABLE, D3DRS_FOGENABLE }, // TRUE to enable fog blending
	{ "D3DRS_FOGTABLEMODE"                /*=  93*/, 3424, xtD3DFOGMODE,          NV2A_FOG_MODE, D3DRS_FOGTABLEMODE }, // D3DFOGMODE
	{ "D3DRS_FOGSTART"                    /*=  94*/, 3424, xtFloat,               NV2A_FOG_COORD_DIST, D3DRS_FOGSTART }, // float fog start (for both vertex and pixel fog)
	{ "D3DRS_FOGEND"                      /*=  95*/, 3424, xtFloat,               NV2A_FOG_MODE, D3DRS_FOGEND }, // float fog end
	{ "D3DRS_FOGDENSITY"                  /*=  96*/, 3424, xtFloat,               NV2A_FOG_EQUATION_CONSTANT, D3DRS_FOGDENSITY }, // float fog density // + NV2A_FOG_EQUATION_LINEAR + NV2A_FOG_EQUATION_QUADRATIC
	{ "D3DRS_RANGEFOGENABLE"              /*=  97*/, 3424, xtBOOL,                NV2A_FOG_COORD_DIST, D3DRS_RANGEFOGENABLE }, // TRUE to enable range-based fog
	{ "D3DRS_WRAP0"                       /*=  98*/, 3424, xtD3DWRAP,             NV2A_TX_WRAP(0), D3DRS_WRAP0 }, // D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 1st texture coord.
	{ "D3DRS_WRAP1"                       /*=  99*/, 3424, xtD3DWRAP,             NV2A_TX_WRAP(1), D3DRS_WRAP1 }, // D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 2nd texture coord.
	{ "D3DRS_WRAP2"                       /*= 100*/, 3424, xtD3DWRAP,             NV2A_TX_WRAP(2), D3DRS_WRAP2 }, // D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 3rd texture coord.
	{ "D3DRS_WRAP3"                       /*= 101*/, 3424, xtD3DWRAP,             NV2A_TX_WRAP(3), D3DRS_WRAP3 }, // D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 4th texture coord.
	{ "D3DRS_LIGHTING"                    /*= 102*/, 3424, xtBOOL,                NV2A_LIGHT_MODEL, D3DRS_LIGHTING }, // TRUE to enable lighting // TODO : Needs push-buffer data conversion
	{ "D3DRS_SPECULARENABLE"              /*= 103*/, 3424, xtBOOL,                NV2A_RC_FINAL0, D3DRS_SPECULARENABLE }, // TRUE to enable specular
	{ "D3DRS_LOCALVIEWER"                 /*= 104*/, 3424, xtBOOL,                0, D3DRS_LOCALVIEWER }, // TRUE to enable camera-relative specular highlights
	{ "D3DRS_COLORVERTEX"                 /*= 105*/, 3424, xtBOOL,                0, D3DRS_COLORVERTEX }, // TRUE to enable per-vertex color
	{ "D3DRS_BACKSPECULARMATERIALSOURCE"  /*= 106*/, 3424, xtD3DMCS,              0 }, // D3DMATERIALCOLORSOURCE (Xbox extension) nsp.
	{ "D3DRS_BACKDIFFUSEMATERIALSOURCE"   /*= 107*/, 3424, xtD3DMCS,              0 }, // D3DMATERIALCOLORSOURCE (Xbox extension) nsp.
	{ "D3DRS_BACKAMBIENTMATERIALSOURCE"   /*= 108*/, 3424, xtD3DMCS,              0 }, // D3DMATERIALCOLORSOURCE (Xbox extension) nsp.
	{ "D3DRS_BACKEMISSIVEMATERIALSOURCE"  /*= 109*/, 3424, xtD3DMCS,              0 }, // D3DMATERIALCOLORSOURCE (Xbox extension) nsp.
	{ "D3DRS_SPECULARMATERIALSOURCE"      /*= 110*/, 3424, xtD3DMCS,              NV2A_COLOR_MATERIAL, D3DRS_SPECULARMATERIALSOURCE }, // D3DMATERIALCOLORSOURCE
	{ "D3DRS_DIFFUSEMATERIALSOURCE"       /*= 111*/, 3424, xtD3DMCS,              0, D3DRS_DIFFUSEMATERIALSOURCE }, // D3DMATERIALCOLORSOURCE
	{ "D3DRS_AMBIENTMATERIALSOURCE"       /*= 112*/, 3424, xtD3DMCS,              0, D3DRS_AMBIENTMATERIALSOURCE }, // D3DMATERIALCOLORSOURCE
	{ "D3DRS_EMISSIVEMATERIALSOURCE"      /*= 113*/, 3424, xtD3DMCS,              0, D3DRS_EMISSIVEMATERIALSOURCE }, // D3DMATERIALCOLORSOURCE
	{ "D3DRS_BACKAMBIENT"                 /*= 114*/, 3424, xtD3DCOLOR,            NV2A_LIGHT_MODEL_BACK_SIDE_PRODUCT_AMBIENT_PLUS_EMISSION_R }, // D3DCOLOR (Xbox extension) // ..NV2A_MATERIAL_FACTOR_BACK_B nsp. Was NV2A_LIGHT_MODEL_BACK_AMBIENT_R
	{ "D3DRS_AMBIENT"                     /*= 115*/, 3424, xtD3DCOLOR,            NV2A_LIGHT_MODEL_FRONT_SIDE_PRODUCT_AMBIENT_PLUS_EMISSION_R, D3DRS_AMBIENT }, // D3DCOLOR // ..NV2A_LIGHT_MODEL_FRONT_AMBIENT_B + NV2A_MATERIAL_FACTOR_FRONT_R..NV2A_MATERIAL_FACTOR_FRONT_A  Was NV2A_LIGHT_MODEL_FRONT_AMBIENT_R
	{ "D3DRS_POINTSIZE"                   /*= 116*/, 3424, xtFloat,               NV2A_POINT_PARAMETER(0), D3DRS_POINTSIZE }, // float point size
	{ "D3DRS_POINTSIZE_MIN"               /*= 117*/, 3424, xtFloat,               0, D3DRS_POINTSIZE_MIN }, // float point size min threshold
	{ "D3DRS_POINTSPRITEENABLE"           /*= 118*/, 3424, xtBOOL,                NV2A_POINT_SMOOTH_ENABLE, D3DRS_POINTSPRITEENABLE }, // TRUE to enable point sprites
	{ "D3DRS_POINTSCALEENABLE"            /*= 119*/, 3424, xtBOOL,                NV2A_POINT_PARAMETERS_ENABLE, D3DRS_POINTSCALEENABLE }, // TRUE to enable point size scaling
	{ "D3DRS_POINTSCALE_A"                /*= 120*/, 3424, xtFloat,               0, D3DRS_POINTSCALE_A }, // float point attenuation A value
	{ "D3DRS_POINTSCALE_B"                /*= 121*/, 3424, xtFloat,               0, D3DRS_POINTSCALE_B }, // float point attenuation B value
	{ "D3DRS_POINTSCALE_C"                /*= 122*/, 3424, xtFloat,               0, D3DRS_POINTSCALE_C }, // float point attenuation C value
	{ "D3DRS_POINTSIZE_MAX"               /*= 123*/, 3424, xtFloat,               0, D3DRS_POINTSIZE_MAX }, // float point size max threshold
	{ "D3DRS_PATCHEDGESTYLE"              /*= 124*/, 3424, xtDWORD,               0, D3DRS_PATCHEDGESTYLE }, // D3DPATCHEDGESTYLE
	{ "D3DRS_PATCHSEGMENTS"               /*= 125*/, 3424, xtDWORD,               0 }, // DWORD number of segments per edge when drawing patches, nsp (D3DRS_PATCHSEGMENTS exists in Direct3D 8, but not in 9)
	// TODO -oDxbx : Is X_D3DRS_SWAPFILTER really a xtD3DMULTISAMPLE_TYPE?
	{ "D3DRS_SWAPFILTER"                  /*= 126*/, 4034, xtD3DMULTISAMPLE_TYPE, 0, D3DRS_UNSUPPORTED, "D3DTEXF_LINEAR etc. filter to use for Swap" }, // nsp. Verified absent in 3944, present in 4034.  4034 state based on test-case : The Simpsons Road Rage
	{ "D3DRS_PRESENTATIONINTERVAL"        /*= 127*/, 4627, xtDWORD,               0 }, // nsp. Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DEFERRED_UNUSED8"            /*= 128*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DEFERRED_UNUSED7"            /*= 129*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DEFERRED_UNUSED6"            /*= 130*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DEFERRED_UNUSED5"            /*= 131*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DEFERRED_UNUSED4"            /*= 132*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DEFERRED_UNUSED3"            /*= 133*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DEFERRED_UNUSED2"            /*= 134*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DEFERRED_UNUSED1"            /*= 135*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	// End of "deferred" render states, continuing with "complex" render states :
	{ "D3DRS_PSTEXTUREMODES"              /*= 136*/, 3424, xtDWORD,               0 }, // This is where pPSDef->PSTextureModes is stored (outside the pPSDEF - see DxbxUpdateActivePixelShader)
	{ "D3DRS_VERTEXBLEND"                 /*= 137*/, 3424, xtD3DVERTEXBLENDFLAGS, NV2A_SKIN_MODE, D3DRS_VERTEXBLEND },
	{ "D3DRS_FOGCOLOR"                    /*= 138*/, 3424, xtD3DCOLOR,            NV2A_FOG_COLOR, D3DRS_FOGCOLOR }, // SwapRgb
	{ "D3DRS_FILLMODE"                    /*= 139*/, 3424, xtD3DFILLMODE,         NV2A_POLYGON_MODE_FRONT, D3DRS_FILLMODE },
	{ "D3DRS_BACKFILLMODE"                /*= 140*/, 3424, xtD3DFILLMODE,         NV2A_POLYGON_MODE_BACK }, // nsp.
	{ "D3DRS_TWOSIDEDLIGHTING"            /*= 141*/, 3424, xtBOOL,                0 }, // nsp.  // FIXME map from NV2A_LIGHT_MODEL
	{ "D3DRS_NORMALIZENORMALS"            /*= 142*/, 3424, xtBOOL,                NV2A_NORMALIZE_ENABLE, D3DRS_NORMALIZENORMALS },
	{ "D3DRS_ZENABLE"                     /*= 143*/, 3424, xtBOOL,                NV2A_DEPTH_TEST_ENABLE, D3DRS_ZENABLE }, // D3DZBUFFERTYPE?
	{ "D3DRS_STENCILENABLE"               /*= 144*/, 3424, xtBOOL,                NV2A_STENCIL_ENABLE, D3DRS_STENCILENABLE },
	{ "D3DRS_STENCILFAIL"                 /*= 145*/, 3424, xtD3DSTENCILOP,        NV2A_STENCIL_OP_FAIL, D3DRS_STENCILFAIL },
	{ "D3DRS_FRONTFACE"                   /*= 146*/, 3424, xtD3DFRONT,            NV2A_FRONT_FACE }, // nsp.
	{ "D3DRS_CULLMODE"                    /*= 147*/, 3424, xtD3DCULL,             NV2A_CULL_FACE, D3DRS_CULLMODE },
	{ "D3DRS_TEXTUREFACTOR"               /*= 148*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(0), D3DRS_TEXTUREFACTOR },
	{ "D3DRS_ZBIAS"                       /*= 149*/, 3424, xtLONG,                0, D3DRS_DEPTHBIAS }, // Was D3DRS_ZBIAS
	{ "D3DRS_LOGICOP"                     /*= 150*/, 3424, xtD3DLOGICOP,          NV2A_COLOR_LOGIC_OP_OP }, // nsp.
	{ "D3DRS_EDGEANTIALIAS"               /*= 151*/, 3424, xtBOOL,                NV2A_LINE_SMOOTH_ENABLE, D3DRS_ANTIALIASEDLINEENABLE }, // Was D3DRS_EDGEANTIALIAS. Dxbx note : No Xbox ext. (according to Direct3D8) !
	{ "D3DRS_MULTISAMPLEANTIALIAS"        /*= 152*/, 3424, xtBOOL,                NV2A_MULTISAMPLE_CONTROL, D3DRS_MULTISAMPLEANTIALIAS },
	{ "D3DRS_MULTISAMPLEMASK"             /*= 153*/, 3424, xtDWORD,               NV2A_MULTISAMPLE_CONTROL, D3DRS_MULTISAMPLEMASK },
	{ "D3DRS_MULTISAMPLETYPE"             /*= 154*/, 3424, xtD3DMULTISAMPLE_TYPE, 0, D3DRS_UNSUPPORTED, "aliasses D3DMULTISAMPLE_TYPE, removed from 4034 onward", 4034 }, // Verified present in 3944, removed in 4034. 4034 state based on test-case : The Simpsons Road Rage
	{ "D3DRS_MULTISAMPLEMODE"             /*= 155*/, 4034, xtD3DMULTISAMPLEMODE,  0 }, // D3DMULTISAMPLEMODE for the backbuffer. Verified absent in 3944, present in 4034.  4034 state based on test-case : The Simpsons Road Rage
	{ "D3DRS_MULTISAMPLERENDERTARGETMODE" /*= 156*/, 4034, xtD3DMULTISAMPLEMODE,  NV2A_RT_FORMAT }, // Verified absent in 3944, present in 4034. Presence in 4034 is based on test-case : The Simpsons Road Rage
	{ "D3DRS_SHADOWFUNC"                  /*= 157*/, 3424, xtD3DCMPFUNC,          NV2A_TX_RCOMP },
	{ "D3DRS_LINEWIDTH"                   /*= 158*/, 3424, xtFloat,               NV2A_LINE_WIDTH },
	{ "D3DRS_SAMPLEALPHA"                 /*= 159*/, 4627, xtD3DSAMPLEALPHA,      0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DXT1NOISEENABLE"             /*= 160*/, 3424, xtBOOL,                NV2A_CLEAR_DEPTH_VALUE },
	{ "D3DRS_YUVENABLE"                   /*= 161*/, 3911, xtBOOL,                NV2A_CONTROL0 }, // Verified present in 3944
	{ "D3DRS_OCCLUSIONCULLENABLE"         /*= 162*/, 3911, xtBOOL,                NV2A_OCCLUDE_ZSTENCIL_EN }, // Verified present in 3944
	{ "D3DRS_STENCILCULLENABLE"           /*= 163*/, 3911, xtBOOL,                NV2A_OCCLUDE_ZSTENCIL_EN }, // Verified present in 3944
	{ "D3DRS_ROPZCMPALWAYSREAD"           /*= 164*/, 3911, xtBOOL,                0 }, // Verified present in 3944
	{ "D3DRS_ROPZREAD"                    /*= 165*/, 3911, xtBOOL,                0 }, // Verified present in 3944
	{ "D3DRS_DONOTCULLUNCOMPRESSED"       /*= 166*/, 3911, xtBOOL,                0 } // Verified present in 3944
};

bool IsRenderStateAvailableInCurrentXboxD3D8Lib(RenderStateInfo& aRenderStateInfo)
{
	bool bIsRenderStateAvailable = (aRenderStateInfo.V <= X86_CODE_D3D8V);
	if (aRenderStateInfo.R > 0) { // Applies to xbox::X_D3DRS_MULTISAMPLETYPE
		// Note : X_D3DRS_MULTISAMPLETYPE seems the only render state that got
		// removed (from 4039 onwards), so we check that limitation here as well
		bIsRenderStateAvailable &= (X86_CODE_D3D8V < aRenderStateInfo.R);
	}


	return bIsRenderStateAvailable;
}

const RenderStateInfo& GetDxbxRenderStateInfo(int32_t State)
{
	return DxbxRenderStateInfo[State];
}


void BuildRenderStateMappingTable()
{
	g_RenderStateOffsets.fill(-1);

	int32_t XboxIndex = 0;
	for (uint32_t RenderState = X_D3DRS_FIRST; RenderState <= X_D3DRS_LAST; RenderState++)
	{
		auto RenderStateInfo = GetDxbxRenderStateInfo(RenderState);

		if (IsRenderStateAvailableInCurrentXboxD3D8Lib(RenderStateInfo))
		{
			g_RenderStateOffsets[RenderState] = XboxIndex;
			XboxIndex++;
		}
	}
	g_renderStateCount = XboxIndex;
	g_renderStateSnapshotValid = false;
}

void* GetPixelShaderRenderStatePointer()
{
	return &g_renderState[X_D3DRS_PS_FIRST];
}


static void D3DDevice_SetRenderState_Simple(int32_t Method, int32_t Value)
{
	//Fetch the RenderState conversion info for the given input
	int32_t GuestRenderStateIndex = -1;

	for (int32_t i = X_D3DRS_PS_FIRST; i <= X_D3DRS_LAST; i++)
	{
		if (GetDxbxRenderStateInfo(i).M == PUSH_METHOD(Method))
		{
			GuestRenderStateIndex = i;
			break;
		}
	}

	if (GuestRenderStateIndex == -1) {
		XANTHUS_WARN("RenderState_Simple could not be found in RenderState table Method {:x}, Value {:x}", Method, Value);
		return;
	}

	SetGuestRenderState(GuestRenderStateIndex, Value);
}


bool GuestRenderStateExists(uint32_t State)
{
	if (g_RenderStateOffsets[State] >= 0) {
		return true;
	}

	return false;
}

float GetGuestRenderStateAsFloat(uint32_t State)
{
	return *reinterpret_cast<float*>(&(g_renderState[g_RenderStateOffsets[State]]));
}

void SetGuestRenderState(uint32_t State, uint32_t Value)
{
	if (!GuestRenderStateExists(State)) 
	{
		return;
	}


	g_renderState[g_RenderStateOffsets[State]] = Value;
}

uint32_t GetGuestRenderState(uint32_t State)
{
	if (GuestRenderStateExists(State) == false)
	{
		return 0;
	}

	return g_renderState[g_RenderStateOffsets[State]];
}


void UpdateAllRenderState()
{
	// Iterate through each RenderState and set the associated host render state
	// We start counting at X_D3DRS_SIMPLE_FIRST, to skip the pixel shader renderstates handled elsewhere
	if (g_renderStateSnapshotValid &&
		memcmp(g_renderState, g_renderStateSnapshot.data(), g_renderStateCount * sizeof(uint32_t)) == 0)
		return;

	for (uint32_t RenderState = X_D3DRS_SIMPLE_FIRST; RenderState <= X_D3DRS_LAST; RenderState++)
	{
		auto Value = GetGuestRenderState(RenderState);

		if (GuestRenderStateExists(RenderState) == false || Value == g_dirty_render_states[RenderState] || RenderState == X_D3DRS_PSTEXTUREMODES)
		{
			continue;
		}

		//Set the host render state
		// Specular enable is baked into the cached light block
		if (RenderState == X_D3DRS_SPECULARENABLE)
			MarkFixedFunctionVertexShaderLightsDirty();

		switch (RenderState)
		{
		// Fixed-function states that only refresh uploaded shader constants.
		case X_D3DRS_FOGSTART:
		case X_D3DRS_FOGEND:
		case X_D3DRS_FOGDENSITY:
		case X_D3DRS_AMBIENT:
		case X_D3DRS_BACKAMBIENT:
		case X_D3DRS_POINTSIZE:
		case X_D3DRS_POINTSIZE_MIN:
		case X_D3DRS_POINTSIZE_MAX:
		case X_D3DRS_POINTSCALE_A:
		case X_D3DRS_POINTSCALE_B:
		case X_D3DRS_POINTSCALE_C:
			MarkFixedFunctionVertexShaderStateDirty();
			break;
		// Fixed-function states that can also select a different specialized
		// vertex shader variant.
		case X_D3DRS_FOGENABLE:
		case X_D3DRS_FOGTABLEMODE:
		case X_D3DRS_RANGEFOGENABLE:
		case X_D3DRS_LIGHTING:
		case X_D3DRS_SPECULARENABLE:
		case X_D3DRS_LOCALVIEWER:
		case X_D3DRS_COLORVERTEX:
		case X_D3DRS_SPECULARMATERIALSOURCE:
		case X_D3DRS_DIFFUSEMATERIALSOURCE:
		case X_D3DRS_AMBIENTMATERIALSOURCE:
		case X_D3DRS_EMISSIVEMATERIALSOURCE:
		case X_D3DRS_POINTSPRITEENABLE:
		case X_D3DRS_POINTSCALEENABLE:
		case X_D3DRS_VERTEXBLEND:
		case X_D3DRS_TWOSIDEDLIGHTING:
		case X_D3DRS_NORMALIZENORMALS:
			MarkFixedFunctionVertexShaderVariantDirty();
			break;
		default:
			break;
		}
		SetRenderState((X_D3DRENDERSTATETYPE)RenderState, Value);
		g_dirty_render_states[RenderState] = Value;
	}

	memcpy(g_renderStateSnapshot.data(), g_renderState, g_renderStateCount * sizeof(uint32_t));
	g_renderStateSnapshotValid = true;
}

void InitGuestRenderStatePointer(uint32_t* ptr)
{
	g_renderState = (uint32_t*)ptr;
}



GUEST_FUNCTION_HOOK_FASTCALL(sub_3D2420, D3DDevice_SetRenderState_Simple);