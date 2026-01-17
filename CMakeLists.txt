cmake_minimum_required(VERSION 3.15)
project(DllInjector VERSION 1.0.0 LANGUAGES CXX)

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
)

set(HEADERS
    include/process_manager.h
    include/injector.h
)

# Executable
add_executable(DllInjector ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(DllInjector PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

# Windows specific settings
if(WIN32)
    target_compile_definitions(DllInjector PRIVATE 
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    
    # Request admin privileges
    if(MSVC)
        set_target_properties(DllInjector PROPERTIES
            LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""
        )
    endif()
endif()

# Installation
install(TARGETS DllInjector
    RUNTIME DESTINATION bin
)
