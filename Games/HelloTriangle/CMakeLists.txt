#general information 
cmake_minimum_required(VERSION 3.31)

#cpp options
set(CMAKE_CXX_STANDARD 23)

#Common options
project(HelloTriangle VERSION 1.0.0)

set(GAME_SOURCE_FILES 
    main.cpp
)

add_executable(${PROJECT_NAME} ${GAME_SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_link_libraries(${PROJECT_NAME} PUBLIC ${ENGINE_LIBRARY_TARGET})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC /Zc:preprocessor)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Games/HelloTriangle)

# ============================================
# Shaders
# ============================================
find_program(GLSLC glslc HINTS $ENV{VULKAN_SDK}/Bin)

if(NOT GLSLC)
    message(FATAL_ERROR "glslc not found!")
endif()

set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Shaders)
set(SHADER_OUTPUT_DIR ${CMAKE_BINARY_DIR}/Games/${PROJECT_NAME}/Binaries/Shaders)

file(GLOB SHADER_SOURCES "${SHADER_SOURCE_DIR}/*.vert" "${SHADER_SOURCE_DIR}/*.frag")
message(STATUS "Found shaders: ${SHADER_SOURCES}")

foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SPV_OUTPUT "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv")
 
    message(STATUS "Compiling: ${SHADER_NAME} → ${SPV_OUTPUT}")

    # ★ Compile whene generated ★
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_OUTPUT_DIR}
        COMMAND ${GLSLC} ${SHADER} -o ${SPV_OUTPUT}
        RESULT_VARIABLE RESULT
        ERROR_VARIABLE ERROR_MSG
    )

    if(NOT RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to compile shader: ${SHADER_NAME}\n${ERROR_MSG}")
    endif()

endforeach()


# ★ Copying to folder ★
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources/Shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_OUTPUT_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources/Shaders
    COMMENT "Copying shaders to executable directory"
)