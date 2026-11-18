cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

project(Xanthus VERSION 0.1.0 LANGUAGES C CXX)

option(XANTHUS_RECOMP "Enable Xanthus recompilation tools (make your XboxRecomp2 output an include path)" OFF)
option(XANTHUS_IMGUI  "Enable Xanthus vendored ImGUI debug features"    OFF)
option(XANTHUS_LOG    "Enable Xanthus logging functionality (portable)" OFF)
option(XANTHUS_CPM    "Enable Xanthus CPM"                              OFF)
if (WIN32)
    option(XANTHUS_D3D12 "Add D3D12 support for rendering" ON)
endif()

# =============================================================================
# Vendored
# =============================================================================

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/plume ${CMAKE_BINARY_DIR}/plume)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/DirectX-Headers ${CMAKE_BINARY_DIR}/dx_headers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/unordered_dense ${CMAKE_BINARY_DIR}/unordered_dense)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/fmt ${CMAKE_BINARY_DIR}/fmt)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/XShaderRecomp ${CMAKE_BINARY_DIR}/shader_recomp)

target_include_directories(plume PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/DirectX-Headers/include/dxguids
)

# =============================================================================
# Modules
# =============================================================================

set(XANTHUS_SOURCES "")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/cpu.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/gpu.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/kernel.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/input.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/platform.cmake) # Platform-specific implementations

# =============================================================================
# Library
# =============================================================================

add_library(xanthus STATIC ${XANTHUS_SOURCES})

# Shaders must be generated after defining the library target
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/shaders.cmake)

target_include_directories(xanthus PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/plume
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/unordered_dense/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/fmt/include
    ${CMAKE_CURRENT_SOURCE_DIR}/recomp/XShaderRecomp/XShaderRecomp/

)

target_include_directories(xanthus PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(XANTHUS_RECOMP)
    target_compile_definitions(xanthus PRIVATE XANTHUS_RECOMP=1)
endif()

if(XANTHUS_LOG)
    target_compile_definitions(xanthus PRIVATE XANTHUS_LOG=1)
endif()

if(XANTHUS_D3D12)
    target_compile_definitions(xanthus PRIVATE XANTHUS_D3D12=1)
endif()

# =============================================================================
# Helper Libraries & Linking
# =============================================================================

if(NOT CMAKE_SYSTEM_NAME MATCHES "^(iOS|Android)$")
    find_package(SDL3 CONFIG QUIET)
endif()

if(APPLE)
    if(EXISTS "/opt/homebrew/lib/cmake/SDL3/SDL3Config.cmake")
        set(SDL3_DIR "/opt/homebrew/lib/cmake/SDL3" CACHE PATH "Homebrew SDL3" FORCE)
        find_package(SDL3 CONFIG QUIET)
    endif()
endif()

if(NOT SDL3_FOUND)
    if(NOT XANTHUS_CPM)
        message(FATAL_ERROR "SDL3 was not found via find_package. Xanthus can fetch it automatically: -DXANTHUS_CPM=ON")
    endif()
    message(STATUS "SDL3 not found via find_package; fetching it locally (via CPM).")
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake)
    if(CMAKE_SYSTEM_NAME STREQUAL "Android")
        CPMAddPackage(
            NAME SDL3
            GITHUB_REPOSITORY libsdl-org/SDL
            GIT_TAG release-3.4.16
            PATCH_COMMAND ${CMAKE_COMMAND} -DSDL3_SOURCE_DIR=<SOURCE_DIR> -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/deps/patches/sdl3_vulkan_maintenance1.cmake"
        )
    else()
        CPMAddPackage(
            NAME SDL3
            GITHUB_REPOSITORY libsdl-org/SDL
            GIT_TAG release-3.4.16
        )
    endif()
endif()

target_link_libraries(xanthus PRIVATE
    SDL3::SDL3
    fmt::fmt
    unordered_dense
    plume
)
