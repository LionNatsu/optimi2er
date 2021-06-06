cmake_minimum_required(VERSION 3.12)
project(optimi2er)

set(CMAKE_CXX_STANDARD 14)

# cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/home/lion/.local/ -DLLVM_TARGETS_TO_BUILD=X86 -DLLVM_BUILD_LLVM_DYLIB=ON -DLLVM_LINK_LLVM_DYLIB=ON ..

option(LLVM_DIR     "Where is ...lib/cmake/llvm")
option(LLVM_SHARED  "Use libLLVM.so instead of static components" OFF)

find_package(LLVM REQUIRED CONFIG)
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_executable(optimi2er main.cpp)
target_include_directories(optimi2er PRIVATE ${LLVM_INCLUDE_DIRS})

if(${LLVM_SHARED})
    set(llvm_libs "LLVM")
else()
    llvm_map_components_to_libnames(llvm_libs core)
endif()

target_link_libraries(optimi2er ${llvm_libs})
