set(XANTHUS_GPU_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/gpu)
list(APPEND XANTHUS_SOURCES
    # Top-level
    "${XANTHUS_GPU_SRC}/FixedFunction.cpp"
    "${XANTHUS_GPU_SRC}/guest_convert.cpp"
    "${XANTHUS_GPU_SRC}/pipeline.cpp"
    "${XANTHUS_GPU_SRC}/pixelshader.cpp"
    "${XANTHUS_GPU_SRC}/pushbuffer.cpp"
    "${XANTHUS_GPU_SRC}/renderstate.cpp"
    "${XANTHUS_GPU_SRC}/resources.cpp"
    "${XANTHUS_GPU_SRC}/swizzle.cpp"
    "${XANTHUS_GPU_SRC}/textures.cpp"
    "${XANTHUS_GPU_SRC}/vertexbuffer.cpp"
    "${XANTHUS_GPU_SRC}/vertexdeclaration.cpp"
    "${XANTHUS_GPU_SRC}/vertexshader.cpp"
    "${XANTHUS_GPU_SRC}/video.cpp"
    # Frame pacing
    "${XANTHUS_GPU_SRC}/frame_pacing/frame_pacer.cpp"
    "${XANTHUS_GPU_SRC}/frame_pacing/timer_resolution.cpp"
    # Ubershaders
    "${XANTHUS_GPU_SRC}/ubershaders/fixed_function_pixel_shader_variants.cpp"
    "${XANTHUS_GPU_SRC}/ubershaders/fixed_function_vertex_shader_variants.cpp"
    # Shadercache
    "${CMAKE_CURRENT_SOURCE_DIR}/src/shader/shader_cache.cpp"
)

if(XANTHUS_IMGUI)
    list(APPEND XANTHUS_SOURCES
        "${XANTHUS_GPU_SRC}/imgui/imgui_common.cpp"
        "${XANTHUS_GPU_SRC}/imgui/imgui_font_builder.cpp"
        "${XANTHUS_GPU_SRC}/imgui/imgui_snapshot.cpp"
    )
endif()
