set(XANTHUS_KRNL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/kernel)

list(APPEND XANTHUS_SOURCES
    "${XANTHUS_KRNL_SRC}/heap.cpp"
    "${XANTHUS_KRNL_SRC}/imports.cpp"
    "${XANTHUS_KRNL_SRC}/kernel.cpp"
    "${XANTHUS_KRNL_SRC}/memory.cpp"
    "${XANTHUS_KRNL_SRC}/io/file_system.cpp"
)
