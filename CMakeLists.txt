cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(QuestEngine)
set(CMAKE_CXX_STANDARD 26)
set(QUEST_ROOT ${CMAKE_SOURCE_DIR})
set(QUEST_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Modules)
set(QUEST_THIRD_PARTY_LIBS ${QUEST_ROOT}/ThirdParty)
set(QUEST_BUILD_LIB_TYPE SHARED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)

# SDL3 ships as a prebuilt DLL + import lib. Declaring it as an imported SHARED target
# (rather than linking the .lib by raw path) is what puts SDL3.dll into
# $<TARGET_RUNTIME_DLLS:Launch>, so it gets deployed by the build instead of by hand.
add_library(SDL3::SDL3 SHARED IMPORTED GLOBAL)
set_target_properties(SDL3::SDL3 PROPERTIES
        IMPORTED_LOCATION ${QUEST_THIRD_PARTY_LIBS}/Engine/SDL3-3.4.8/lib/x64/SDL3.dll
        IMPORTED_IMPLIB ${QUEST_THIRD_PARTY_LIBS}/Engine/SDL3-3.4.8/lib/x64/SDL3.lib
        INTERFACE_INCLUDE_DIRECTORIES ${QUEST_THIRD_PARTY_LIBS}/Engine/SDL3-3.4.8/include
)

# Setup easy linking of libs in the ThirdParty folder
link_directories(ThirdParty)

# Add the engine directory
add_subdirectory(Engine)

# Add consumer directories
add_subdirectory(Sandbox)
