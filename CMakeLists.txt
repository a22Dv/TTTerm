cmake_minimum_required(VERSION 3.15)
project(termichess CXX)
set(FILES
    src/main.cpp
)
add_executable(termichess ${FILES})
target_compile_features(termichess PRIVATE cxx_std_20)
