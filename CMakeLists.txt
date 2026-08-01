cmake_minimum_required(VERSION 3.31.0)

#setup conan profiles
set(PROFILE_PATH_Windows_Release "${CMAKE_CURRENT_SOURCE_DIR}/Automation/ConanProfiles/WindowsRelease")
set(PROFILE_PATH_Windows_Debug "${CMAKE_CURRENT_SOURCE_DIR}/Automation/ConanProfiles/WindowsDebug")
set(PROFILE_PATH_Linux_Release "${CMAKE_CURRENT_SOURCE_DIR}/Automation/ConanProfiles/LinuxRelease")
set(PROFILE_PATH_Linux_Debug "${CMAKE_CURRENT_SOURCE_DIR}/Automation/ConanProfiles/LinuxDebug")

set(CURRENT_PROFILE "PROFILE_PATH_${CMAKE_HOST_SYSTEM_NAME}_${CMAKE_BUILD_TYPE}")
message(STATUS "set conan profile: ${CURRENT_PROFILE}")

if(DEFINED ${CURRENT_PROFILE})
    set(CONAN_PROFILE "${${CURRENT_PROFILE}}")
    message(STATUS "using conan profile: ${CONAN_PROFILE}")
else()
    message(FATAL_ERROR "Unsupported configuration for: ${CURRENT_PROFILE}")
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/Automation/CMAKE/CMakeHelpers.cmake)
setup_conan(${CONAN_PROFILE} ${CONAN_PROFILE})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PROJECT_NAME Vulkan)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) #executable file in that directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) #libraries to that directory too

set_property(GLOBAL PROPERTY USE_FOLDERS ON) 
set(CMAKE_SUPPRESS_REGENERATION  TRUE)  #deleting ZERO_CHECK project in IDE

# Structure

# ============================================
# Engine Library
# ============================================
add_subdirectory(Engine)

# ============================================
# Games
# ============================================
add_subdirectory(Games/HelloTriangle)

# ============================================
# Startup Project
# ============================================
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HelloTriangle)

