cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Use folders nicely on Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(QuestEngine)
set(CMAKE_CXX_STANDARD 20)

# Set QUEST_ROOT
set(QUEST_ROOT ${CMAKE_SOURCE_DIR})

# Third party directory
set(THIRD_PARTY_DIR ${QUEST_ROOT}/ThirdParty)

# Third party (engine) directory
set(THIRD_PARTY_ENGINE_DIR ${THIRD_PARTY_DIR}/Engine)

# Add the engine
add_subdirectory(Engine)

# Add the test project
add_subdirectory(Sandbox)

# Set startup project to Sandbox (change later to only do it with visual studio)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)