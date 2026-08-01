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
