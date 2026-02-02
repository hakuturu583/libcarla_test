# [BUG] Missing RPC library symbols in libcarla/0.10.0

## Description
The `libcarla/0.10.0` Conan package is missing RPC library symbols, causing link errors when building C++ applications.

## Environment
- **Package**: libcarla/0.10.0
- **Compiler**: GCC 11.4.0
- **Platform**: Linux x86_64
- **Conan**: 2.x

## Link Errors
```
undefined reference to `rpc::client::get_timeout() const'
undefined reference to `rpc::client::~client()'
undefined reference to `rpc::client::throw_timeout(...)'
undefined reference to `rpc::client::wait_conn()'
undefined reference to `rpc::client::get_next_call_idx()'
undefined reference to `rpc::client::post(...)'
undefined reference to `typeinfo for rpc::timeout'
```

## Reproduction
```bash
# conanfile.txt
[requires]
libcarla/0.10.0

[options]
libcarla/*:with_client=True
libcarla/*:with_server=False

# Install and build
conan install . --output-folder=build --build=missing
cmake --preset conan-release
cmake --build --preset conan-release
# Link error occurs
```

## Root Cause
The `libcarla-client.a` static library references RPC symbols but the RPC library is not:
1. Included in the Conan package, OR
2. Properly declared as a dependency, OR
3. Listed in `package_info()` method

## Possible Solutions

### Option 1: Add rpclib dependency
```python
# In conanfile.py
def requirements(self):
    self.requires("rpclib/2.3.0")
```

### Option 2: Include RPC library in package
```python
def package_info(self):
    self.cpp_info.components["carla-client"].libs = [
        "carla-client",
        "carla-rpc",  # Add RPC library
    ]
```

### Option 3: Declare rpclib requirement
```python
def package_info(self):
    self.cpp_info.components["carla-client"].requires = [
        "rpclib::rpclib",
    ]
```

## Additional Details
See `docs/libcarla_fix_prompt.md` for comprehensive investigation tasks and testing procedures.

## Request
Please review and fix the libcarla/0.10.0 Conan recipe to properly include or link the RPC library.
