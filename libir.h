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

#ifndef INCLUDED_LIBIR_H
#define INCLUDED_LIBIR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Forward declarations of types.  */
typedef struct libir_function libir_function;
typedef struct libir_cfg libir_cfg;
typedef struct libir_cfg_block libir_cfg_block;
typedef struct libir_cfg_edge libir_cfg_edge;
typedef struct libir_stmt libir_stmt;

typedef struct libir_cfg_block_iter libir_cfg_block_iter;
typedef struct libir_stmt_iter libir_stmt_iter;

/* Iterators typically have this API:

   extern int
   libir_FOO_iter_is_done (libir_FOO_iter *iter);

   extern libir_FOO *
   libir_FOO_iter_current (libir_FOO_iter *iter);

   extern void
   libir_FOO_iter_next (libir_FOO_iter *iter);

   extern void
   libir_FOO_iter_unref (libir_FOO_iter *iter);

*/

/* libir_function.  */

extern libir_cfg *
libir_function_get_cfg (libir_function *fn);

extern void
libir_function_unref (libir_function *fn);

/* libir_cfg.  */

extern void
libir_cfg_unref (libir_cfg *cfg);

/* libir_cfg_block.  */

extern void
libir_cfg_block_unref (libir_cfg_block *block);

/* libir_stmt.  */

extern void
libir_stmt_unref (libir_stmt *stmt);

/* libir_cfg_block_iter.  */

extern libir_cfg_block_iter *
libir_cfg_iter_blocks (libir_cfg *cfg);

extern int
libir_cfg_block_iter_is_done (libir_cfg_block_iter *bi);

extern libir_cfg_block *
libir_cfg_block_iter_current (libir_cfg_block_iter *bi);

extern void
libir_cfg_block_iter_next (libir_cfg_block_iter *bi);

extern void
libir_cfg_block_iter_unref (libir_cfg_block_iter *bi);

/* libir_cfg_stmt_iter.  */

extern libir_stmt_iter *
libir_cfg_block_iter_phis (libir_cfg_block *block);

extern libir_stmt_iter *
libir_cfg_block_iter_stmts (libir_cfg_block *block);

extern int
libir_stmt_iter_is_done (libir_stmt_iter *si);

extern libir_stmt *
libir_stmt_iter_current (libir_stmt_iter *si);

extern void
libir_stmt_iter_next (libir_stmt_iter *si);

extern void
libir_stmt_iter_unref (libir_stmt_iter *si);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // #ifndef INCLUDED_LIBIR_H
