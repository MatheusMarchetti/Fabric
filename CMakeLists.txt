cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

include("scripts/setup.cmake")

project(Fabric
    VERSION 0.1.0 # https://semver.org/spec/v0.1.0.html
    DESCRIPTION "A self-made game engine designed for Windows PCs and Xbox consoles"
    HOMEPAGE_URL "https://github.com/MatheusMarchetti/Fabric"
    LANGUAGES CXX CSharp
)

add_subdirectory("engine")
add_subdirectory("tools")

if(WIN32)
    message(STATUS "Building Fabric for Windows.")
else()
    message(FATAL_ERROR "Unsupported target platform.")
endif()

set(CMAKE_SUPPRESS_REGENERATION ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Profiling;Release" CACHE STRING "" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "Build")
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${FB_TESTS_NAME})

    set_target_properties(${FB_TESTS_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()

