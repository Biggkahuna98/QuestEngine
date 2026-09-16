set (CMAKE_CXX_STANDARD 26)
set (MODULE_NAME VulkanRHI)

file (GLOB_RECURSE SOURCES
        Private/*.h
        Private/*.hpp
        Private/*.inl
        Private/*.cpp
)

add_library(${MODULE_NAME} ${QUEST_BUILD_LIB_TYPE} ${SOURCES})
target_compile_definitions(${MODULE_NAME} PRIVATE
        VULKANRHI_INTERNAL_USE_ONLY

        VULKAN_HPP_STORAGE_SHARED
        VULKAN_HPP_STORAGE_SHARED_EXPORT
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)
target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Public)

# Linking the RHI interface target brings its Public headers AND the glm include
# directory along transitively. This is the link that makes #include "glm/glm.hpp"
# resolve inside the RHI headers when they are compiled through this module.
target_link_libraries(${MODULE_NAME} PUBLIC Core)

# Find Vulkan
find_package(Vulkan REQUIRED COMPONENTS glslc glslang SPIRV-Tools shaderc_combined)
if (NOT Vulkan_FOUND)
    message(STATUS "Vulkan not found. Please install it.")
    return()
endif()

# Find SPIRV-Cross
set(VulkanSDKPath $ENV{VULKAN_SDK})
set(SPIRV_CROSS_LIBS
        ${VulkanSDKPath}/Lib/spirv-cross-glsl.lib
        ${VulkanSDKPath}/Lib/spirv-cross-core.lib
        ${VulkanSDKPath}/Lib/spirv-cross-cpp.lib
        ${VulkanSDKPath}/Lib/spirv-cross-reflect.lib
        ${VulkanSDKPath}/Lib/SPIRV.lib
)

set(SPIRV_CROSS_LIBS-debug
        ${VulkanSDKPath}/Lib/spirv-cross-glsld.lib
        ${VulkanSDKPath}/Lib/spirv-cross-cored.lib
        ${VulkanSDKPath}/Lib/spirv-cross-cppd.lib
        ${VulkanSDKPath}/Lib/spirv-cross-reflectd.lib
        ${VulkanSDKPath}/Lib/SPIRVd.lib
)

set(VULKAN_LIBRARIES
        ${Vulkan_LIBRARY}
        ${Vulkan_glslang_LIBRARY}
        ${Vulkan_shaderc_combined_LIBRARY}
        ${Vulkan_SPIRV-Tools_LIBRARY}
)

message(STATUS "VULKAN INCLUDE DIR: " ${Vulkan_INCLUDE_DIR})

set(SLANG_LIBS
        ${PREBUILT_BINARIES}/slang/lib/slang.lib
        ${PREBUILT_BINARIES}/slang/lib/slang-rt.lib
        ${PREBUILT_BINARIES}/slang/lib/gfx.lib
        ${PREBUILT_BINARIES}/slang/lib/slang-compiler.lib
)

target_link_libraries(${MODULE_NAME} PRIVATE Vulkan::Vulkan ${SPIRV_CROSS_LIBS} SDL3::SDL3)
target_include_directories(${MODULE_NAME} PUBLIC ${Vulkan_INCLUDE_DIR} ${QUEST_THIRD_PARTY_LIBS}/Engine/SDL3-3.4.8/include)

# Propagate QE_BUILD_SHARED to consumers so VULKANRHI_API expands to dllimport.
if (QUEST_BUILD_LIB_TYPE STREQUAL "SHARED")
    target_compile_definitions(${MODULE_NAME} INTERFACE QE_BUILD_SHARED)
endif()
