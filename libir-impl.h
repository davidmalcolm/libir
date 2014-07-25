/*
   Copyright 2014 David Malcolm <dmalcolm@redhat.com>
   Copyright 2014 Red Hat, Inc.

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

namespace ir
{

class function::impl
{
public:
  virtual void unref () = 0;
  virtual cfg::graph get_cfg () = 0;
};

class cfg::graph::impl
{
public:
  virtual void unref () = 0;
  virtual block_iter iter_blocks () = 0;

};

class cfg::block::impl
{
public:
  virtual void unref () = 0;
  virtual stmt_iter iter_phis () = 0;
  virtual stmt_iter iter_stmts () = 0;
};

class cfg::edge::impl
{
public:
  virtual void unref () = 0;
};

class stmt::impl
{
public:
  virtual void unref () = 0;
};

template<>
class block_iter::impl
{
public:
  virtual void unref () = 0;
  virtual bool is_done () const = 0;
  virtual cfg::block get_block () const = 0;
  virtual void next () = 0;
};

template<>
class stmt_iter::impl
{
public:
  virtual void unref () = 0;
  virtual bool is_done () const = 0;
  virtual stmt get_stmt () const = 0;
  virtual void next () = 0;
};

} // namespace ir

#endif // #ifndef INCLUDED_LIBIR_IMPL_H
