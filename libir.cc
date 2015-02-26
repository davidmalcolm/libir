/*
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
#include "libir-impl.h"

#include <assert.h>

null_stmt_iter_impl null_stmt_iter_impl::s_singleton;

libir_cfg *
libir_function_get_cfg (libir_function *fn)
{
  return fn->get_cfg ();
}

void
libir_function_unref (libir_function *fn)
{
  fn->unref ();
}

void
libir_cfg_unref (libir_cfg *cfg)
{
  cfg->unref ();
}

void
libir_cfg_block_unref (libir_cfg_block *block)
{
  block->unref ();
}

void
libir_stmt_unref (libir_stmt *stmt)
{
  stmt->unref ();
}

libir_cfg_block_iter *
libir_cfg_iter_blocks (libir_cfg *cfg)
{
  return cfg->iter_blocks ();
}

extern int
libir_cfg_block_iter_is_done (libir_cfg_block_iter *bi)
{
  return bi->is_done ();
}

libir_cfg_block *
libir_cfg_block_iter_current (libir_cfg_block_iter *bi)
{
  return bi->get_block ();
}

void
libir_cfg_block_iter_next (libir_cfg_block_iter *bi)
{
  bi->next ();
}

void
libir_cfg_block_iter_unref (libir_cfg_block_iter *bi)
{
  bi->unref ();
}

/* libir_cfg_stmt_iter.  */

libir_stmt_iter *
libir_cfg_block_iter_phis (libir_cfg_block *block)
{
  return block->iter_phis ();
}

libir_stmt_iter *
libir_cfg_block_iter_stmts (libir_cfg_block *block)
{
  return block->iter_stmts ();
}

extern int
libir_stmt_iter_is_done (libir_stmt_iter *si)
{
  return si->is_done ();
}

libir_stmt *
libir_stmt_iter_current (libir_stmt_iter *si)
{
  libir_stmt *stmt;
  stmt = si->get_stmt ();
  assert (stmt);
  return stmt;
}

void
libir_stmt_iter_next (libir_stmt_iter *si)
{
  si->next ();
}

void
libir_stmt_iter_unref (libir_stmt_iter *si)
{
  si->unref ();
}

