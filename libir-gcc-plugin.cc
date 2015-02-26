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

#include <gcc-plugin.h>
#include "tree.h"
#include "function.h"
#include "basic-block.h"

#if (GCC_VERSION >= 4009)
#include "tree-ssa-alias.h" /* needed by gimple.h in 4.9 */
#include "internal-fn.h" /* needed by gimple.h in 4.9 */
#include "is-a.h" /* needed by gimple.h in 4.9 */
#include "predict.h" /* needed by gimple.h in 4.9 */
#include "gimple-expr.h" /* needed by gimple.h in 4.9 */
#endif
#include "gimple.h"

/* gcc 4.9 moved gimple_stmt_iterator into this header */
#if (GCC_VERSION >= 4009)
#include "gimple-iterator.h"
#endif

#include "params.h"
#include "tree.h"
#include "diagnostic.h"
#include "cgraph.h"
#include "opts.h"
#include "rtl.h"

#include "libir-impl.h"

/* Getting the length of a vector, returning 0 if it is NULL: */
#if (GCC_VERSION >= 4008)
  #define GCC_COMPAT_VEC_LENGTH(KIND, V) \
    ( (V) ? ( (V)->length() ) : 0 )
#else
  #define GCC_COMPAT_VEC_LENGTH(KIND, V) \
    ( VEC_length(KIND, (V)) )
#endif

/* Looking up an element by index: */
#if (GCC_VERSION >= 4008)
  #define GCC_COMPAT_VEC_INDEX(KIND, V, IDX) \
    ( (*(V))[IDX] )
#else
  #define GCC_COMPAT_VEC_INDEX(KIND, V, IDX) \
    ( VEC_index(KIND, (V), (IDX) ) )
#endif

/* Concrete implementation classes. */

class gcc_function_impl : public ir::function::impl
{
public:
  gcc_function_impl (struct function *inner);
  void unref ();
  ir::cfg::graph get_cfg ();

private:
  struct function *m_inner;
};

class gcc_cfg_impl : public ir::cfg::graph::impl
{
public:
  gcc_cfg_impl (struct control_flow_graph *inner);
  void unref ();
  ir::block_iter iter_blocks ();

private:
  struct control_flow_graph *m_inner;
};

class gcc_block_impl : public ir::cfg::block::impl
{
public:
  gcc_block_impl (basic_block inner);

  void unref ();
  ir::stmt_iter iter_phis ();
  ir::stmt_iter iter_stmts ();

private:
  basic_block m_inner;
};

class gimple_stmt_impl : public ir::stmt::impl
{
public:
  gimple_stmt_impl (gimple inner);
  void unref ();

private:
  gimple m_inner;
};

class gcc_block_iter_impl : public ir::block_iter::impl
{
public:
  gcc_block_iter_impl (struct control_flow_graph *inner);

  void unref ();
  bool is_done () const;
  ir::cfg::block get_block () const;
  void next ();

private:
  struct control_flow_graph *m_inner;
  int m_index;
};

class gimple_iter_impl : public ir::stmt_iter::impl
{
public:
  gimple_iter_impl (gimple_seq seq);

  void unref ();
  bool is_done () const;
  ir::stmt get_stmt () const;
  void next ();

private:
  /* FIXME: gimple vs RTL */
  gimple_stmt_iterator m_gsi;
};

/* Implementations */

// class gcc_function_impl : public ir::function::impl
gcc_function_impl::gcc_function_impl (struct function *inner)
  : m_inner (inner)
{
}

void
gcc_function_impl::unref ()
{
  delete this;
}

ir::cfg::graph
gcc_function_impl::get_cfg ()
{
  gcc_assert (m_inner->cfg);
  return ir::cfg::graph (new gcc_cfg_impl (m_inner->cfg));
}

// class gcc_cfg_impl : public ir::cfg::graph::impl
gcc_cfg_impl::gcc_cfg_impl (struct control_flow_graph *inner)
  : m_inner (inner)
{
}

void
gcc_cfg_impl::unref ()
{
  delete this;
}

ir::block_iter
gcc_cfg_impl::iter_blocks ()
{
  gcc_assert (m_inner);
  return ir::block_iter (new gcc_block_iter_impl (m_inner));
}

// class gcc_block_iter_impl : public ir::block_iter::impl

gcc_block_iter_impl::gcc_block_iter_impl (struct control_flow_graph *inner)
  : m_inner (inner),
    m_index (0)
{
}

void
gcc_block_iter_impl::unref ()
{
  delete this;
}

bool
gcc_block_iter_impl::is_done () const
{
  return m_index >= m_inner->x_n_basic_blocks;
}

ir::cfg::block
gcc_block_iter_impl::get_block () const
{
  basic_block bb = GCC_COMPAT_VEC_INDEX (basic_block,
                                         m_inner->x_basic_block_info,
                                         m_index);
  return ir::cfg::block (new gcc_block_impl (bb));
}

void
gcc_block_iter_impl::next ()
{
  m_index++;
}

// class gimple_stmt_impl : public ir::stmt::impl

gimple_stmt_impl::gimple_stmt_impl (gimple inner) :
  m_inner (inner)
{
}

void
gimple_stmt_impl::unref ()
{
  delete this;
}

// class gcc_block_impl : public ir::cfg::block::impl

gcc_block_impl::gcc_block_impl (basic_block inner) :
  m_inner (inner)
{
}

void
gcc_block_impl::unref ()
{
  delete this;
}

/* In GCC 4.7, struct basic_block_def had a
     struct gimple_bb_info * gimple;
   within its il union.

   In GCC 4.8, this became:
     struct gimple_bb_info gimple
   i.e. it is no longer dereferenced
*/
static struct gimple_bb_info *
checked_get_gimple_info (basic_block bb)
{
  if (bb->flags & BB_RTL)
    {
      return NULL;
    }

#if (GCC_VERSION >= 4008)
  return &bb->il.gimple;
#else
  return bb->il.gimple;
#endif
}

ir::stmt_iter
gcc_block_impl::iter_phis ()
{
  struct gimple_bb_info *info;
  info = checked_get_gimple_info (m_inner);

  if (!info)
    return ir::stmt_iter (ir::null_stmt_iter_impl::get());

  return ir::stmt_iter (new gimple_iter_impl (info->phi_nodes));
}

ir::stmt_iter
gcc_block_impl::iter_stmts ()
{
  struct gimple_bb_info *info;
  info = checked_get_gimple_info (m_inner);

  if (!info)
    return ir::stmt_iter (ir::null_stmt_iter_impl::get());

  return ir::stmt_iter (new gimple_iter_impl (info->seq));
}

// class gimple_iter_impl : public ir::stmt_iter::impl

gimple_iter_impl::gimple_iter_impl (gimple_seq seq)
{
  m_gsi = gsi_start (seq);
}

void
gimple_iter_impl::unref ()
{
  delete this;
}

bool
gimple_iter_impl::is_done () const
{
  return gsi_end_p (m_gsi);
}

ir::stmt
gimple_iter_impl::get_stmt () const
{
  gimple stmt = gsi_stmt (m_gsi);
  return ir::stmt (new gimple_stmt_impl (stmt));
}

void
gimple_iter_impl::next ()
{
  gsi_next (&m_gsi);
}


ir::function
cfun_as_ir_function ()
{
  return ir::function (new gcc_function_impl (cfun));
}
