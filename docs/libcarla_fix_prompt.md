# libcarla Conan Package Fix Request

## Problem Summary

The `libcarla/0.10.0` Conan package has missing RPC library symbols, causing link errors when building C++ applications that use the CARLA client library.

## Error Details

### Link Errors Encountered
```
undefined reference to `rpc::client::get_timeout() const'
undefined reference to `rpc::client::~client()'
undefined reference to `rpc::client::throw_timeout(...)'
undefined reference to `rpc::client::wait_conn()'
undefined reference to `rpc::client::get_next_call_idx()'
undefined reference to `rpc::client::post(...)'
undefined reference to `typeinfo for rpc::timeout'
```

### Build Environment
- **Compiler**: GCC 11.4.0
- **C++ Standard**: C++20 (also tested with C++17)
- **Platform**: Linux x86_64
- **Conan Version**: 2.x
- **libcarla Package**: libcarla/0.10.0 from hakuturu583 repository
- **Build Type**: Release

### Package Configuration Used
```txt
[requires]
libcarla/0.10.0

[options]
libcarla/*:with_client=True
libcarla/*:with_server=False
```

### CMakeLists.txt Configuration
```cmake
cmake_minimum_required(VERSION 3.15)
project(carla_sample CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(carla REQUIRED)

add_executable(carla_sample main.cpp)
target_link_libraries(carla_sample PRIVATE libcarla::libcarla)
# Also tested with: carla::carla-client
```

## Root Cause Analysis

The libcarla static library (`libcarla-client.a`) references RPC symbols but the RPC library is not being:
1. Included in the Conan package, OR
2. Properly linked as a dependency, OR
3. Declared in the `package_info()` method of the Conan recipe

## Investigation Tasks

Please investigate the following in the libcarla Conan recipe:

### 1. Check Dependencies in conanfile.py

```python
def requirements(self):
    # Is rpclib or similar RPC library declared?
    # Example of what might be missing:
    # self.requires("rpclib/2.3.0")
```

### 2. Check Build Configuration

```python
def build(self):
    # Is rpclib being linked during the build?
    # Check CMake arguments or build flags
```

### 3. Check package_info() Method

```python
def package_info(self):
    # Are all necessary libraries declared?
    # Example of what might be missing:
    self.cpp_info.components["carla-client"].libs = [
        "carla-client",
        # "rpc",  # <-- Possibly missing
    ]

    # Or should rpclib be declared as a requirement?
    self.cpp_info.components["carla-client"].requires = [
        # "rpclib::rpclib",  # <-- Possibly missing
    ]
```

### 4. Check Static Library Contents

```bash
# Verify what symbols are actually in libcarla-client.a
nm -C libcarla-client.a | grep "rpc::client"
# Should show defined symbols, not just undefined references
```

## Recommended Fixes

Based on the error patterns, the fix likely involves one or more of:

### Option A: Add rpclib as a Conan Requirement

```python
def requirements(self):
    # Add RPC library dependency
    self.requires("rpclib/2.3.0")  # or appropriate version
    # ... other requirements
```

### Option B: Include RPC Library in Package

If rpclib is built as part of libcarla:

```python
def package(self):
    # Ensure RPC library is copied to package
    self.copy("*.a", dst="lib", keep_path=False)

def package_info(self):
    # Declare both libraries
    self.cpp_info.components["carla-client"].libs = [
        "carla-client",
        "carla-rpc",  # or whatever the RPC lib is named
    ]
```

### Option C: Fix Link Order

If the libraries exist but aren't linked correctly:

```python
def package_info(self):
    # Ensure proper link order
    self.cpp_info.components["carla-client"].libs = [
        "carla-client",
    ]
    # Declare rpclib dependency
    self.cpp_info.components["carla-client"].requires = [
        "rpclib::rpclib",
    ]
```

## Testing the Fix

After implementing the fix, test with:

```bash
# Clean and rebuild
rm -rf build ~/.conan2/p/libca*
conan install . --output-folder=build --build=missing
cmake --preset conan-release
cmake --build --preset conan-release

# Verify no link errors
./build/build/Release/carla_sample
```

## Additional Information

### CARLA Version
This package appears to be for CARLA 0.10.0. Please verify:
- Which CARLA version this corresponds to (0.9.x series?)
- Whether the upstream CARLA source properly includes rpclib

### Upstream References
- CARLA Official: https://github.com/carla-simulator/carla
- CARLA uses rpclib: https://github.com/rpclib/rpclib
- CARLA typically bundles rpclib in `LibCarla/cmake/client/CMakeLists.txt`

## Request

Please review the libcarla/0.10.0 Conan recipe and implement the necessary fixes to include the RPC library symbols. The package should be able to link successfully without requiring users to manually add rpclib as a dependency.

Thank you for maintaining this package!
