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

namespace ir {

// ir::block_iter
template<>
block_iter::~iter ()
{
  m_pimpl->unref ();
}

template<>
bool
block_iter::is_done () const
{
  return m_pimpl->is_done ();
}

template<>
cfg::block
block_iter::operator*() const
{
  return m_pimpl->get_block ();
}

template<>
void
block_iter::next ()
{
  return m_pimpl->next ();
}

// ir::stmt_iter
template<>
stmt_iter::~iter ()
{
  m_pimpl->unref ();
}

template<>
bool
stmt_iter::is_done () const
{
  return m_pimpl->is_done ();
}

template<>
stmt
stmt_iter::operator*() const
{
  return m_pimpl->get_stmt ();
}

template<>
void
stmt_iter::next ()
{
  return m_pimpl->next ();
}

// ir::function

function::~function ()
{
  m_pimpl->unref ();
}

cfg::graph
function::get_cfg ()
{
  return m_pimpl->get_cfg ();
}

// class cfg::graph

cfg::graph::~graph ()
{
  m_pimpl->unref ();
}

block_iter
cfg::graph::iter_blocks ()
{
  return m_pimpl->iter_blocks ();
}

// class cfg::block
cfg::block::~block ()
{
  m_pimpl->unref ();
}

stmt_iter
cfg::block::iter_phis ()
{
  return m_pimpl->iter_phis ();
}

stmt_iter
cfg::block::iter_stmts ()
{
  return m_pimpl->iter_stmts ();
}

// class stmt
stmt::~stmt ()
{
  m_pimpl->unref ();
}

null_stmt_iter_impl null_stmt_iter_impl::s_singleton;

} // namespace ir
