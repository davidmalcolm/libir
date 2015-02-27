/* Example plugin implemented in C using libir.h
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

#include "libir.h"
#include <stdio.h>

/* This can be written in C, purely using libir.h, without needing
   GCC headers: */

void
foo_test (libir_function *fn)
{
  int num_blocks = 0;
  int num_phis = 0;
  int num_stmts = 0;

  libir_cfg *cfg;
  libir_cfg_block_iter *bi;

  printf ("within foo_test\n");

  cfg = libir_function_get_cfg (fn);
  for (bi = libir_cfg_iter_blocks (cfg);
       !libir_cfg_block_iter_is_done (bi);
       libir_cfg_block_iter_next (bi))
    {
      libir_cfg_block *b = libir_cfg_block_iter_current (bi);
      libir_stmt_iter *si;

      num_blocks++;

      for (si = libir_cfg_block_iter_phis (b);
           !libir_stmt_iter_is_done (si);
           libir_stmt_iter_next (si))
        {
          libir_stmt *stmt = libir_stmt_iter_current (si);
          num_phis++;
          libir_stmt_unref (stmt);
        }
      libir_stmt_iter_unref (si);

      for (si = libir_cfg_block_iter_stmts (b);
           !libir_stmt_iter_is_done (si);
           libir_stmt_iter_next (si))
        {
          libir_stmt *stmt = libir_stmt_iter_current (si);
          num_stmts++;
          libir_stmt_unref (stmt);
        }
      libir_stmt_iter_unref (si);

      libir_cfg_block_unref (b);
    }
  libir_cfg_block_iter_unref (bi);
  libir_cfg_unref (cfg);

  printf ("num blocks: %i; num phis: %i; num stmts: %i\n",
          num_blocks, num_phis, num_stmts);
}

/* FIXME: the rest of this needs to be compiled as C++.  */
/* FIXME: move the rest of this to a support library */
#include <gcc-plugin.h>
#include "tree-pass.h"
#include "function.h"

extern "C" {

extern libir_function *
cfun_as_ir_function ();

int plugin_is_GPL_compatible;

extern int
plugin_init (struct plugin_name_args *plugin_info,
             struct plugin_gcc_version *version) __attribute__((nonnull));
}; // extern "C"


static void
callback (void *gcc_data, void *user_data)
{
    struct opt_pass *pass = (struct opt_pass *)gcc_data;
    printf ("callback called: %s\n", pass->name);

    if (0==strcmp (pass->name, "veclower")
        || 0==strcmp (pass->name, "*rest_of_compilation"))
      {
        printf ("cfun: %p\n", cfun);

        foo_test (cfun_as_ir_function ());
      }
}

int
plugin_init (struct plugin_name_args *plugin_info,
             struct plugin_gcc_version *version)
{
  printf ("got here\n");

  register_callback ("test-plugin",
                     PLUGIN_PASS_EXECUTION,
                     callback,
                     NULL);

  return 0;
}
