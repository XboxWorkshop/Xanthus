#pragma once

#include "variant_000.hlsl.spirv.h"
#include "variant_001.hlsl.spirv.h"
#include "variant_002.hlsl.spirv.h"
#include "variant_003.hlsl.spirv.h"
#include "variant_004.hlsl.spirv.h"
#include "variant_005.hlsl.spirv.h"
#include "variant_006.hlsl.spirv.h"
#include "variant_007.hlsl.spirv.h"
#include "variant_008.hlsl.spirv.h"
#include "variant_009.hlsl.spirv.h"
#include "variant_010.hlsl.spirv.h"
#include "variant_011.hlsl.spirv.h"
#include "variant_012.hlsl.spirv.h"
#include "variant_013.hlsl.spirv.h"
#include "variant_014.hlsl.spirv.h"
#include "variant_015.hlsl.spirv.h"
#include "variant_016.hlsl.spirv.h"
#include "variant_017.hlsl.spirv.h"
#include "variant_018.hlsl.spirv.h"
#include "variant_019.hlsl.spirv.h"
#include "variant_020.hlsl.spirv.h"
#include "variant_021.hlsl.spirv.h"
#include "variant_022.hlsl.spirv.h"
#include "variant_023.hlsl.spirv.h"
#include "variant_024.hlsl.spirv.h"
#include "variant_025.hlsl.spirv.h"
#include "variant_026.hlsl.spirv.h"
#include "variant_027.hlsl.spirv.h"
#include "variant_028.hlsl.spirv.h"
#include "variant_029.hlsl.spirv.h"
#include "variant_030.hlsl.spirv.h"
#include "variant_031.hlsl.spirv.h"
#include "variant_032.hlsl.spirv.h"
#include "variant_033.hlsl.spirv.h"
#include "variant_034.hlsl.spirv.h"
#include "variant_035.hlsl.spirv.h"
#include "variant_036.hlsl.spirv.h"
#include "variant_037.hlsl.spirv.h"
#include "variant_038.hlsl.spirv.h"
#include "variant_039.hlsl.spirv.h"
#include "variant_040.hlsl.spirv.h"
#include "variant_041.hlsl.spirv.h"
#include "variant_042.hlsl.spirv.h"
#include "variant_043.hlsl.spirv.h"
#include "variant_044.hlsl.spirv.h"
#include "variant_045.hlsl.spirv.h"
#include "variant_046.hlsl.spirv.h"
#include "variant_047.hlsl.spirv.h"
#include "variant_048.hlsl.spirv.h"
#include "variant_049.hlsl.spirv.h"
#include "variant_050.hlsl.spirv.h"
#include "variant_051.hlsl.spirv.h"
#include "variant_052.hlsl.spirv.h"
#include "variant_053.hlsl.spirv.h"
#include "variant_054.hlsl.spirv.h"
#include "variant_055.hlsl.spirv.h"
#include "variant_056.hlsl.spirv.h"
#include "variant_057.hlsl.spirv.h"
#include "variant_058.hlsl.spirv.h"

#ifdef XANTHUS_D3D12
#include "variant_000.hlsl.dxil.h"
#include "variant_001.hlsl.dxil.h"
#include "variant_002.hlsl.dxil.h"
#include "variant_003.hlsl.dxil.h"
#include "variant_004.hlsl.dxil.h"
#include "variant_005.hlsl.dxil.h"
#include "variant_006.hlsl.dxil.h"
#include "variant_007.hlsl.dxil.h"
#include "variant_008.hlsl.dxil.h"
#include "variant_009.hlsl.dxil.h"
#include "variant_010.hlsl.dxil.h"
#include "variant_011.hlsl.dxil.h"
#include "variant_012.hlsl.dxil.h"
#include "variant_013.hlsl.dxil.h"
#include "variant_014.hlsl.dxil.h"
#include "variant_015.hlsl.dxil.h"
#include "variant_016.hlsl.dxil.h"
#include "variant_017.hlsl.dxil.h"
#include "variant_018.hlsl.dxil.h"
#include "variant_019.hlsl.dxil.h"
#include "variant_020.hlsl.dxil.h"
#include "variant_021.hlsl.dxil.h"
#include "variant_022.hlsl.dxil.h"
#include "variant_023.hlsl.dxil.h"
#include "variant_024.hlsl.dxil.h"
#include "variant_025.hlsl.dxil.h"
#include "variant_026.hlsl.dxil.h"
#include "variant_027.hlsl.dxil.h"
#include "variant_028.hlsl.dxil.h"
#include "variant_029.hlsl.dxil.h"
#include "variant_030.hlsl.dxil.h"
#include "variant_031.hlsl.dxil.h"
#include "variant_032.hlsl.dxil.h"
#include "variant_033.hlsl.dxil.h"
#include "variant_034.hlsl.dxil.h"
#include "variant_035.hlsl.dxil.h"
#include "variant_036.hlsl.dxil.h"
#include "variant_037.hlsl.dxil.h"
#include "variant_038.hlsl.dxil.h"
#include "variant_039.hlsl.dxil.h"
#include "variant_040.hlsl.dxil.h"
#include "variant_041.hlsl.dxil.h"
#include "variant_042.hlsl.dxil.h"
#include "variant_043.hlsl.dxil.h"
#include "variant_044.hlsl.dxil.h"
#include "variant_045.hlsl.dxil.h"
#include "variant_046.hlsl.dxil.h"
#include "variant_047.hlsl.dxil.h"
#include "variant_048.hlsl.dxil.h"
#include "variant_049.hlsl.dxil.h"
#include "variant_050.hlsl.dxil.h"
#include "variant_051.hlsl.dxil.h"
#include "variant_052.hlsl.dxil.h"
#include "variant_053.hlsl.dxil.h"
#include "variant_054.hlsl.dxil.h"
#include "variant_055.hlsl.dxil.h"
#include "variant_056.hlsl.dxil.h"
#include "variant_057.hlsl.dxil.h"
#include "variant_058.hlsl.dxil.h"
#endif

namespace FixedFunctionVertexShaderVariants::Generated
{
struct Program
{
    uint64_t key;
    const uint8_t* spirv;
    size_t spirvSize;
#ifdef XANTHUS_D3D12
    const uint8_t* dxil;
    size_t dxilSize;
#endif
};

static const Program Programs[] =
{
    {
        0x06D4C3C4C383B150ULL,
        g_variant_027_spirv, sizeof(g_variant_027_spirv),
#ifdef XANTHUS_D3D12
        g_variant_027_dxil, sizeof(g_variant_027_dxil),
#endif
    },
    {
        0x09860D0D70439218ULL,
        g_variant_056_spirv, sizeof(g_variant_056_spirv),
#ifdef XANTHUS_D3D12
        g_variant_056_dxil, sizeof(g_variant_056_dxil),
#endif
    },
    {
        0x0AB6EE9AE538EC75ULL,
        g_variant_008_spirv, sizeof(g_variant_008_spirv),
#ifdef XANTHUS_D3D12
        g_variant_008_dxil, sizeof(g_variant_008_dxil),
#endif
    },
    {
        0x1F49D54881EEE59DULL,
        g_variant_022_spirv, sizeof(g_variant_022_spirv),
#ifdef XANTHUS_D3D12
        g_variant_022_dxil, sizeof(g_variant_022_dxil),
#endif
    },
    {
        0x1FAB707A1EA2BCD9ULL,
        g_variant_016_spirv, sizeof(g_variant_016_spirv),
#ifdef XANTHUS_D3D12
        g_variant_016_dxil, sizeof(g_variant_016_dxil),
#endif
    },
    {
        0x331CE56F16602D01ULL,
        g_variant_004_spirv, sizeof(g_variant_004_spirv),
#ifdef XANTHUS_D3D12
        g_variant_004_dxil, sizeof(g_variant_004_dxil),
#endif
    },
    {
        0x33AF6A2016B00BFCULL,
        g_variant_044_spirv, sizeof(g_variant_044_spirv),
#ifdef XANTHUS_D3D12
        g_variant_044_dxil, sizeof(g_variant_044_dxil),
#endif
    },
    {
        0x36DD7448F8D8B481ULL,
        g_variant_024_spirv, sizeof(g_variant_024_spirv),
#ifdef XANTHUS_D3D12
        g_variant_024_dxil, sizeof(g_variant_024_dxil),
#endif
    },
    {
        0x3C62211EE30A3A3EULL,
        g_variant_036_spirv, sizeof(g_variant_036_spirv),
#ifdef XANTHUS_D3D12
        g_variant_036_dxil, sizeof(g_variant_036_dxil),
#endif
    },
    {
        0x3EFE089C9AA220BCULL,
        g_variant_003_spirv, sizeof(g_variant_003_spirv),
#ifdef XANTHUS_D3D12
        g_variant_003_dxil, sizeof(g_variant_003_dxil),
#endif
    },
    {
        0x4281AB6AE25E866BULL,
        g_variant_002_spirv, sizeof(g_variant_002_spirv),
#ifdef XANTHUS_D3D12
        g_variant_002_dxil, sizeof(g_variant_002_dxil),
#endif
    },
    {
        0x43B4671FF78C63CFULL,
        g_variant_034_spirv, sizeof(g_variant_034_spirv),
#ifdef XANTHUS_D3D12
        g_variant_034_dxil, sizeof(g_variant_034_dxil),
#endif
    },
    {
        0x46E436034A25AF1BULL,
        g_variant_054_spirv, sizeof(g_variant_054_spirv),
#ifdef XANTHUS_D3D12
        g_variant_054_dxil, sizeof(g_variant_054_dxil),
#endif
    },
    {
        0x4788761627F6C3DAULL,
        g_variant_049_spirv, sizeof(g_variant_049_spirv),
#ifdef XANTHUS_D3D12
        g_variant_049_dxil, sizeof(g_variant_049_dxil),
#endif
    },
    {
        0x4B4A90C302F4CF4EULL,
        g_variant_045_spirv, sizeof(g_variant_045_spirv),
#ifdef XANTHUS_D3D12
        g_variant_045_dxil, sizeof(g_variant_045_dxil),
#endif
    },
    {
        0x4E757F36431FA35BULL,
        g_variant_028_spirv, sizeof(g_variant_028_spirv),
#ifdef XANTHUS_D3D12
        g_variant_028_dxil, sizeof(g_variant_028_dxil),
#endif
    },
    {
        0x5128A604A0D03776ULL,
        g_variant_007_spirv, sizeof(g_variant_007_spirv),
#ifdef XANTHUS_D3D12
        g_variant_007_dxil, sizeof(g_variant_007_dxil),
#endif
    },
    {
        0x5197112CA90FC6F5ULL,
        g_variant_020_spirv, sizeof(g_variant_020_spirv),
#ifdef XANTHUS_D3D12
        g_variant_020_dxil, sizeof(g_variant_020_dxil),
#endif
    },
    {
        0x553281A7BAA05831ULL,
        g_variant_023_spirv, sizeof(g_variant_023_spirv),
#ifdef XANTHUS_D3D12
        g_variant_023_dxil, sizeof(g_variant_023_dxil),
#endif
    },
    {
        0x558FF6B96783FAECULL,
        g_variant_013_spirv, sizeof(g_variant_013_spirv),
#ifdef XANTHUS_D3D12
        g_variant_013_dxil, sizeof(g_variant_013_dxil),
#endif
    },
    {
        0x62A99559A8DFBEC4ULL,
        g_variant_011_spirv, sizeof(g_variant_011_spirv),
#ifdef XANTHUS_D3D12
        g_variant_011_dxil, sizeof(g_variant_011_dxil),
#endif
    },
    {
        0x661B3300ECE5BC0FULL,
        g_variant_039_spirv, sizeof(g_variant_039_spirv),
#ifdef XANTHUS_D3D12
        g_variant_039_dxil, sizeof(g_variant_039_dxil),
#endif
    },
    {
        0x6671991BAFB532F0ULL,
        g_variant_040_spirv, sizeof(g_variant_040_spirv),
#ifdef XANTHUS_D3D12
        g_variant_040_dxil, sizeof(g_variant_040_dxil),
#endif
    },
    {
        0x6C32EF02933C0DA1ULL,
        g_variant_041_spirv, sizeof(g_variant_041_spirv),
#ifdef XANTHUS_D3D12
        g_variant_041_dxil, sizeof(g_variant_041_dxil),
#endif
    },
    {
        0x704A7D532DD9EE6DULL,
        g_variant_012_spirv, sizeof(g_variant_012_spirv),
#ifdef XANTHUS_D3D12
        g_variant_012_dxil, sizeof(g_variant_012_dxil),
#endif
    },
    {
        0x7139DD570931AEBDULL,
        g_variant_050_spirv, sizeof(g_variant_050_spirv),
#ifdef XANTHUS_D3D12
        g_variant_050_dxil, sizeof(g_variant_050_dxil),
#endif
    },
    {
        0x73042DDD2F5E1DBFULL,
        g_variant_006_spirv, sizeof(g_variant_006_spirv),
#ifdef XANTHUS_D3D12
        g_variant_006_dxil, sizeof(g_variant_006_dxil),
#endif
    },
    {
        0x77AF33D724E8EF11ULL,
        g_variant_046_spirv, sizeof(g_variant_046_spirv),
#ifdef XANTHUS_D3D12
        g_variant_046_dxil, sizeof(g_variant_046_dxil),
#endif
    },
    {
        0x7CEF70426DA4D831ULL,
        g_variant_009_spirv, sizeof(g_variant_009_spirv),
#ifdef XANTHUS_D3D12
        g_variant_009_dxil, sizeof(g_variant_009_dxil),
#endif
    },
    {
        0x7E3F904D58611C6EULL,
        g_variant_058_spirv, sizeof(g_variant_058_spirv),
#ifdef XANTHUS_D3D12
        g_variant_058_dxil, sizeof(g_variant_058_dxil),
#endif
    },
    {
        0x85F7D76731CF8DFCULL,
        g_variant_033_spirv, sizeof(g_variant_033_spirv),
#ifdef XANTHUS_D3D12
        g_variant_033_dxil, sizeof(g_variant_033_dxil),
#endif
    },
    {
        0x866BC133295A026AULL,
        g_variant_038_spirv, sizeof(g_variant_038_spirv),
#ifdef XANTHUS_D3D12
        g_variant_038_dxil, sizeof(g_variant_038_dxil),
#endif
    },
    {
        0x91A0032DC74797A9ULL,
        g_variant_018_spirv, sizeof(g_variant_018_spirv),
#ifdef XANTHUS_D3D12
        g_variant_018_dxil, sizeof(g_variant_018_dxil),
#endif
    },
    {
        0x928F593181ADD0DCULL,
        g_variant_048_spirv, sizeof(g_variant_048_spirv),
#ifdef XANTHUS_D3D12
        g_variant_048_dxil, sizeof(g_variant_048_dxil),
#endif
    },
    {
        0x96C40A9A2B60354FULL,
        g_variant_010_spirv, sizeof(g_variant_010_spirv),
#ifdef XANTHUS_D3D12
        g_variant_010_dxil, sizeof(g_variant_010_dxil),
#endif
    },
    {
        0x99A54764634C1849ULL,
        g_variant_019_spirv, sizeof(g_variant_019_spirv),
#ifdef XANTHUS_D3D12
        g_variant_019_dxil, sizeof(g_variant_019_dxil),
#endif
    },
    {
        0x9D9AF9026A8FA48FULL,
        g_variant_035_spirv, sizeof(g_variant_035_spirv),
#ifdef XANTHUS_D3D12
        g_variant_035_dxil, sizeof(g_variant_035_dxil),
#endif
    },
    {
        0xA3A2DA7E9C2A8956ULL,
        g_variant_037_spirv, sizeof(g_variant_037_spirv),
#ifdef XANTHUS_D3D12
        g_variant_037_dxil, sizeof(g_variant_037_dxil),
#endif
    },
    {
        0xA4567362963F6A8AULL,
        g_variant_021_spirv, sizeof(g_variant_021_spirv),
#ifdef XANTHUS_D3D12
        g_variant_021_dxil, sizeof(g_variant_021_dxil),
#endif
    },
    {
        0xAB9D92183F87F53AULL,
        g_variant_042_spirv, sizeof(g_variant_042_spirv),
#ifdef XANTHUS_D3D12
        g_variant_042_dxil, sizeof(g_variant_042_dxil),
#endif
    },
    {
        0xAE823BBA197A8946ULL,
        g_variant_014_spirv, sizeof(g_variant_014_spirv),
#ifdef XANTHUS_D3D12
        g_variant_014_dxil, sizeof(g_variant_014_dxil),
#endif
    },
    {
        0xB18EF1DA3D3788CEULL,
        g_variant_032_spirv, sizeof(g_variant_032_spirv),
#ifdef XANTHUS_D3D12
        g_variant_032_dxil, sizeof(g_variant_032_dxil),
#endif
    },
    {
        0xB337E6B2A6A8395EULL,
        g_variant_000_spirv, sizeof(g_variant_000_spirv),
#ifdef XANTHUS_D3D12
        g_variant_000_dxil, sizeof(g_variant_000_dxil),
#endif
    },
    {
        0xB54391F70A8436F0ULL,
        g_variant_052_spirv, sizeof(g_variant_052_spirv),
#ifdef XANTHUS_D3D12
        g_variant_052_dxil, sizeof(g_variant_052_dxil),
#endif
    },
    {
        0xB7628A764E758AB9ULL,
        g_variant_017_spirv, sizeof(g_variant_017_spirv),
#ifdef XANTHUS_D3D12
        g_variant_017_dxil, sizeof(g_variant_017_dxil),
#endif
    },
    {
        0xC36648B5B5B14CB6ULL,
        g_variant_051_spirv, sizeof(g_variant_051_spirv),
#ifdef XANTHUS_D3D12
        g_variant_051_dxil, sizeof(g_variant_051_dxil),
#endif
    },
    {
        0xC546B2F40EEE522EULL,
        g_variant_001_spirv, sizeof(g_variant_001_spirv),
#ifdef XANTHUS_D3D12
        g_variant_001_dxil, sizeof(g_variant_001_dxil),
#endif
    },
    {
        0xCD715566004B10E4ULL,
        g_variant_015_spirv, sizeof(g_variant_015_spirv),
#ifdef XANTHUS_D3D12
        g_variant_015_dxil, sizeof(g_variant_015_dxil),
#endif
    },
    {
        0xD3E8EE793F261CEBULL,
        g_variant_025_spirv, sizeof(g_variant_025_spirv),
#ifdef XANTHUS_D3D12
        g_variant_025_dxil, sizeof(g_variant_025_dxil),
#endif
    },
    {
        0xD84DADB96A241F20ULL,
        g_variant_057_spirv, sizeof(g_variant_057_spirv),
#ifdef XANTHUS_D3D12
        g_variant_057_dxil, sizeof(g_variant_057_dxil),
#endif
    },
    {
        0xD85AF30FA765DE07ULL,
        g_variant_030_spirv, sizeof(g_variant_030_spirv),
#ifdef XANTHUS_D3D12
        g_variant_030_dxil, sizeof(g_variant_030_dxil),
#endif
    },
    {
        0xD91364B74868C0D9ULL,
        g_variant_031_spirv, sizeof(g_variant_031_spirv),
#ifdef XANTHUS_D3D12
        g_variant_031_dxil, sizeof(g_variant_031_dxil),
#endif
    },
    {
        0xDBED1F91889652CAULL,
        g_variant_053_spirv, sizeof(g_variant_053_spirv),
#ifdef XANTHUS_D3D12
        g_variant_053_dxil, sizeof(g_variant_053_dxil),
#endif
    },
    {
        0xDD39F8BCAC1B8559ULL,
        g_variant_047_spirv, sizeof(g_variant_047_spirv),
#ifdef XANTHUS_D3D12
        g_variant_047_dxil, sizeof(g_variant_047_dxil),
#endif
    },
    {
        0xE6E0FCC9288A6A56ULL,
        g_variant_043_spirv, sizeof(g_variant_043_spirv),
#ifdef XANTHUS_D3D12
        g_variant_043_dxil, sizeof(g_variant_043_dxil),
#endif
    },
    {
        0xFB20DBF244E81690ULL,
        g_variant_029_spirv, sizeof(g_variant_029_spirv),
#ifdef XANTHUS_D3D12
        g_variant_029_dxil, sizeof(g_variant_029_dxil),
#endif
    },
    {
        0xFD8A18E713203C4DULL,
        g_variant_055_spirv, sizeof(g_variant_055_spirv),
#ifdef XANTHUS_D3D12
        g_variant_055_dxil, sizeof(g_variant_055_dxil),
#endif
    },
    {
        0xFDA9D2E7AC60BC33ULL,
        g_variant_005_spirv, sizeof(g_variant_005_spirv),
#ifdef XANTHUS_D3D12
        g_variant_005_dxil, sizeof(g_variant_005_dxil),
#endif
    },
    {
        0xFEB49CDDAD792BF7ULL,
        g_variant_026_spirv, sizeof(g_variant_026_spirv),
#ifdef XANTHUS_D3D12
        g_variant_026_dxil, sizeof(g_variant_026_dxil),
#endif
    },
};

struct State
{
    uint64_t hash;
    FixedFunctionVertexShaderControlState state;
    uint64_t programKey;
};

static const State States[] =
{
    { 0x0A3F8AA0F529FA43ULL, {1,1,1,0,0,0,0,0,0,1,1,2,{3,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7552,0,0,0}, 0xE6E0FCC9288A6A56ULL },
    { 0x0D94F43A1A4A7219ULL, {1,1,1,0,0,1,0,0,0,0,1,2,{3,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{0,0,0,0},8064,0,0,0}, 0xDBED1F91889652CAULL },
    { 0x0DA6FA3DD1CC21E0ULL, {1,1,1,0,0,0,0,0,0,0,1,2,{3,0,0,0,0,0,0,0},{1,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7552,0,0,0}, 0x866BC133295A026AULL },
    { 0x0FC5136B86BD0771ULL, {1,1,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0,0,0},{0,1,0,0},{0,0,2,3},{0,1,0,0},{2,0,0,0},6544,0,0,0}, 0x5128A604A0D03776ULL },
    { 0x13601255EBEA3AACULL, {1,1,1,0,0,0,0,0,0,0,1,2,{3,0,0,0,0,0,0,0},{0,1,0,0},{0,0,2,3},{0,1,0,0},{2,0,0,0},6528,0,0,0}, 0xA3A2DA7E9C2A8956ULL },
    { 0x172DB611CACB68F0ULL, {1,1,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0,0,0},{1,0,0,0},{0,1,2,3},{1,0,0,0},{0,0,0,0},7576,0,0,0}, 0x558FF6B96783FAECULL },
    { 0x1B8823BFCD7A79C2ULL, {1,1,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{0,0,0,0},8088,0,0,0}, 0xC546B2F40EEE522EULL },
    { 0x1DE33CF88B6B7C06ULL, {1,1,1,0,0,1,0,0,0,1,0,0,{3,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7552,0,0,0}, 0x09860D0D70439218ULL },
    { 0x22C3B5B68DA705EDULL, {1,1,0,0,0,1,0,0,0,0,1,2,{0,0,0,0,0,0,0,0},{1,0,0,0},{0,1,2,3},{1,0,0,0},{0,0,0,0},7576,0,0,0}, 0xD85AF30FA765DE07ULL },
    { 0x247378AF0777F36CULL, {1,1,1,0,0,0,0,1,0,1,1,2,{3,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7552,0,0,0}, 0x4788761627F6C3DAULL },
    { 0x287B882619DEE06DULL, {1,1,1,0,0,1,0,0,0,1,1,2,{3,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7552,0,0,0}, 0xD84DADB96A241F20ULL },
    { 0x448CAF865077554AULL, {1,1,1,0,0,0,0,1,0,1,0,0,{3,0,0,0,0,0,0,0},{1,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7552,0,0,0}, 0x928F593181ADD0DCULL },
    { 0x4A18D1CD5D835959ULL, {1,1,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7568,0,0,0}, 0x3EFE089C9AA220BCULL },
    { 0x50949A80C7C4EAEEULL, {1,1,0,0,0,1,0,0,0,0,1,2,{0,0,0,0,0,0,0,0},{0,1,0,0},{0,0,2,3},{0,1,0,0},{2,0,0,0},6544,0,0,0}, 0xFB20DBF244E81690ULL },
    { 0x537CC2D1F7B14A9FULL, {1,1,0,0,0,0,0,0,0,0,1,2,{0,0,0,0,0,0,0,0},{0,1,0,0},{0,0,2,3},{0,1,0,0},{2,0,0,0},6544,0,0,0}, 0xA4567362963F6A8AULL },
    { 0x5A18656C7FD26398ULL, {1,1,1,0,0,1,0,0,0,0,0,0,{3,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{0,0,0,0},8064,0,0,0}, 0x7139DD570931AEBDULL },
    { 0x5F961BA711F4BBC1ULL, {1,1,1,0,0,0,0,0,0,0,0,0,{3,0,0,0,0,0,0,0},{1,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7552,0,0,0}, 0x43B4671FF78C63CFULL },
    { 0x6115F1AEF3F44B53ULL, {1,1,1,0,0,0,0,0,0,0,0,0,{3,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{0,0,0,0},8064,0,0,0}, 0xD91364B74868C0D9ULL },
    { 0x6204BF03FD8564A2ULL, {1,1,1,0,0,1,0,0,0,1,1,2,{3,0,0,0,0,0,0,0},{0,1,0,0},{0,0,2,3},{0,1,0,0},{2,0,0,0},6528,0,0,0}, 0x7E3F904D58611C6EULL },
    { 0x6768B9476DA45472ULL, {1,1,1,0,0,0,0,0,0,1,0,0,{3,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,2,0,0},6528,0,0,0}, 0x6671991BAFB532F0ULL },
    { 0x69A4E0D2589325FBULL, {1,1,1,0,0,0,0,0,0,1,0,0,{3,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7552,0,0,0}, 0x661B3300ECE5BC0FULL },
    { 0x6D8132D9A763435FULL, {1,1,1,0,0,0,0,0,0,1,1,2,{3,0,0,0,0,0,0,0},{1,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7552,0,0,0}, 0x77AF33D724E8EF11ULL },
    { 0x6E38AEF24C96E114ULL, {1,1,1,0,0,0,0,0,0,1,1,2,{3,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,2,0,0},6528,0,0,0}, 0x33AF6A2016B00BFCULL },
    { 0x7AAA9ABA8430543DULL, {1,1,0,0,0,0,0,0,0,0,1,2,{0,0,0,0,0,0,0,0},{1,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7568,0,0,0}, 0x553281A7BAA05831ULL },
    { 0x7BB953653D046567ULL, {1,1,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,3,3,0},4496,0,0,0}, 0x73042DDD2F5E1DBFULL },
    { 0x7CFFB6CB4BFBCC42ULL, {1,1,1,0,0,1,0,0,0,0,0,0,{3,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7552,0,0,0}, 0xC36648B5B5B14CB6ULL },
    { 0x83BF54C59CA093CAULL, {1,1,0,0,0,0,0,0,0,0,1,2,{0,0,0,0,0,0,0,0},{1,0,0,0},{0,1,2,3},{1,0,0,0},{0,0,0,0},7568,0,0,0}, 0xD3E8EE793F261CEBULL },
    { 0x88B78D83D5C8E6E3ULL, {1,1,1,0,0,0,0,0,0,1,0,0,{3,0,0,0,0,0,0,0},{1,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7552,0,0,0}, 0xAB9D92183F87F53AULL },
    { 0x895E796FFF543C5BULL, {1,1,0,0,0,0,0,0,0,0,1,2,{0,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{0,0,0,0},8088,0,0,0}, 0x1FAB707A1EA2BCD9ULL },
    { 0x8D7C080381C938C3ULL, {1,1,0,0,0,0,0,0,0,0,1,2,{0,0,0,0,0,0,0,0},{1,0,0,0},{0,1,2,3},{0,0,0,0},{2,2,0,0},6544,0,0,0}, 0x36DD7448F8D8B481ULL },
    { 0x8F23749DB2439D8BULL, {1,1,1,0,0,0,0,0,0,0,0,0,{3,0,0,0,0,0,0,0},{0,1,0,0},{0,0,2,3},{0,1,0,0},{2,0,0,0},6528,0,0,0}, 0x85F7D76731CF8DFCULL },
    { 0x901C2CE192B91D46ULL, {1,1,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0,0,0},{0,0,0,0},{1,1,2,3},{0,0,0,0},{0,2,0,0},6544,0,0,0}, 0x0AB6EE9AE538EC75ULL },
    { 0x95F5C35544DD6C0EULL, {1,1,0,0,0,0,0,0,0,0,1,2,{0,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,2,0,0},6544,0,0,0}, 0x91A0032DC74797A9ULL },
    { 0x97EA732654874EB3ULL, {1,1,1,0,0,0,0,0,0,0,1,2,{3,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{0,0,0,0},8064,0,0,0}, 0x9D9AF9026A8FA48FULL },
    { 0x997987D9B136B3C3ULL, {1,1,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{0,0,0,0},8080,0,0,0}, 0xB337E6B2A6A8395EULL },
    { 0x9C41F8429DA6E42EULL, {1,1,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0,0,0},{1,0,0,0},{0,1,2,3},{0,0,0,0},{2,2,0,0},6544,0,0,0}, 0x96C40A9A2B60354FULL },
    { 0x9D6F0432DA0704B1ULL, {1,1,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0,0,0},{1,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7568,0,0,0}, 0x7CEF70426DA4D831ULL },
    { 0x9DF1BC7826C4EDD1ULL, {1,1,0,0,0,0,0,0,0,0,1,2,{0,0,0,0,0,0,0,0},{0,0,1,0},{0,1,0,3},{0,0,1,0},{2,2,0,0},4496,0,0,0}, 0x99A54764634C1849ULL },
    { 0xA162165C08FBBEEAULL, {1,1,0,0,0,1,0,0,0,0,0,0,{0,0,0,0,0,0,0,0},{1,0,0,0},{0,1,2,3},{1,0,0,0},{0,0,0,0},7576,0,0,0}, 0x4E757F36431FA35BULL },
    { 0xADF6A7F13F117247ULL, {1,1,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,2,0,0},6544,0,0,0}, 0xFDA9D2E7AC60BC33ULL },
    { 0xAF4DFEAE05F60836ULL, {1,1,1,0,0,0,0,1,0,1,0,0,{3,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7552,0,0,0}, 0xDD39F8BCAC1B8559ULL },
    { 0xAF89786B3F4AF4DEULL, {1,1,0,0,0,0,0,0,0,0,1,2,{0,0,0,0,0,0,0,0},{0,0,0,0},{1,1,2,3},{0,0,0,0},{0,2,0,0},6544,0,0,0}, 0x1F49D54881EEE59DULL },
    { 0xB09F07B7E3AEFB52ULL, {1,1,0,0,0,0,0,0,0,0,1,2,{0,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{0,0,0,0},8080,0,0,0}, 0xCD715566004B10E4ULL },
    { 0xB0ECCD2CAF06C29EULL, {1,1,1,0,0,1,0,0,0,0,1,2,{3,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7552,0,0,0}, 0x46E436034A25AF1BULL },
    { 0xB99FC730746BC05FULL, {1,1,0,0,0,0,0,0,0,0,1,2,{0,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,3,3,0},4496,0,0,0}, 0x5197112CA90FC6F5ULL },
    { 0xBBA958FB88F96F5EULL, {1,1,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0,0,0},{1,0,0,0},{1,1,2,3},{0,0,0,0},{0,2,0,0},6544,0,0,0}, 0xAE823BBA197A8946ULL },
    { 0xBDEAEB7C0DD8B248ULL, {1,1,1,0,0,1,0,0,0,0,0,0,{3,0,0,0,0,0,0,0},{0,1,0,0},{0,0,2,3},{0,1,0,0},{2,0,0,0},6528,0,0,0}, 0xB54391F70A8436F0ULL },
    { 0xBF5F479B665FF431ULL, {1,1,1,0,0,0,0,0,0,1,1,2,{3,0,0,0,0,0,0,0},{0,1,0,0},{0,0,2,3},{0,1,0,0},{2,0,0,0},6528,0,0,0}, 0x4B4A90C302F4CF4EULL },
    { 0xC122AD2B00AF3B41ULL, {1,1,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0,0,0},{1,1,0,0},{0,1,2,3},{0,0,0,0},{2,2,0,0},6544,0,0,0}, 0x62A99559A8DFBEC4ULL },
    { 0xC57B5B7316AE2A27ULL, {1,1,1,0,0,1,0,0,0,0,1,2,{3,0,0,0,0,0,0,0},{0,1,0,0},{0,0,2,3},{0,1,0,0},{2,0,0,0},6528,0,0,0}, 0xFD8A18E713203C4DULL },
    { 0xCA74F5525E9861E1ULL, {1,1,0,0,0,0,0,0,0,0,1,2,{0,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7568,0,0,0}, 0xB7628A764E758AB9ULL },
    { 0xD0C82D457E5C795CULL, {1,1,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0,0,0},{0,0,0,0},{0,0,2,3},{0,0,0,0},{2,0,0,0},6544,0,0,0}, 0x4281AB6AE25E866BULL },
    { 0xD3A748423F021213ULL, {1,1,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7576,0,0,0}, 0x331CE56F16602D01ULL },
    { 0xD86799438BEA439EULL, {1,1,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0,0,0},{1,0,0,0},{0,1,2,3},{1,0,0,0},{0,0,0,0},7568,0,0,0}, 0x704A7D532DD9EE6DULL },
    { 0xDAAD3E2D60489307ULL, {1,1,0,0,0,0,0,0,0,0,1,2,{0,0,0,0,0,0,0,0},{1,0,0,0},{1,1,2,3},{0,0,0,0},{0,2,0,0},6544,0,0,0}, 0x06D4C3C4C383B150ULL },
    { 0xE99A22A305107E75ULL, {1,1,0,0,0,0,0,0,0,0,1,2,{0,0,0,0,0,0,0,0},{1,0,0,0},{0,1,2,3},{1,0,0,0},{0,0,0,0},7576,0,0,0}, 0xFEB49CDDAD792BF7ULL },
    { 0xF06058A6A3B6012DULL, {1,1,1,0,0,0,0,0,0,1,0,0,{3,0,0,0,0,0,0,0},{0,1,0,0},{0,0,2,3},{0,1,0,0},{2,0,0,0},6528,0,0,0}, 0x6C32EF02933C0DA1ULL },
    { 0xF210D0BB1004C396ULL, {1,1,1,0,0,0,0,0,0,0,0,0,{3,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7552,0,0,0}, 0xB18EF1DA3D3788CEULL },
    { 0xFF75AD857E8F0652ULL, {1,1,1,0,0,0,0,0,0,0,1,2,{3,0,0,0,0,0,0,0},{0,0,0,0},{0,1,2,3},{0,0,0,0},{2,0,0,0},7552,0,0,0}, 0x3C62211EE30A3A3EULL },
};
static_assert(std::size(Programs) == 59);
static_assert(std::size(States) == 59);
}
