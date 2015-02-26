/* C++ bindings for libir.h.
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

#ifndef INCLUDED_LIBIRPLUSPLUS_H
#define INCLUDED_LIBIRPLUSPLUS_H

#include "libir.h"
#include <stdlib.h>

/* Forward declarations of types.  */
namespace ir
{
  class function;
  namespace cfg
  {
    class graph;
    class block;
    class edge;
  };
  class stmt;
} // namespace ir

namespace ir
{
template <class T_element, class T_impl>
class iter
{
public:
  iter (T_impl *impl) : m_impl (impl) {}
  ~iter ();

  bool is_done () const;
  T_element operator*() const;
  void next ();

private:
  T_impl *m_impl;
};

typedef iter<cfg::block, ::libir_cfg_block_iter> block_iter;
typedef iter<stmt, ::libir_stmt_iter>  stmt_iter;

class function
{
public:
  typedef ::libir_function impl;

  function (impl *impl_) : m_impl (impl_) {}
  ~function () { libir_function_unref (m_impl); }

  inline cfg::graph get_cfg ();

private:
  impl *m_impl;
};

class cfg::graph
{
 public:
  typedef ::libir_cfg impl;

  graph (impl *impl) : m_impl (impl) {}
  ~graph () { libir_cfg_unref (m_impl); }

  inline block_iter iter_blocks ();

private:
  impl *m_impl;
};

class cfg::block
{
public:
  typedef ::libir_cfg_block impl;

  block (impl *impl) : m_impl (impl) {}
  ~block () { libir_cfg_block_unref (m_impl); }

  inline stmt_iter iter_phis ();
  inline stmt_iter iter_stmts ();

private:
  impl *m_impl;
};

#if 0
class cfg::edge
{
public:
  typedef ::libir_cfg_edge impl;

  edge (impl *impl) : m_impl (impl) {}
  ~edge ();

  cfg::block get_src () const;
  cfg::block get_dest () const;

private:
  impl *m_impl;
};
#endif

class stmt
{
public:
  typedef ::libir_stmt impl;

  stmt (impl *impl) : m_impl (impl) {}
  ~stmt () { libir_stmt_unref (m_impl); }

private:
  impl *m_impl;
};

template <>
block_iter::~block_iter ()
{
  libir_cfg_block_iter_unref (m_impl);
}

template <>
bool
block_iter::is_done () const
{
  return libir_cfg_block_iter_is_done (m_impl);
}

template <>
cfg::block
block_iter::operator* () const
{
  return cfg::block (libir_cfg_block_iter_current (m_impl));
}

template <>
void
block_iter::next ()
{
  libir_cfg_block_iter_next (m_impl);
}

template <>
stmt_iter::~stmt_iter ()
{
  libir_stmt_iter_unref (m_impl);
}

template <>
bool
stmt_iter::is_done () const
{
  return libir_stmt_iter_is_done (m_impl);
}

template <>
stmt
stmt_iter::operator* () const
{
  return stmt (libir_stmt_iter_current (m_impl));
}

template <>
void
stmt_iter::next ()
{
  libir_stmt_iter_next (m_impl);
}

inline cfg::graph
function::get_cfg ()
{
  return cfg::graph (libir_function_get_cfg (m_impl));
}

inline block_iter
cfg::graph::iter_blocks ()
{
  return block_iter (libir_cfg_iter_blocks (m_impl));
}

inline stmt_iter
cfg::block::iter_phis ()
{
  return stmt_iter (libir_cfg_block_iter_phis (m_impl));
}

inline stmt_iter
cfg::block::iter_stmts ()
{
  return stmt_iter (libir_cfg_block_iter_stmts (m_impl));
}

} // namespace ir

#endif // #ifndef INCLUDED_LIBIRPLUSPLUS_H
