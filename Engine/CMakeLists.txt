cmake_minimum_required(VERSION 3.31.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(VulkanEngine)

set(ENGINE_SOURCE_FILES
	Source/Core/Engine.h
	Source/Core/Engine.cpp
	Source/Core/Utility.h
	Source/Core/GameConfig.h
	Source/Core/EngineTypes.h
	Source/Core/PlatformDefines.h
	
	Source/Log/Log.h
	Source/Log/Log.cpp

	Source/Runtime/Camera.h
	Source/Runtime/Camera.cpp
	Source/Runtime/Input.h
	Source/Runtime/Input.cpp

	Source/Render/IRenderer.h
	Source/Render/Vertex.h
	Source/Render/MeshDesc.h
	Source/Render/Vulkan/VulkanRenderer.h
	Source/Render/Vulkan/VulkanRenderer.cpp
	Source/Render/Vulkan/VulkanVertexLayout.h
	Source/Render/Vulkan/VulkanVertexLayout.cpp
	Source/Render/Vulkan/VulkanSwapChain.cpp
	Source/Render/Vulkan/VulkanPipeline.cpp
	Source/Render/Vulkan/VulkanDescriptors.cpp
	Source/Render/Vulkan/VulkanBuffers.cpp
	Source/Render/Vulkan/VulkanCommands.cpp
	Source/Render/Vulkan/VulkanDeviceHelpers.cpp

	Source/Window/GLFW/GLFWWindow.h
	Source/Window/GLFW/GLFWWindow.cpp
	Source/Window/GLFW/GLFWWindowManager.h
	Source/Window/GLFW/GLFWWindowManager.cpp

	Source/pch.cpp
	Source/pch.h
	.clang-format

	Source/Libraries/Common/Math.h

	Source/Libraries/Math3D/Math3D.h
	Source/Libraries/Math3D/Vec2.h
	Source/Libraries/Math3D/Vec3.h
	Source/Libraries/Math3D/Vec4.h
	Source/Libraries/Math3D/Mat4.h
)

add_library(${PROJECT_NAME}Lib ${ENGINE_SOURCE_FILES})
target_include_directories(${PROJECT_NAME}Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source)

# Add GLFW library
add_subdirectory(ThirdParty/glfw-3.4 "${CMAKE_CURRENT_BINARY_DIR}/glfw_build")
include_directories(ThirdParty/glfw-3.4/include)
set_property(TARGET glfw PROPERTY FOLDER thirdparty/GLFW3)
set_property(TARGET uninstall PROPERTY FOLDER thirdparty/GLFW3)
set_property(TARGET update_mappings PROPERTY FOLDER thirdparty/GLFW3)

# Add glm algebra library
target_include_directories(${PROJECT_NAME}Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glm)

# Add conan libs
find_package(spdlog REQUIRED)
find_package(Vulkan)

# Link libraries
target_link_libraries(${PROJECT_NAME}Lib PUBLIC glfw)
target_link_libraries(${PROJECT_NAME}Lib PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME}Lib PUBLIC Vulkan::Vulkan)

if(MSVC)
    target_compile_options(${PROJECT_NAME}Lib PUBLIC /Zc:preprocessor)
endif()

#folders
set_property(GLOBAL PROPERTY USE_FOLDERS On)
set_target_properties(${PROJECT_NAME}Lib PROPERTIES FOLDER Engine)

include(${CMAKE_SOURCE_DIR}/Automation/CMAKE/CMakeHelpers.cmake)

set(ENGINE_LIBRARY_TARGET ${PROJECT_NAME}Lib PARENT_SCOPE)

create_ide_folders(ENGINE_SOURCE_FILES)
setup_precompiled_headers(${PROJECT_NAME}Lib Source/pch.cpp Source/pch.h "${ENGINE_SOURCE_FILES}")