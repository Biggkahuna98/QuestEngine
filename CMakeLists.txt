cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

# Use folders nicely on Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(QuestEngine)
set(CMAKE_CXX_STANDARD 20)
add_compile_options(-MP -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-pragma-once-outside-header)

# Set QUEST_ROOT
set(QUEST_ROOT ${CMAKE_SOURCE_DIR})

# Add the engine
add_subdirectory(QuestEngine)

# Add the test project
add_subdirectory(Sandbox)

# Set startup project to Sandbox (change later to only do it with visual studio)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)