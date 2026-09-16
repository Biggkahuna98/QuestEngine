set (CMAKE_CXX_STANDARD 26)
set (MODULE_NAME Core)

file (GLOB_RECURSE SOURCES
        Private/*.h
        Private/*.hpp
        Private/*.inl
        Private/*.cpp
)

add_library(${MODULE_NAME} ${QUEST_BUILD_LIB_TYPE} ${SOURCES})
target_compile_definitions(${MODULE_NAME} PRIVATE CORE_INTERNAL_USE_ONLY)
target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Public)

target_include_directories(${MODULE_NAME} PRIVATE
    ${QUEST_THIRD_PARTY_LIBS}/Engine/SDL3-3.4.8/include
)

# Add private linkages here
set (PRIVATE_LINKS
    ${QUEST_THIRD_PARTY_LIBS}/Engine/SDL3-3.4.8/lib/x64/SDL3.lib
)

# Link the private libs
target_link_libraries(${MODULE_NAME} PRIVATE ${PRIVATE_LINKS})

# Propagate QE_BUILD_SHARED to consumers so QUEST_API expands to dllimport
if (QUEST_BUILD_LIB_TYPE STREQUAL "SHARED")
    target_compile_definitions(${MODULE_NAME} INTERFACE QE_BUILD_SHARED)
endif()