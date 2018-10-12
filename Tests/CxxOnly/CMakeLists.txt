# a simple CXX only test case
project (CxxOnly CXX)

set(CMAKE_DEBUG_POSTFIX "_test_debug_postfix")
if(WIN32)
  set(EXTRA_SRCS test.CPP)
endif()

add_executable (CxxOnly cxxonly.cxx)

add_library(testcxx1.my STATIC libcxx1.cxx ${EXTRA_SRCS})
add_library(testcxx2 SHARED libcxx2.cxx)
add_library(testCxxModuleLibrary MODULE testCxxModuleLibrary.cxx)

target_link_libraries(CxxOnly testcxx1.my testcxx2)

if(NOT CMAKE_CXX_MODULES_AVAILABLE)
  message(WARNING "Skipping test of C++ Modules TS features. To test, use a "
                  "compiler that supports it and one CMake knows how to call.")
else()
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang$")
    add_cxx_module(testCxxModuleTs)
    target_compile_definitions(CxxOnly PRIVATE _MODULES_TS)
    target_link_libraries(CxxOnly PRIVATE testCxxModuleTs)
  else()
    message(WARNING "Skipping test of C++ Modules TS as the properly prepared "
                    "test file for compiler ${CMAKE_CXX_COMPILER_ID} is not "
                    "known.")
  endif()
endif()
