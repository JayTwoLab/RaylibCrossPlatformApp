# Raylib Cross-Platform Test Application

- A minimal cross-platform example using [raylib](https://github.com/raysan5/raylib) demonstrating a resizable window, simple input handling (keyboard, mouse, touch), and basic rendering.

## Features
- **Resizable window**: The app uses a resizable window and reports current screen size.
- **Input**: Move the circle with arrow keys, mouse, or touch input.
- **Status overlay**: Shows FPS and current screen resolution.

## Requirements
- CMake 3.24 or newer
- A C/C++ compiler supporting C++17 (MSVC, clang, gcc)
- raylib library installed and findable by CMake

## Build (basic)
1. Create a build directory and run CMake:

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

2. The produced executable name is controlled by `EXE_NAME` in [CMakeLists.txt](CMakeLists.txt).

Platform notes:
- Windows: Use Visual Studio generator or pass `-G "Ninja"` if you prefer Ninja.
- macOS: You can build with Xcode or a Unix-style generator; the CMake file has basic Apple bundle settings for iOS/macOS.
- Linux: Ensure `Threads` and common system libraries are available; the CMake script links `m` and `dl` where needed.

## Run
- Run the generated executable from the build output. Example on Unix-like systems:

```bash
./MyRaylibApp
```

## Controls
- Arrow keys: move the circle
- Mouse / touch: click or touch to move the circle to pointer position
- Esc or close window: exit

## Project layout
- `src/main.cpp` — main application source
- `CMakeLists.txt` — build configuration
- `LICENSE` — project license

## Binary
- Pre-built raylib library
   - [raylib 6.0](https://github.com/RobLoach/raylib-cpp/releases/tag/v6.0.0), MSVC 2022 : [raylib.zip](https://github.com/JayTwoLab/RaylibCrossPlatformApp/releases/download/v0.1.0/raylib.zip)
 
## License
See the `LICENSE` file at the repository root.
