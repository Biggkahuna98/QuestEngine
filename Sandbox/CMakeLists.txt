set (CMAKE_CXX_STANDARD 26)
set (MODULE_NAME Sandbox)

file (GLOB_RECURSE SOURCES
        Private/*.h
        Private/*.hpp
        Private/*.inl
        Private/*.cpp
)

add_library(${MODULE_NAME} SHARED ${SOURCES})
# Runtime provides the Application contract + Engine; it transitively brings Core + Render.
target_link_libraries(${MODULE_NAME} PUBLIC Runtime)
target_include_directories(${MODULE_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${QUEST_MODULES}/Core/Public
        ${QUEST_MODULES}/Runtime/Public
        ${QUEST_MODULES}/RHI/Public
)
target_compile_definitions(${MODULE_NAME} INTERFACE SANDBOX_BUILD_SHARED)
target_compile_definitions(${MODULE_NAME} PRIVATE SANDBOX_INTERNAL_USE_ONLY)