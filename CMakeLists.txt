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

# -------------------------------------------------------------
# imgui 1. 공통 ImGui 코어 소스 정의
# -------------------------------------------------------------
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

set(IMGUI_CORE_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

set(PLATFORM_SOURCES "")
set(PLATFORM_LIBS "")
set(PLATFORM_DEFS "") 

# -------------------------------------------------------------
# imgui 2. OS 및 플랫폼별 분기 설정
# -------------------------------------------------------------

# (1) Android (NDK 빌드 환경)
if(ANDROID)
    message(STATUS "Configuring for Android")

    # Android 전용 백엔드 소스
    list(APPEND PLATFORM_SOURCES ${IMGUI_DIR}/backends/imgui_impl_android.cpp)

    # OpenGL ES 3.0 매크로 설정 (imgui_impl_opengl3.cpp 내부 분기용)
    list(APPEND PLATFORM_DEFS IMGUI_IMPL_OPENGL_ES3)

    # NDK 시스템 라이브러리 링크
    list(APPEND PLATFORM_LIBS
        GLESv3
        EGL
        android
        log
    )

# (2) iOS
elseif(IOS)
    message(STATUS "Configuring for iOS")

    # iOS용 OpenGL ES 3.0 매크로 및 프레임워크 링크
    list(APPEND PLATFORM_DEFS
        IMGUI_IMPL_OPENGL_ES3
        GL_SILENCE_DEPRECATION
    )
    list(APPEND PLATFORM_LIBS
        "-framework OpenGLES"
        "-framework UIKit"
        "-framework Foundation"
        "-framework QuartzCore"
    )
    # ※ iOS에서 GLFW 대신 SDL2/3 또는 자체 UIKit 뷰를 연결할 경우 해당 백엔드 추가

# (3) Desktop (Windows, macOS, Linux)
else()
    message(STATUS "Configuring for Desktop")

    # GLFW 백엔드 추가
    list(APPEND PLATFORM_SOURCES ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp)

    # 패키지 탐색
    find_package(OpenGL REQUIRED)
    find_package(glfw3 CONFIG QUIET)
    if(NOT TARGET glfw AND NOT TARGET glfw3::glfw)
        find_package(glfw3 REQUIRED)
    endif()

    # GLFW 타겟 링크 설정
    if(TARGET glfw3::glfw)
        list(APPEND PLATFORM_LIBS glfw3::glfw)
    else()
        list(APPEND PLATFORM_LIBS glfw)
    endif()

    list(APPEND PLATFORM_LIBS OpenGL::GL)

    # OS별 추가 설정
    if(APPLE)
        # macOS 데스크톱
        list(APPEND PLATFORM_DEFS GL_SILENCE_DEPRECATION)
        list(APPEND PLATFORM_LIBS
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
        )
    elseif(UNIX)
        # Linux
        list(APPEND PLATFORM_LIBS ${CMAKE_DL_LIBS} pthread)
    elseif(WIN32)
        # Windows 로더 정의 (GLAD 등을 사용하는 경우)
        # list(APPEND PLATFORM_DEFS IMGUI_IMPL_OPENGL_LOADER_GLAD)
    endif()
endif()

# -------------------------------------------------------------
# imgui 3. 타겟 생성 및 속성 부여
# -------------------------------------------------------------

# Android에서는 단독 실행 파일 대신 JNI용 공유 라이브러리(SHARED)로 만드는 경우가 많으나,
# ImGui 자체는 모든 환경에서 STATIC 라이브러리로 빌드하는 것이 표준적입니다.
add_library(imgui STATIC
    ${IMGUI_CORE_SOURCES}
    ${PLATFORM_SOURCES}
)

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_compile_definitions(imgui PUBLIC ${PLATFORM_DEFS})
target_link_libraries(imgui PUBLIC ${PLATFORM_LIBS})


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
target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/imgui")

target_link_libraries(${EXE_NAME} PRIVATE raylib)
target_link_libraries(${EXE_NAME} PRIVATE imgui)

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





 


