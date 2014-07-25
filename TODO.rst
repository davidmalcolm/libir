libir-gcc-plugin.cc:

* Integrate with ggc: keep a list of all live wrapper objects; register a
  traversal hook, and mark them when it fires.

