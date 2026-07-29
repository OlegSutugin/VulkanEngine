
macro(system_info)
    message("======== System info ======== ")
    message("Using ${CMAKE_CXX_COMPILER_ID} compiler")
    if(WIN32)
        message("Running on Windows")
    elseif(LINUX)
        message("Running on Linux")
    endif()

    if(MSVC)
        message("Compiler  ${MSVC_VERSION} version and ${MSVC_TOOLSET_VERSION} toolset version")
    endif()

    message("Compiler Flags: ${CMAKE_CXX_FLAGS}")
    message("Compiler debug Flags: ${CMAKE_CXX_FLAGS_DEBUG}")
    message("Compiler release Flags: ${CMAKE_CXX_FLAGS_RELEASE}")
    message("Configuration types = ${CMAKE_CONFIGURATION_TYPES}")
    message("====================================== ")
endmacro()


#pch
function(setup_precompiled_headers TARGET PCH_SOURCE PCH_HEADER SOURCE_FILES)
    if(MSVC)
        get_filename_component(PCH_HEADER_NAME ${PCH_HEADER} NAME)
        target_sources(${TARGET} PRIVATE ${PCH_HEADER} ${PCH_SOURCE}) #for precompile headers
        set_source_files_properties(${PCH_SOURCE} PROPERTIES COMPILE_FLAGS "/Yc${PCH_HEADER_NAME}")

        foreach(SOURCE_FILE ${SOURCE_FILES})
            if(${SOURCE_FILE} MATCHES "\\.cpp$" AND NOT ${SOURCE_FILE} STREQUAL ${PCH_SOURCE})
                set_source_files_properties(${SOURCE_FILE} PROPERTIES COMPILE_FLAGS "/Yu${PCH_HEADER_NAME}")
            endif()
        endforeach()
        target_compile_options(${TARGET} PRIVATE "/FI${PCH_HEADER_NAME}")
    endif()
endfunction()

#conan settings
macro(setup_conan HOST_PROFILE BUILD_PROFILE)
set(CMAKE_CONAN_PATH "${CMAKE_BINARY_DIR}/conan_provider.cmake")
    if(NOT EXISTS ${CMAKE_CONAN_PATH})
        message(STATUS "Downloading conan.cmake from  https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/refs/heads/develop2/conan_provider.cmake" ${CMAKE_CONAN_PATH})
    endif()

    set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES ${CMAKE_CONAN_PATH})
    set(CONAN_HOST_PROFILE ${HOST_PROFILE})
    set(CONAN_BUILD_PROFILE ${BUILD_PROFILE})
endmacro()

macro(create_ide_folders SOURCE_FILES)
    foreach(SOURCE IN LISTS ${SOURCE_FILES})
        get_filename_component(SOURCE_PATH "${SOURCE}" PATH)
        string(REPLACE "${PROJECT_SOURCE_DIR}" "" SOURCE_PATH_REL "${SOURCE_PATH}")
        string(REPLACE "/" "\\" GROUP_PATH "${SOURCE_PATH_REL}")
        source_group("${GROUP_PATH}" FILES "${SOURCE}")
    endforeach()
endmacro()