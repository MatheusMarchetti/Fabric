set(FB_TESTS_NAME "tests")

file(GLOB_RECURSE "${FB_TESTS_NAME}_SOURCES"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

    add_executable(${FB_TESTS_NAME} ${${FB_TESTS_NAME}_SOURCES})

    GROUP_FILE_TREE_FILTER_TARGET_NAME("${${FB_TESTS_NAME}_SOURCES}" "tools" ${FB_TESTS_NAME})
    
    target_include_directories(${FB_TESTS_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source)
    target_include_directories(${FB_TESTS_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_include_directories(${FB_TESTS_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/engine/core/include")
    
    target_link_libraries(${FB_TESTS_NAME} core)

    set_target_properties(${FB_TESTS_NAME} PROPERTIES CXX_STANDARD 20)

    if(MSVC)
        target_compile_options(${FB_TESTS_NAME} PRIVATE
            "/wd26812"  # The enum type 'type-name' is unscoped. Prefer 'enum class' over 'enum'
            "/WX"       # Treat linker warnings as errors
            "/W4"       # Warning level 4
            "/MP"       # Build with multiple processes
            )
    endif()
