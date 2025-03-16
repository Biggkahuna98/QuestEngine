set_property(TARGET PROPERTY USE_FOLDERS ON)

set (TARGET_NAME QuestEngine)
set (QUEST_LIBRARY_TYPE STATIC)
set (CMAKE_CXX_STANDARD 20)

message("Building Quest Engine static library...")

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

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_library(${TARGET_NAME} ${QUEST_LIBRARY_TYPE} ${SOURCES})
set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
add_compile_options(-Wno-pragma-once-outside-header)

target_compile_definitions(${TARGET_NAME} PRIVATE 
	_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS=1
	_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING=1
	_CRT_SECURE_NO_WARNINGS=1
	GLM_FORCE_DEPTH_ZERO_TO_ONE=1
	QE_PLATFORM_WINDOWS=1
)
target_compile_definitions(${TARGET_NAME} PRIVATE 
	$<$<CONFIG:DEBUG>:QE_DEBUG_MODE=1> 
	$<$<CONFIG:RELWITHDEBINFO>:QE_DEBUG_MODE=1>
)

# Guess I need this line to make sure I can find my local includes
#target_include_directories(${TARGET_NAME} PRIVATE 
#	${CMAKE_CURRENT_SOURCE_DIR}/Include/
#)

target_compile_options(${TARGET_NAME} PRIVATE -W4 -Wno-c++98-compat -Wno-c++98-compat-pedantic)

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

# Get the vcpkg dependencies
#find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Stb REQUIRED)

# Set vendor includes
set(VENDOR_INCLUDES
	${Stb_INCLUDE_DIR}
)

# Link Vulkan
target_link_libraries(${TARGET_NAME} PRIVATE ${Vulkan_LIBRARIES} glm::glm imgui::imgui spdlog::spdlog)

# Include vendor includes
target_include_directories(${TARGET_NAME} PUBLIC ${Vulkan_INCLUDE_DIR} ${VENDOR_INCLUDES})

message("Done building Quest Engine")