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
#include <string.h>

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

int plugin_is_GPL_compatible;

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
