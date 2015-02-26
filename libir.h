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
template <class T>
class iter
{
public:
  class impl;
  iter (impl *impl) : m_pimpl (impl) {}
  ~iter ();

  bool is_done () const;
  T operator*() const;
  void next ();

private:
  impl *m_pimpl;
};

typedef iter<cfg::block> block_iter;
typedef iter<stmt>  stmt_iter;

class function
{
public:
  class impl;
  function (impl *impl) : m_pimpl (impl) {}
  ~function ();

  cfg::graph get_cfg ();

private:
  impl *m_pimpl;
};

class cfg::graph
{
public:
  class impl;
  graph (impl *impl) : m_pimpl (impl) {}
  ~graph ();

  block_iter iter_blocks ();

private:
  impl *m_pimpl;
};

class cfg::block
{
public:
  class impl;
  block (impl *impl) : m_pimpl (impl) {}
  ~block ();

  stmt_iter iter_phis ();
  stmt_iter iter_stmts ();

private:
  impl *m_pimpl;
};

class cfg::edge
{
public:
  class impl;
  edge (impl *impl) : m_pimpl (impl) {}
  ~edge ();

  cfg::block get_src () const;
  cfg::block get_dest () const;

private:
  impl *m_pimpl;
};

class stmt
{
public:
  class impl;
  stmt (impl *impl) : m_pimpl (impl) {}
  ~stmt ();

private:
  impl *m_pimpl;
};

} // namespace ir

#endif // #ifndef INCLUDED_LIBIR_H
