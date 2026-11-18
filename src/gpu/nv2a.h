#pragma once

// NV2A Push buffer command masks
#define PUSH_TYPE_MASK         0x00000002 // 2 bits
#define PUSH_TYPE_SHIFT        0
#define PUSH_TYPE_METHOD        0 // method
#define PUSH_TYPE_JMP_FAR       1 // jump far
#define PUSH_TYPE_CALL_FAR      2 // call far
#define PUSH_TYPE_METHOD_UNUSED 3 // method (unused)
#define PUSH_METHOD_MASK       0x00001FFC // 12 bits
#define PUSH_METHOD_SHIFT      0 // Dxbx note : Not 2, because methods are actually DWORD offsets (and thus defined with increments of 4)
#define PUSH_SUBCH_MASK        0x0000E000 // 3 bits
#define PUSH_SUBCH_SHIFT       13
#define PUSH_COUNT_MASK        0x1FFC0000 // 11 bits
#define PUSH_COUNT_SHIFT       18
#define PUSH_INSTR_MASK        0xE0000000 // 3 bits
#define PUSH_INSTR_SHIFT       29
#define PUSH_INSTR_IMM_INCR     0 // immediate, increment
#define PUSH_INSTR_JMP_NEAR     1 // near jump
#define PUSH_INSTR_IMM_NOINC    2 // immediate, no-increment
#define PUSH_ADDR_FAR_MASK     0xFFFFFFFC // 30 bits
#define PUSH_ADDR_FAR_SHIFT    0
#define PUSH_ADDR_NEAR_MASK    0x1FFFFFFC // 27 bits
#define PUSH_ADDR_NEAR_SHIFT   0 // Cxbx note : Not 2, because methods are actually DWORD offsets (and thus defined with increments of 4)

#define PUSH_TYPE(dwPushCommand) ((dwPushCommand & PUSH_TYPE_MASK) >> PUSH_TYPE_SHIFT)
#define PUSH_METHOD(dwPushCommand) ((dwPushCommand & PUSH_METHOD_MASK) >> PUSH_METHOD_SHIFT)
#define PUSH_SUBCH(dwPushCommand) ((dwPushCommand & PUSH_SUBCH_MASK) >> PUSH_SUBCH_SHIFT)
#define PUSH_COUNT(dwPushCommand) ((dwPushCommand & PUSH_COUNT_MASK) >> PUSH_COUNT_SHIFT)
#define PUSH_INSTR(dwPushCommand) ((dwPushCommand & PUSH_INSTR_MASK) >> PUSH_INSTR_SHIFT)
#define PUSH_ADDR_FAR(dwPushCommand) ((dwPushCommand & PUSH_ADDR_FAR_MASK) >> PUSH_ADDR_FAR_SHIFT)
#define PUSH_ADDR_NEAR(dwPushCommand) ((dwPushCommand & PUSH_ADDR_NEAR_MASK) >> PUSH_ADDR_NEAR_SHIFT)

#define PUSH_METHOD_MAX ((PUSH_METHOD_MASK | 3) >> PUSH_METHOD_SHIFT) // = 8191
#define PUSH_SUBCH_MAX (PUSH_SUBCH_MASK >> PUSH_SUBCH_SHIFT) // = 7
#define PUSH_COUNT_MAX (PUSH_COUNT_MASK >> PUSH_COUNT_SHIFT) // = 2047



#define NV2A_VERTEXSHADER_ATTRIBUTES 16


#define NV2A_LTCTXA_COUNT  26
#define NV2A_LTCTXB_COUNT  52
#define NV2A_LTC1_COUNT    20
#define NV2A_MAX_LIGHTS 8
#define NV2A_MAX_BATCH_LENGTH 0x1FFFF
#define NV_PGRAPH_SIZE              0x002000

// Decode push buffer command (inverse of D3DPUSH_ENCODE)
inline void D3DPUSH_DECODE(const DWORD dwPushCommand, DWORD& dwMethod, DWORD& dwSubCh, DWORD& dwCount)
{
	dwMethod = PUSH_METHOD(dwPushCommand);
	dwSubCh = PUSH_SUBCH(dwPushCommand);
	dwCount = PUSH_COUNT(dwPushCommand);
}

/// nv2a microcode header
typedef struct
{
	uint16_t Version; // See VERSION_XVS*
	uint16_t NumInst;
}
X_VSH_SHADER_HEADER;



typedef struct VertexAttribute {
	bool dma_select;
	uint32_t offset;

	/* inline arrays are packed in order?
	* Need to pass the offset to converted attributes */
	unsigned int inline_array_offset;

	float inline_value[4];

	unsigned int format;
	unsigned int size; /* size of the data type */
	unsigned int count; /* number of components */
	uint32_t stride;

	bool needs_conversion;
	uint8_t* converted_buffer;
	unsigned int converted_elements;
	unsigned int converted_size;
	unsigned int converted_count;

	float* inline_buffer;
} VertexAttribute;

typedef struct PGRAPHState {

	unsigned int primitive_mode;

	/* lighting constant arrays */
	uint32_t ltctxa[NV2A_LTCTXA_COUNT][4];
	bool ltctxa_dirty[NV2A_LTCTXA_COUNT];
	uint32_t ltctxb[NV2A_LTCTXB_COUNT][4];
	bool ltctxb_dirty[NV2A_LTCTXB_COUNT];
	uint32_t ltc1[NV2A_LTC1_COUNT][4];
	bool ltc1_dirty[NV2A_LTC1_COUNT];

	// should figure out where these are in lighting context
	float light_infinite_half_vector[NV2A_MAX_LIGHTS][3];
	float light_infinite_direction[NV2A_MAX_LIGHTS][3];
	float light_local_position[NV2A_MAX_LIGHTS][3];
	float light_local_attenuation[NV2A_MAX_LIGHTS][3];

	VertexAttribute vertex_attributes[NV2A_VERTEXSHADER_ATTRIBUTES];

	unsigned int inline_array_length;
	uint32_t inline_array[NV2A_MAX_BATCH_LENGTH];

	unsigned int inline_elements_length;
	uint16_t inline_elements[NV2A_MAX_BATCH_LENGTH]; // Cxbx-Reloaded TODO : Restore uint32_t once HLE_draw_inline_elements can using that

	unsigned int inline_buffer_length;

	unsigned int draw_arrays_length;
	unsigned int draw_arrays_max_count;

	uint32_t regs[NV_PGRAPH_SIZE]; // TODO : union
} PGRAPHState;

typedef struct NV2AState {
	void(*vblank_cb)(void*, X86Context*);
	uint64_t vblank_last;

	bool exiting;

	// MemoryRegion ramin;
	struct {
		uint8_t* ramin_ptr;
		size_t ramin_size;
	} pramin;

	struct {
		uint32_t pending_interrupts;
		uint32_t enabled_interrupts;
		uint32_t regs[0x001000];
	} pmc;

	struct PGRAPHState pgraph;

	struct {
		uint32_t pending_interrupts;
		uint32_t enabled_interrupts;
		uint32_t start;
		uint32_t regs[0x001000];
	} pcrtc;



} NV2AState;

extern std::unique_ptr<NV2AState> m_nv2a_state;

#define NV_PGRAPH_CTX_CACHE1                             0x00000160
#define NV_PGRAPH_CTX_CACHE2                             0x00000180
#define NV_PGRAPH_CTX_CACHE3                             0x000001A0
#define NV_PGRAPH_CTX_CACHE4                             0x000001C0
#define NV_PGRAPH_CTX_CACHE5                             0x000001E0


// GCC implementation of FFS
static int nv2a_ffs(int valu)
{
	int bit;

	if (valu == 0)
		return 0;

	for (bit = 1; !(valu & 1); bit++)
		valu >>= 1;

	return bit;
}

#define GET_MASK(v, mask) (((v) & (mask)) >> (nv2a_ffs(mask)-1))

#define NV_PGRAPH_CTX_SWITCH1                            0x0000014C
#define NV_PGRAPH_CTX_SWITCH1_GRCLASS                      0x000000FF
#define NV_PGRAPH_CTX_SWITCH2                            0x00000150
#define NV_PGRAPH_CTX_SWITCH3                            0x00000154
#define NV_PGRAPH_CTX_SWITCH4                            0x00000158
#define NV_PGRAPH_CTX_SWITCH5                            0x0000015C


#define NV_PGRAPH_CONTROL_0                              0x0000194C
#define NV_PGRAPH_CONTROL_1                              0x00001950
#define NV_PGRAPH_CONTROL_0_ZENABLE                          (1 << 14)
#define NV097_SET_BEGIN_END_OP_END                         0x00
#define NV_PGRAPH_CONTROL_1_STENCIL_TEST_ENABLE              (1 << 0)
#define NV_KELVIN_PRIMITIVE                              0x0097

#define NV_PGRAPH_CTX_CONTROL                            0x00000144
#define NV_PGRAPH_CTX_CONTROL_CHID                          (1 << 16)
#define NV_PGRAPH_CTX_USER                               0x00000148
#define NV_PGRAPH_CTX_USER_CHID                            0x1F000000

#define NV097_INLINE_ARRAY                                 0x00001818
#define NV2A_MAX_BATCH_LENGTH 0x1FFFF

#   define NV097_ARRAY_ELEMENT16                              0x00001800
#   define NV097_ARRAY_ELEMENT32                              0x00001808

#define NV_SET_OBJECT                                        0x00000000

//so far, it looks like Shadow only use that
#define NV097_SET_BEGIN_END                                0x000017FC