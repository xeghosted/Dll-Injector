cmake_minimum_required(VERSION 3.15)
project(DllInjectorGUI VERSION 1.0.0 LANGUAGES CXX RC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection
if(MSVC)
    # Visual Studio specific settings will be applied
elseif(MINGW)
    # MinGW specific settings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Source files
set(SOURCES
    src/main.cpp
    src/process_manager.cpp
    src/injector.cpp
    src/main_window.cpp
)

set(HEADERS
    include/process_manager.h
    include/injector.h
    include/main_window.h
    include/resource.h
)

# Resource file
set(RESOURCES
    resources/resource.rc
)

# Executable (GUI application - no console window)
add_executable(DllInjectorGUI WIN32 ${SOURCES} ${HEADERS} ${RESOURCES})

# Include directories
target_include_directories(DllInjectorGUI PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/resources
)

# Windows specific settings
if(WIN32)
    target_compile_definitions(DllInjectorGUI PRIVATE 
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    
    # Link against common controls
    target_link_libraries(DllInjectorGUI PRIVATE
        comctl32
        dwmapi
        uxtheme
    )
    
    # Request admin privileges
    if(MSVC)
        set_target_properties(DllInjectorGUI PROPERTIES
            LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""
        )
    endif()
endif()

# Installation
install(TARGETS DllInjectorGUI
    RUNTIME DESTINATION bin
)
