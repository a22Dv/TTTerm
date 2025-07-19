cmake_minimum_required(VERSION 3.15)
project(termichess CXX)
set(FILES
    src/main.cpp
)
add_executable(ttterm ${FILES})
target_compile_features(ttterm PRIVATE cxx_std_20)
