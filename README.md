# CARLA C++ Sample Project

This project is a simple scenario sample using the CARLA Simulator C++ API.

## Features

- Connect to CARLA server
- Spawn vehicles
- Control vehicles (move forward/stop)
- Display real-time position and speed

## Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2019+)
- CMake 3.15 or higher
- Conan 2.x
- Running CARLA Simulator server

## Build Instructions

### 1. Add Conan Remote (if necessary)

```bash
# Add hakuturu583's Conan repository
conan remote add hakuturu https://your-conan-remote-url
```

> **Note**: You need to add a Conan remote that hosts the `libcarla/0.10.0` package.

### 2. Install Dependencies

**Important**: Run this command from the project root directory, NOT from inside the build directory.

```bash
conan install . --output-folder=build --build=missing
```

This command will:
- Download all required dependencies (including libcarla/0.10.0)
- Generate the `conan_toolchain.cmake` file in the build directory
- Configure the build environment

### 3. Configure and Build with CMake

**Option A: Using CMake Presets (Recommended for CMake 3.23+)**

```bash
cmake --preset conan-release
cmake --build --preset conan-release
```

**Option B: Manual Configuration**

```bash
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## How to Run

### 1. Start CARLA Server

First, start the CARLA server:

```bash
# In the CARLA installation directory
./CarlaUE5.sh
# Or on Windows:
# CarlaUE5.exe
```

### 2. Run the Sample Program

```bash
./carla_sample
# Or specify host and port:
./carla_sample localhost 2000
```

## Scenario Description

1. Connect to CARLA server
2. Spawn Tesla Model 3 (or any available vehicle)
3. Move forward at 50% throttle for 5 seconds
4. Display position and speed every 0.1 seconds
5. Apply brake to stop
6. Destroy the vehicle

## Customization

You can modify `main.cpp` to:

- Change the vehicle type
- Change spawn location
- Adjust driving duration and speed
- Add steering control
- Add sensors (camera, LiDAR, etc.)

## Known Issues

### Link Errors with libcarla/0.10.0

**Status**: ✅ FIXED

The issue was caused by missing component dependencies in the `conanfile.py`. The RPC library and RecastNavigation libraries were built and packaged correctly, but were not declared as component dependencies in the `package_info()` method.

**Fix Applied**:
The `/home/masaya/workspace/carla/conanfile.py` has been updated to properly declare internal library components (rpc, recast, detour, detourcrowd) and link them to the carla-client and carla-server components.

If you encounter this issue with an older version of the package, you can rebuild the package from the local CARLA repository:
```bash
cd /home/masaya/workspace/carla
conan create . --build=missing
```

## Troubleshooting

### Cannot connect to CARLA server

- Verify that the CARLA server is running
- Check that the port number is correct (default: 2000)
- Check firewall settings

### Build errors

- Verify that Conan dependencies are correctly installed
- Check that your compiler supports C++20 (required for libcarla)
- Make sure you ran `conan install` from the project root directory before running cmake

### CMake cannot find conan_toolchain.cmake

- This means you skipped step 2 (Install Dependencies)
- Go back to the project root directory and run:
  ```bash
  conan install . --output-folder=build --build=missing
  ```

## References

- [CARLA C++ Client Example](https://carla.readthedocs.io/en/latest/adv_cpp_client/)
- [CARLA C++ Reference](https://carla.readthedocs.io/en/latest/ref_cpp/)
- [GitHub Example](https://github.com/carla-simulator/carla/tree/master/Examples/CppClient)
- [Doxygen API Reference](http://carla.org/Doxygen/html/index.html)
