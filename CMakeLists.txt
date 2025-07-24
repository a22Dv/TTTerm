cmake_minimum_required(VERSION 3.15)
project(ttterm CXX)

set (
    SRC_FILES
    src/main.cpp
    src/tctassets.cpp
    src/tctaudio.cpp
    src/tctdisplay.cpp
)
add_executable(ttterm)
target_sources(ttterm PRIVATE ${SRC_FILES})
target_compile_features(ttterm PRIVATE cxx_std_20)

 # Explicitly enable exception handling. Required by clangd.
target_compile_options(ttterm PRIVATE /EHsc)
target_include_directories(ttterm PRIVATE "${CMAKE_SOURCE_DIR}/include")
