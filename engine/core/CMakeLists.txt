set(FB_CORE_NAME "core")

file(GLOB_RECURSE "${FB_CORE_NAME}_SOURCES"
"${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/source/*.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

add_library(${FB_CORE_NAME} ${${FB_CORE_NAME}_SOURCES})

GROUP_FILE_TREE_FILTER_TARGET_NAME("${${FB_CORE_NAME}_SOURCES}" "engine" ${FB_CORE_NAME})

target_include_directories(${FB_CORE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source)
target_include_directories(${FB_CORE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(${FB_CORE_NAME} PROPERTIES CXX_STANDARD 20)

if(MSVC)
target_compile_options(${FB_CORE_NAME} PRIVATE
    "/wd26812"  # The enum type 'type-name' is unscoped. Prefer 'enum class' over 'enum'
    "/WX"       # Treat linker warnings as errors
    "/W4"       # Warning level 4
    "/MP"       # Build with multiple processes
    )
endif()