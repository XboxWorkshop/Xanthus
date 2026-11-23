function(compile_shader FILE_PATH TARGET_NAME)
    set(FILE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/gpu/shader/${FILE_PATH}.hlsl)
    cmake_path(GET FILE_PATH STEM VARIABLE_NAME)
    if (XANTHUS_D3D12)
        add_custom_command(
            OUTPUT ${FILE_PATH}.dxil.h
            COMMAND ${DIRECTX_DXC_TOOL} -T ${TARGET_NAME} -HV 2021 -all-resources-bound -Wno-ignored-attributes -Fh ${FILE_PATH}.dxil.h ${FILE_PATH} -Vn g_${VARIABLE_NAME}_dxil
            DEPENDS ${FILE_PATH}
        )
        target_sources(xanthus PRIVATE ${FILE_PATH}.dxil.h)
    endif()
    add_custom_command(
        OUTPUT ${FILE_PATH}.spirv.h
        COMMAND ${DIRECTX_DXC_TOOL} -T ${TARGET_NAME} -HV 2021 -all-resources-bound -spirv -fvk-use-dx-layout ${ARGN} -Fh ${FILE_PATH}.spirv.h ${FILE_PATH} -Vn g_${VARIABLE_NAME}_spirv
        DEPENDS ${FILE_PATH}
    )
    target_sources(xanthus PRIVATE ${FILE_PATH}.spirv.h)
endfunction()

################################################################################
# Ubershaders: Composed of:
# - Generic fixed-function shaders (fallback)
# - Derived optimized fixed function shaders (variants - derived from generic)

include("${CMAKE_CURRENT_SOURCE_DIR}/vendor/XShaderRecomp/XShaderRecomp/fixed_function/XShaderRecompFixedFunction.cmake")

if(CMAKE_HOST_WIN32)
    set(DIRECTX_DXC_TOOL "${CMAKE_CURRENT_SOURCE_DIR}/vendor/XShaderRecomp/thirdparty/dxc-bin/bin/x64/dxc.exe")
elseif(CMAKE_HOST_LINUX)
    set(DXC_REAL    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/XShaderRecomp/thirdparty/dxc-bin/bin/x64/dxc-linux")
    set(DXC_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/XShaderRecomp/thirdparty/dxc-bin/lib/x64")
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/dxc-linux.sh.in"
        "${CMAKE_BINARY_DIR}/dxc-linux.sh"
        @ONLY
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                         GROUP_READ GROUP_EXECUTE
                         WORLD_READ WORLD_EXECUTE)
    set(DIRECTX_DXC_TOOL "${CMAKE_BINARY_DIR}/dxc-linux.sh")
elseif(CMAKE_HOST_APPLE)
    set(DIRECTX_DXC_TOOL "${CMAKE_CURRENT_SOURCE_DIR}/vendor/XShaderRecomp/thirdparty/dxc-bin/bin/x64/dxc-macos")
else()
    message(FATAL_ERROR "Unknown host; you must be on Windows, Linux, or MacOS to compile ubershaders.")
endif()

xsr_add_fixed_function_shaders(
    TARGET xanthus
    DXC ${DIRECTX_DXC_TOOL}
    DXIL ${XANTHUS_D3D12}
    DXIL_MACRO XANTHUS_D3D12
    OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/gpu/ubershaders/generated"
    PIXEL_GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/gpu/ubershaders/generated/fixed_function_pixel_shader"
    VERTEX_GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/gpu/ubershaders/generated/fixed_function_vertex_shader")

# END OF UBERSHADERS
################################################################################

function(compile_vertex_shader FILE_PATH)
    compile_shader(${FILE_PATH} vs_6_0 -fvk-invert-y)
endfunction()

function(compile_pixel_shader FILE_PATH)
    compile_shader(${FILE_PATH} ps_6_0)
endfunction()

compile_vertex_shader(PassthroughVertexShader)

compile_vertex_shader(copy_vs)
compile_pixel_shader(copy_color_ps)
compile_pixel_shader(copy_depth_ps)

compile_pixel_shader(gamma_correction_ps)
compile_pixel_shader(imgui_ps)
compile_vertex_shader(imgui_vs)


compile_vertex_shader(R2D_vs0)
compile_vertex_shader(R2D_vs1)
