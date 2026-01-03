set_property(TARGET PROPERTY USE_FOLDERS ON)
set (CMAKE_CXX_STANDARD 23)

file (GLOB_RECURSE SOURCES
	Source/*.h 
	Source/*.hpp
	Source/*.inl
	Source/*.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})


add_library(RHITest SHARED ${SOURCES})

target_compile_definitions(RHITest PRIVATE
		_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS=1
		_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING=1
		BUILD_DLL
)
target_include_directories(RHITest PRIVATE ${QUEST_ROOT}/Engine/Include/ ${THIRD_PARTY_ENGINE_DIR}/imgui)
target_link_libraries(RHITest QuestEngine)

if(DEFINED CURRENT_CMAKE_PRESET_NAME)
	message(STATUS "Preset name: ${CURRENT_CMAKE_PRESET_NAME}")
	# copy dll to runtime .exe folder
	add_custom_command(TARGET RHITest POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/RHITest/RHITest.dll"
		"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/Runtime/"
		COMMENT "Copying RHITest.dll to QuestRuntime.exe directory"
	)
	#[[
	add_custom_command(TARGET RHITest POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/RHITest/RHITest.ilk"
		"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/Runtime/"
		COMMENT "Copying RHITest.ilk to QuestRuntime.exe directory"
	)
	]]
	add_custom_command(TARGET RHITest POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/RHITest/RHITest.pdb"
		"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/Runtime/"
		COMMENT "Copying RHITest.pdb to QuestRuntime.exe directory"
	)
endif()