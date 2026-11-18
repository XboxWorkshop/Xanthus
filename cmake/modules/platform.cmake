set(XANTHUS_PLAT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/platform)

if(WIN32)
    set(XANTHUS_PLAT "${XANTHUS_PLAT_SRC}/win32")
elseif(LINUX)
    set(XANTHUS_PLAT "${XANTHUS_PLAT_SRC}/linux")
else()
    message(FATAL_ERROR "Unknown target platform; add a new folder in src/platform/ and implement according OS functions.")
endif()

list(APPEND XANTHUS_SOURCES
    "${XANTHUS_PLAT}/process.cpp"
)
