set_property(TARGET PROPERTY USE_FOLDERS ON)
set (CMAKE_CXX_STANDARD 23)

file (GLOB_RECURSE SOURCES
	Source/*.h 
	Source/*.hpp
	Source/*.inl
	Source/*.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})


add_library(Sandbox SHARED ${SOURCES})

target_compile_definitions(Sandbox PRIVATE
		_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS=1
		_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING=1
		BUILD_DLL
)
target_include_directories(Sandbox PRIVATE ${QUEST_ROOT}/Engine/Include/ ${THIRD_PARTY_ENGINE_DIR}/imgui)
target_link_libraries(Sandbox QuestEngine)

if(DEFINED CURRENT_CMAKE_PRESET_NAME)
	message(STATUS "Preset name: ${CURRENT_CMAKE_PRESET_NAME}")
	# copy dll to runtime .exe folder
	add_custom_command(TARGET Sandbox POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/Sandbox/Sandbox.dll"
		"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/Runtime/"
		COMMENT "Copying Sandbox.dll to QuestRuntime.exe directory"
	)
	add_custom_command(TARGET Sandbox POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/Sandbox/Sandbox.ilk"
		"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/Runtime/"
		COMMENT "Copying Sandbox.dll to QuestRuntime.exe directory"
	)
	add_custom_command(TARGET Sandbox POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/Sandbox/Sandbox.pdb"
		"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/Runtime/"
		COMMENT "Copying Sandbox.dll to QuestRuntime.exe directory"
	)
endif()