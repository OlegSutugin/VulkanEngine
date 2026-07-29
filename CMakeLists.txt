cmake_minimum_required(VERSION 3.31.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PROJECT_NAME Vulkan)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) #executable file in that directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) #libraries to that directory too

set_property(GLOBAL PROPERTY USE_FOLDERS ON) 
set(CMAKE_SUPPRESS_REGENERATION  TRUE)  #deleting ZERO_CHECK project in IDE

# Structure
add_subdirectory(Engine)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${ENGINE_EXE_TARGET})

include(${CMAKE_SOURCE_DIR}/Automation/CMAKE/CMakeHelpers.cmake)

