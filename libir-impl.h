/* Private header declaring the internal abstract base classes.
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

/* Classes for implementing a libir backend.  */

#ifndef INCLUDED_LIBIR_IMPL_H
#define INCLUDED_LIBIR_IMPL_H

#include "libir.h"

/* Definitions of the opaque types declared in libir.h.
   They are all abstract C++ classes.  */

class libir_function
{
public:
  virtual void unref () = 0;
  virtual libir_cfg *get_cfg () = 0;
};

class libir_cfg
{
public:
  virtual void unref () = 0;
  virtual libir_cfg_block_iter *iter_blocks () = 0;

};

class libir_cfg_block
{
public:
  virtual void unref () = 0;
  virtual libir_stmt_iter *iter_phis () = 0;
  virtual libir_stmt_iter *iter_stmts () = 0;
};

class libir_cfg_edge
{
public:
  virtual void unref () = 0;
};

class libir_stmt
{
public:
  virtual void unref () = 0;
};

class libir_cfg_block_iter
{
public:
  virtual void unref () = 0;
  virtual bool is_done () const = 0;
  virtual libir_cfg_block *get_block () const = 0;
  virtual void next () = 0;
};

class libir_stmt_iter
{
public:
  virtual void unref () = 0;
  virtual bool is_done () const = 0;
  virtual libir_stmt *get_stmt () const = 0;
  virtual void next () = 0;
};

class libir_plugin_context
{
 public:
  virtual void
  register_callback (const char *name,
                     void (*cb) (libir_pass *pass,
                                 libir_function *fn,
                                 void *user_data),
                     void *user_data) = 0;
};

class libir_pass
{
 public:
  virtual const char *get_name () const = 0;
};

/* FIXME.  */
class null_stmt_iter_impl : public libir_stmt_iter
{
public:
  static null_stmt_iter_impl *get() {return &s_singleton; }

  void unref () { /* empty; a singleton.  */ }
  bool is_done () const { return true; }
  libir_stmt *get_stmt () const { return 0; }
  void next () { /* empty */ }

private:
  static null_stmt_iter_impl s_singleton;
};

#endif // #ifndef INCLUDED_LIBIR_IMPL_H
