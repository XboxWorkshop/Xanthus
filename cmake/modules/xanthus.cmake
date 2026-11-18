set(XANTHUS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/xanthus)

list(APPEND XANTHUS_SOURCES
    "${XANTHUS_SRC}/xanthus.cpp"
)

if(XANTHUS_LOG)
    list(APPEND XANTHUS_SOURCES
        "${XANTHUS_SRC}/log.cpp"
    )
endif()
