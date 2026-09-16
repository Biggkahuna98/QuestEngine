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

# The engine modules already build into this directory, so they need no copy at all. This
# only has work to do for prebuilt third-party DLLs (SDL3), which live outside the build
# tree. copy_if_different rather than copy: the engine DLLs resolve to files already at the
# destination, and it also avoids a sharing violation when the exe is running. Sources come
# before the destination directory; the -t form is not accepted by copy_if_different here.
add_custom_command(TARGET ${MODULE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${MODULE_NAME}> $<TARGET_FILE_DIR:${MODULE_NAME}>
        COMMAND_EXPAND_LISTS
)