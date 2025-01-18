cmake_minimum_required(VERSION 3.23)
project(helloAsmJIT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O0")
set(CMAKE_C_FLAGS_RELEASE "-O0")

include_directories(include)

link_directories("E:/code/asmJIT_learn/useAsmJIT/lib/")
link_directories("E:/code/asmJIT_learn/asmjit-master/cmake-build-release/")

add_executable(${PROJECT_NAME}_APP main.cpp)

target_link_libraries(${PROJECT_NAME}_APP asmjit)