cmake_minimum_required(VERSION 3.24)

# Set target system before project() for iOS
if(IOS)
    set(CMAKE_SYSTEM_NAME iOS)
endif()

# Project declaration
project(RaylibCrossPlatformApp C CXX)

# C standards
if (NOT DEFINED CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 99)
endif()

# C++ standards
if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
 
# ------------------------------------------------------------------------------
# 1. Executable Name Configuration
# ------------------------------------------------------------------------------
set(EXE_NAME "MyRaylibApp")

# ------------------------------------------------------------------------------
# 2. Source Code Discovery (Limit GLOB scope)
# ------------------------------------------------------------------------------
# Search primarily the src folder to avoid including files inside build directories (out, build).
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/module/Engine/*.cpp"
)

# If you want to include sources from the root path, it's safer to register them explicitly as follows.
# list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

if(NOT SOURCES)
    message(FATAL_ERROR "No source files (.c or .cpp) found in 'src/' directory.")
endif()

# ------------------------------------------------------------------------------
# 3. Find raylib Package
# ------------------------------------------------------------------------------
find_package(raylib REQUIRED)

# rres 활성화 여부를 결정하는 옵션 정의 (기본값 OFF)
option(ENABLE_RRES "Build with rres resource packaging" OFF)

# ------------------------------------------------------------------------------
# 4. Target Creation (Executable / Library)
# ------------------------------------------------------------------------------
if(ANDROID)
    add_library(${EXE_NAME} SHARED ${SOURCES})
elseif(IOS)
    add_executable(${EXE_NAME} MACOSX_BUNDLE ${SOURCES})
else()
    add_executable(${EXE_NAME} ${SOURCES})
endif()

# 옵션이 켜져 있을 때만 USE_RRES 전처리기 매크로 정의
if(ENABLE_RRES)
    target_compile_definitions(${EXE_NAME} PRIVATE USE_RRES)
    message(STATUS "RRES Mode: ENABLED (USE_RRES defined)")
else()
    message(STATUS "RRES Mode: DISABLED (Disk mode)")
endif()

target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/raylib-cpp")
target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/module")
target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/rres")

target_link_libraries(${EXE_NAME} PRIVATE raylib)

# Define CURRENT_PROJECT_DIR for use in the source code
target_compile_definitions(${EXE_NAME} PRIVATE 
    CURRENT_PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

# ------------------------------------------------------------------------------
# 5. Platform and Compiler Specific Configurations
# ------------------------------------------------------------------------------

# --- MSVC Compiler Specific Configurations ---
if(MSVC)
    target_compile_options(${EXE_NAME} PRIVATE /utf-8 /wd4828)
    target_compile_definitions(${EXE_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS NOGDI)
    target_link_libraries(${EXE_NAME} PRIVATE winmm)
endif()

# --- Windows ---
if(WIN32)
    # set_target_properties(${EXE_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# --- macOS / iOS ---
if(APPLE)
    if(IOS)
        set_target_properties(${EXE_NAME} PROPERTIES
            XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.${EXE_NAME}"
            MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        )
    endif()
endif()

# --- Android ---
if(ANDROID)
    target_link_libraries(${EXE_NAME} PRIVATE log android EGL GLESv2 OpenSLES)
endif()

# --- Linux ---
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    find_package(Threads REQUIRED)
    target_link_libraries(${EXE_NAME} PRIVATE Threads::Threads m dl)
endif()





 


