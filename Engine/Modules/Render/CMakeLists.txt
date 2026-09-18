set (CMAKE_CXX_STANDARD 26)
set (MODULE_NAME Render)

file (GLOB_RECURSE SOURCES
        Private/*.h
        Private/*.hpp
        Private/*.inl
        Private/*.cpp
)

add_library(${MODULE_NAME} ${QUEST_BUILD_LIB_TYPE} ${SOURCES})
target_compile_definitions(${MODULE_NAME} PRIVATE RENDER_INTERNAL_USE_ONLY)

target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Public)

target_link_libraries(${MODULE_NAME} PUBLIC Core VulkanRHI)

# Propagate QE_BUILD_SHARED to consumers so RENDER_API expands to dllimport.
if (QUEST_BUILD_LIB_TYPE STREQUAL "SHARED")
    target_compile_definitions(${MODULE_NAME} INTERFACE QE_BUILD_SHARED)
endif()
