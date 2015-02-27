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
