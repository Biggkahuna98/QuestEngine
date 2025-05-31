set (TARGET_NAME QuestEngine)
set (QUEST_LIBRARY_TYPE SHARED)
set (CMAKE_CXX_STANDARD 23)

set_property(TARGET PROPERTY USE_FOLDERS ON)

message("Building Quest Engine ${QUEST_LIBRARY_TYPE} library...")

file (GLOB_RECURSE SOURCES 
	Include/*.hpp
	Include/*.h
	Include/*.inl
	Include/*.cpp

	Source/*.h 
	Source/*.hpp
	Source/*.inl
	Source/*.cpp

	Tests/*.h 
	Tests/*.hpp
	Tests/*.inl
	Tests/*.cpp  
)

file (GLOB_RECURSE ENGINE_SHADERS
	Resources/Shaders/*.vert
	Resources/Shaders/*.frag
)

set (IMGUI_SOURCES
	${THIRD_PARTY_ENGINE_DIR}/imgui/imgui.cpp
	${THIRD_PARTY_ENGINE_DIR}/imgui/imgui_demo.cpp
	${THIRD_PARTY_ENGINE_DIR}/imgui/imgui_draw.cpp
	${THIRD_PARTY_ENGINE_DIR}/imgui/imgui_tables.cpp
	${THIRD_PARTY_ENGINE_DIR}/imgui/imgui_widgets.cpp
	${THIRD_PARTY_ENGINE_DIR}/imgui/backends/imgui_impl_vulkan.cpp
	${THIRD_PARTY_ENGINE_DIR}/imgui/backends/imgui_impl_glfw.cpp
)

message(STATUS ${ENGINE_SHADERS})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${ENGINE_SHADERS})

#set_source_files_properties(${SHADERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(${TARGET_NAME} ${QUEST_LIBRARY_TYPE} ${SOURCES} ${IMGUI_SOURCES})
set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_compile_definitions(${TARGET_NAME} PRIVATE 
	_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS=1
	_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING=1
	_CRT_SECURE_NO_WARNINGS=1
	GLM_FORCE_DEPTH_ZERO_TO_ONE=1
	QE_PLATFORM_WINDOWS=1
	QE_INTERNAL_USE_ONLY=1
	QE_RESOURCES_FOLDER="${QUEST_ROOT}/Engine/Resources/"

	$<$<CONFIG:Debug>:QE_DEBUG_MODE=1> 
	$<$<CONFIG:RelWithDebInfo>:QE_DEBUG_MODE=1>
)

# Guess I need this line to make sure I can find my local includes
target_include_directories(${TARGET_NAME} PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/Include/ # Public includes
	${CMAKE_CURRENT_SOURCE_DIR}/Source/  # Private includes
)

target_compile_options(${TARGET_NAME} PRIVATE -W4)

# Find Vulkan
find_package(Vulkan REQUIRED COMPONENTS glslc glslang SPIRV-Tools shaderc_combined)
if (NOT Vulkan_FOUND)
	message(STATUS "Vulkan not found. Please install it.")
	return()
endif()

set(VULKAN_LIBRARIES
	${Vulkan_LIBRARY}
	${Vulkan_glslang_LIBRARY}
	${Vulkan_shaderc_combined_LIBRARY}
	${Vulkan_SPIRV-Tools_LIBRARY}
)

# Get the 3rd party dependencies
add_subdirectory(${THIRD_PARTY_ENGINE_DIR}/glm ThirdParty/glm)
add_subdirectory(${THIRD_PARTY_ENGINE_DIR}/glfw ThirdParty/glfw)
add_subdirectory(${THIRD_PARTY_ENGINE_DIR}/spdlog ThirdParty/spdlog)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
add_subdirectory(${THIRD_PARTY_ENGINE_DIR}/assimp ThirdParty/assimp)
add_subdirectory(${THIRD_PARTY_ENGINE_DIR}/glaze ThirdParty/glaze)

#add_subdirectory(${THIRD_PARTY_ENGINE_DIR}/imgui ThirdParty/imgui)

# Set vendor includes
set(VENDOR_INCLUDES
	${THIRD_PARTY_ENGINE_DIR}/glm/glm
	${THIRD_PARTY_ENGINE_DIR}/imgui
	${THIRD_PARTY_ENGINE_DIR}/glfw/include
	${THIRD_PARTY_ENGINE_DIR}/spdlog/include
	${THIRD_PARTY_ENGINE_DIR}/assimp/include
	${THIRD_PARTY_ENGINE_DIR}/stb
	${THIRD_PARTY_ENGINE_DIR}/glaze/include
)

# Link private includes
target_link_libraries(${TARGET_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw spdlog assimp)
target_link_libraries(${TARGET_NAME} PUBLIC glm spdlog glaze::glaze)

# Include vendor includes
target_include_directories(${TARGET_NAME} PRIVATE ${Vulkan_INCLUDE_DIR} ${VENDOR_INCLUDES})
target_include_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_ENGINE_DIR}/glm/glm ${THIRD_PARTY_ENGINE_DIR}/imgui)

message("Done building Quest Engine")

if(DEFINED CURRENT_CMAKE_PRESET_NAME)
	message(STATUS "Preset name: ${CURRENT_CMAKE_PRESET_NAME}")
	# copy dll to runtime .exe folder
	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
			COMMAND "${CMAKE_COMMAND}" -E copy
			"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/Engine/QuestEngine.dll"
			"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/Runtime/"
			COMMENT "Copying QuestEngine.dll to QuestRuntime.exe directory"
	)
	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
			COMMAND "${CMAKE_COMMAND}" -E copy
			"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/Engine/QuestEngine.ilk"
			"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/Runtime/"
			COMMENT "Copying QuestEngine.dll to QuestRuntime.exe directory"
	)
	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
			COMMAND "${CMAKE_COMMAND}" -E copy
			"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/Engine/QuestEngine.pdb"
			"${QUEST_ROOT}/Output/Build/${CURRENT_CMAKE_PRESET_NAME}/Runtime/"
			COMMENT "Copying QuestEngine.dll to QuestRuntime.exe directory"
	)
endif()