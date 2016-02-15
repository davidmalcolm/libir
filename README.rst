ASCII art diagram::

  INTERFACE                         IMPLEMENTATION

  +-----------------------+
  |libir++.h              |
  |                       |
  |C++ bindings to libir.h|
  +-----------------------+
              |
  +-----------------------+         +-------------------------+
  |libir.h                |         |libir-impl.h             |
  |                       |         |                         |
  |Pure C API             |---------|C++ abstract base classes|
  |Everything is opaque   |         |for implementing an IR   |
  |Uses no GCC headers    |         |"provider"               |
  +-----------------------+         +-------------------------+
                                                 |
                                                 |--------------------------+
                                                 |                          |
                                    +-------------------------+  +-------------------------+
                                    |libir-gcc-plugin.cc      |  |Other IR providers       |
                                    |                         |  |                         |
                                    |Implementation for GCC   |  |e.g. libgccjit           |
                                    |plugins                  |  +-------------------------+
                                    +-------------------------+


Linkage model
=============

::
                                          +---------------------------+
  +-------------------------+             |libir-gcc-plugin.so        |
  |cc1, cc1plus, etc        |             |                           |
  |                         |             | +-----------------------+ |
  |   dlopen "foo-plugin.so |             | |libir-gcc-plugin.cc    | |
  |   |                     |             | |                       | |
  |   | dlsym "init_plugin"-----------------> "init_plugin"         | |
  |   |                     |             | +----------|------------+ |
  +---|---------------------+             |            | calls        |
      |                                   +------------|--------+-----+
      |                                                |        | linked
      |                     dlopen    +----------------|--------+-+
      +------------------------------>| foo-plugin.so  V          |
                                      |     "init_libir_plugin"   |
                                      +---------------------------+
