cmake_minimum_required(VERSION 3.31.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(VulkanEngine)
add_executable(${PROJECT_NAME} Source/main.cpp)

set(ENGINE_SOURCE_FILES
	Source/Core/Engine.h
	Source/Core/Engine.cpp
	Source/pch.cpp
	Source/pch.h
	.clang-format
)

add_library(${PROJECT_NAME}Lib ${ENGINE_SOURCE_FILES})
target_include_directories(${PROJECT_NAME}Lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)

# Link libraries
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}Lib)

#folders
set_property(GLOBAL PROPERTY USE_FOLDERS On)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Engine)
set_target_properties(${PROJECT_NAME}Lib PROPERTIES FOLDER Engine)

include(${CMAKE_SOURCE_DIR}/Automation/CMAKE/CMakeHelpers.cmake)

set(ENGINE_EXE_TARGET ${PROJECT_NAME} PARENT_SCOPE) #for outer list

create_ide_folders(ENGINE_SOURCE_FILES)
setup_precompiled_headers(${PROJECT_NAME}Lib Source/pch.cpp Source/pch.h "${ENGINE_SOURCE_FILES}")