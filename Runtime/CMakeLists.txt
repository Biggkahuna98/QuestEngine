set_property(TARGET PROPERTY USE_FOLDERS ON)
set (CMAKE_CXX_STANDARD 23)

file (GLOB_RECURSE SOURCES
        Source/*.h
        Source/*.hpp
        Source/*.inl
        Source/*.cpp
)

add_executable(QuestRuntime ${SOURCES})
target_link_libraries(QuestRuntime PRIVATE QuestEngine)
target_include_directories(QuestRuntime PRIVATE ${QUEST_ROOT}/Engine/Include/)

add_custom_command(TARGET QuestRuntime POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:QuestRuntime> $<TARGET_RUNTIME_DLLS:QuestRuntime>
  COMMAND_EXPAND_LISTS
)