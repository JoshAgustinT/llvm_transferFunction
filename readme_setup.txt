Random scribbled notes... not meant to be coherent:

Setting up LLVM in c++:

We want to use llvm/IR/IRBuilder.h
llvm/IR/LLVMContext.h
llvm/IR/Module.h
as #include "xxx" in our c++ file.

To do this:
-- ok sorta confusion since I already installed llvm and it's automatically findable for some reason... must be the make file of compiling and setting up llvm from source code places it in the right directory... or maybe i moved it? not sure... 

either way, copying CMakeLists.txt from the website john gave us did the trick. of course I had to change my source file to bf.cpp and changed the executable output to a.out, it also completely redid my makefile...

here's the code:

```
cmake_minimum_required(VERSION 3.20.0)
project(SimpleProject)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Now build our tools
add_executable(a.out bf.cpp)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

# Link against LLVM libraries
target_link_libraries(a.out ${llvm_libs})
```

After which I could use it as normal after making it ie:
 2011  cmake .
 2012  make
 2013  ./a.out  benches/hanoi.b
 2014  gcc bf.s 
 2015  ./a.out

 To rebuild:
 cmake .
 make

This is with :
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

included in the file, which wasn't working prior of course.

While printing llvm to stdo, to build the llvm:
./a.out  benches/hanoi.b > test.ll
llc test.ll -o output.s
gcc output.s -o first_llvm 
./first_llvm

full example with rebuild:
 2083  cmake .
   make
   ./a.out  simple.b > test.ll
      ./a.out  check.b > test.ll
./a.out benches/hello.b >test.ll
./a.out benches/twinkle.b >test.ll

   llc test.ll -o output.s
   gcc output.s -o first_llvm 
   ./first_llvm

 Old compilation process:
 g++ -O3 bf.cpp
	./a.out benches/hanoi.b -O1
	@echo "linking.."
	gcc bf.s 
	time ./a.out