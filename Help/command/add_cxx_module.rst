add_cxx_module
--------------

Add the compilation of a C++ Modules TS module to the build and sets it up as
a target for linking other targets against it.

.. warning:: `add_cxx_module` is an **experimental** feature!

.. note:: Using `add_cxx_module` requires a C++ compiler (set at
   ``CMAKE_CXX_COMPILER``) that contains an experimental implementation for
   this feature. If your compiler can't compile modules, or CMake does not
   know how to configure your current compiler, an error will be emitted and
   generation will stop.

Using C++ Modules TS have two steps:

 1. The module must be compiled as a module using `add_cxx_module`.
 2. Binaries that import this module must be linked against the module.

Creating module compilations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

  add_cxx_module(modulename [source])

This command will define a target named ``modulename``, which is compiled using
``CMAKE_CXX_COMPILER``. If ``source`` is not specified, the source file
``modulename`` + ``CMAKE_CXX_MODULE_SOURCE_EXTENSION``, e.g.
``modulename.cppm``.

C++ Module TS modules are fully-fledged translations which might have other
libraries they depend on. These libraries can and should be added to the
linked library list of **the module**. CMake will propagate this information
towards the binary, as explained in :command:`target_link_libraries`.

.. code-block:: cmake

  add_cxx_module(my_math_module)
  target_link_libraries(my_math_module PUBLIC m)

Linking the module implementation to the binary
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

`add_cxx_module` sets up ``modulename`` as a target, which can be used as a
library in :command:`target_link_libraries`.

::

  target_link_libraries(binary [...] PRIVATE modulename)

Example
^^^^^^^

In the following example, a module's implemented function is used in an
executable.

.. code-block:: cpp

  // In e.g. mymodule.cppm:
  #include <cmath>

  export module mymodule;
  export int foo() { return 2; }
  export double my_sqrt(double d) { return sqrt(d); }

.. code-block:: cpp

  // In main.cpp:
  #include <iostream>
  import mymodule;

  int main() {
    std::cout << foo() << std::endl;
    return my_sqrt(25);
  }

The following configuration can be used to compile this project.

.. code-block:: cmake

  project(Example)
  add_cxx_module(mymodule)
  target_link_libraries(mymodule m)

  add_executable(main main)
  target_link_libraries(main PRIVATE mymodule)
