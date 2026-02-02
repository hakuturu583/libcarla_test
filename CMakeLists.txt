cmake_minimum_required(VERSION 3.15)
project(carla_sample CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Conan設定
find_package(carla REQUIRED)

# 実行ファイルの作成
add_executable(carla_sample main.cpp)

# ライブラリのリンク
target_link_libraries(carla_sample PRIVATE libcarla::libcarla)

# コンパイルオプション
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(carla_sample PRIVATE -Wall -Wextra)
endif()
