macro(GROUP_FILE_TREE_FILTER_TARGET_NAME FILES FOLDER TARGET_NAME)
    foreach(ITEM IN ITEMS ${FILES})
        get_filename_component(SRC_PATH "${ITEM}" PATH)

        string(REPLACE "${CMAKE_SOURCE_DIR}/${FOLDER}/${TARGET_NAME}" "" GROUP_PATH "${SRC_PATH}")
        string(REPLACE "/" "\\" GROUP_PATH "${GROUP_PATH}")
        
        source_group("${GROUP_PATH}" FILES ${ITEM})
    endforeach()
endmacro()