set (CMAKE_CXX_STANDARD 26)
set (MODULE_NAME Runtime)

file (GLOB_RECURSE SOURCES
        Private/*.h
        Private/*.hpp
        Private/*.inl
        Private/*.cpp
)

# Runtime is the top-level orchestration layer (Engine + Application contract). It sits above
# Render so the Engine can own and drive a concrete Render::Renderer directly.
add_library(${MODULE_NAME} ${QUEST_BUILD_LIB_TYPE} ${SOURCES})
target_compile_definitions(${MODULE_NAME} PRIVATE RUNTIME_INTERNAL_USE_ONLY)
target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Public)

target_link_libraries(${MODULE_NAME} PUBLIC Core Render)

# Propagate QE_BUILD_SHARED to consumers so RUNTIME_API expands to dllimport.
if (QUEST_BUILD_LIB_TYPE STREQUAL "SHARED")
    target_compile_definitions(${MODULE_NAME} INTERFACE QE_BUILD_SHARED)
endif()
