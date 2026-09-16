set (CMAKE_CXX_STANDARD 26)
set (MODULE_NAME Launch)

file (GLOB_RECURSE SOURCES
        Private/*.h
        Private/*.hpp
        Private/*.inl
        Private/*.cpp
)

add_executable(${MODULE_NAME} ${SOURCES})
# Linking Runtime transitively brings Core + Render (and their DLLs into TARGET_RUNTIME_DLLS).
target_link_libraries(${MODULE_NAME} PUBLIC Runtime)
target_include_directories(${MODULE_NAME} PRIVATE
        ${QUEST_ROOT}/Engine/Modules/Core/Public
        ${QUEST_ROOT}/Engine/Modules/Runtime/Public)

add_custom_command(TARGET ${MODULE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${MODULE_NAME}> $<TARGET_RUNTIME_DLLS:${MODULE_NAME}>
        COMMAND_EXPAND_LISTS
)