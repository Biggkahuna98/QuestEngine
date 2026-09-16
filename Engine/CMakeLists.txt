add_compile_definitions(
        QE_RESOURCES_FOLDER="${QUEST_ROOT}/Engine/Resources"

        $<$<CONFIG:Debug>:QE_DEBUG_MODE>
        $<$<CONFIG:RelWithDebInfo>:QE_DEBUG_MODE>
)

if (QUEST_BUILD_LIB_TYPE STREQUAL "SHARED")
    add_compile_definitions(QE_BUILD_SHARED)
endif()

# Primary module all others implicitly link to
add_subdirectory(Modules/Core)

# RHI interface
#add_subdirectory(Modules/RHI)

# Vulkan backend implementing the RHI interface
add_subdirectory(Modules/VulkanRHI)

# Frame-management layer above the RHI (API-agnostic)
add_subdirectory(Modules/Render)

# Top-level orchestration layer (Engine + Application), drives the Renderer
add_subdirectory(Modules/Runtime)

# Launch module with the .exe
add_subdirectory(Modules/Launch)