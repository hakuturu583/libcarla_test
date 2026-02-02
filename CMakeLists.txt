cmake_minimum_required(VERSION 3.15)
project(carla_sample CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Conan configuration
find_package(carla REQUIRED)

# Create executable
add_executable(carla_sample main.cpp)

# Link libraries
target_link_libraries(carla_sample PRIVATE libcarla::libcarla)

# Compiler options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(carla_sample PRIVATE -Wall -Wextra)
endif()
