/* Example plugin implemented in C++ using libir++.h bindings to libir.h
   Copyright 2014-2015 David Malcolm <dmalcolm@redhat.com>
   Copyright 2014-2015 Red Hat, Inc.

   This is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see
   <http://www.gnu.org/licenses/>.
*/

#include "libir++.h"
#include <stdio.h>
#include <string.h>

/* This can be written purely using libir, without needing GCC headers: */

void
foo_test (ir::function fn)
{
  printf ("within foo_test\n");
  int num_blocks = 0;
  int num_phis = 0;
  int num_stmts = 0;

  ir::cfg::graph cfg = fn.get_cfg ();
  for (ir::block_iter bi = cfg.iter_blocks (); !bi.is_done (); bi.next ())
    {
      ir::cfg::block b = *bi;
      num_blocks++;

      for (ir::stmt_iter si = b.iter_phis (); !si.is_done (); si.next ())
        {
          ir::stmt phi = *si;
          num_phis++;
        }

      for (ir::stmt_iter si = b.iter_stmts (); !si.is_done (); si.next ())
        {
          ir::stmt s = *si;
          num_stmts++;
        }
    }

  printf ("num blocks: %i; num phis: %i; num stmts: %i\n",
          num_blocks, num_phis, num_stmts);
}

extern "C" {
int plugin_is_GPL_compatible;
}

static void
callback (libir_pass *pass,
          libir_function *fn,
          void *user_data)
{
  const char *pass_name = libir_pass_get_name (pass);
  printf ("callback called: %s\n", pass_name);

  if (0==strcmp (pass_name, "veclower")
      || 0==strcmp (pass_name, "*rest_of_compilation"))
    {
      foo_test (fn);
    }
}

int
init_libir_plugin (libir_plugin_context *ctxt)
{
  printf ("got here\n");

  libir_plugin_context_register_callback (ctxt,
                                          "test-plugin",
                                          callback,
                                          NULL);

  return 0;
}
