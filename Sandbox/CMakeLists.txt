set_property(TARGET PROPERTY USE_FOLDERS ON)

file (GLOB_RECURSE SOURCES
	Source/*.h 
	Source/*.hpp
	Source/*.inl
	Source/*.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})


add_executable(Sandbox ${SOURCES})

target_compile_definitions(Sandbox PRIVATE _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS=1 _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING=1)
target_include_directories(Sandbox PRIVATE ${QUEST_ROOT}/QuestEngine/Include/)
target_link_libraries(Sandbox QuestEngine)

add_custom_command(TARGET Sandbox POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:Sandbox> $<TARGET_RUNTIME_DLLS:Sandbox>
  COMMAND_EXPAND_LISTS
)