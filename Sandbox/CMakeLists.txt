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

# Sandbox is the consuming code, so it owns deploying itself next to the
# Launch executable that loads it at runtime. Depending on Launch guarantees
# its output directory exists before we copy into it.
add_dependencies(${MODULE_NAME} Launch)

add_custom_command(TARGET ${MODULE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${MODULE_NAME}> $<TARGET_FILE_DIR:Launch>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_PDB_FILE:${MODULE_NAME}> $<TARGET_FILE_DIR:Launch>
)