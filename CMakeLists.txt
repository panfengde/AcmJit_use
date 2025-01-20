cmake_minimum_required(VERSION 3.23)
project(helloAsmJIT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O0")
set(CMAKE_C_FLAGS_RELEASE "-O0")

include_directories(include)
if (APPLE)
    link_directories("/Users/panfeng/coder/myProject/AsmJit/asmjit-master/cmake-build-release")
    add_executable(${PROJECT_NAME}_APP main_arm.cpp)
elseif (WIN32)
    link_directories("E:/code/asmJIT_learn/useAsmJIT/lib/")
    link_directories("E:/code/asmJIT_learn/asmjit-master/cmake-build-release/")
    add_executable(${PROJECT_NAME}_APP main_x86.cpp)
endif ()




target_link_libraries(${PROJECT_NAME}_APP asmjit)