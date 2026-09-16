cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(QuestEngine)
set(CMAKE_CXX_STANDARD 26)
set(QUEST_ROOT ${CMAKE_SOURCE_DIR})
set(QUEST_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Modules)
set(QUEST_THIRD_PARTY_LIBS ${QUEST_ROOT}/ThirdParty)
set(QUEST_BUILD_LIB_TYPE SHARED)

# Setup easy linking of libs in the ThirdParty folder
link_directories(ThirdParty)

# Add the engine directory
add_subdirectory(Engine)

# Add consumer directories
add_subdirectory(Sandbox)