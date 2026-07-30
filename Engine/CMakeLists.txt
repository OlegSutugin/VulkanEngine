cmake_minimum_required(VERSION 3.31.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(VulkanEngine)
add_executable(${PROJECT_NAME} Source/main.cpp)

set(ENGINE_SOURCE_FILES
	Source/Core/Engine.h
	Source/Core/Engine.cpp
	Source/Core/Utility.h
	
	Source/Log/Log.h
	Source/Log/Log.cpp

	Source/Window/GLFW/GLFWWindow.h
	Source/Window/GLFW/GLFWWindow.cpp
	Source/Window/GLFW/GLFWWindowManager.h
	Source/Window/GLFW/GLFWWindowManager.cpp

	Source/pch.cpp
	Source/pch.h
	.clang-format
)

add_library(${PROJECT_NAME}Lib ${ENGINE_SOURCE_FILES})
target_include_directories(${PROJECT_NAME}Lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)

# Add GLFW library
add_subdirectory(ThirdParty/glfw-3.4 "${CMAKE_CURRENT_BINARY_DIR}/glfw_build")
include_directories(ThirdParty/glfw-3.4/include)
set_property(TARGET glfw PROPERTY FOLDER thirdparty/GLFW3)
set_property(TARGET uninstall PROPERTY FOLDER thirdparty/GLFW3)
set_property(TARGET update_mappings PROPERTY FOLDER thirdparty/GLFW3)

# Add conan libs
find_package(spdlog REQUIRED)

# Link libraries
target_link_libraries(${PROJECT_NAME}Lib PUBLIC glfw)
target_link_libraries(${PROJECT_NAME}Lib PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}Lib)

if(MSVC)
    target_compile_options(${PROJECT_NAME}Lib PUBLIC /Zc:preprocessor)
endif()

#folders
set_property(GLOBAL PROPERTY USE_FOLDERS On)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Engine)
set_target_properties(${PROJECT_NAME}Lib PROPERTIES FOLDER Engine)

include(${CMAKE_SOURCE_DIR}/Automation/CMAKE/CMakeHelpers.cmake)

set(ENGINE_EXE_TARGET ${PROJECT_NAME} PARENT_SCOPE) #for outer list

create_ide_folders(ENGINE_SOURCE_FILES)
setup_precompiled_headers(${PROJECT_NAME}Lib Source/pch.cpp Source/pch.h "${ENGINE_SOURCE_FILES}")