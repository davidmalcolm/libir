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

#include "tree-pass.h"

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

#if (GCC_VERSION >= 5000)
typedef rtx_insn *rtx_insn_ptr;
#else
typedef rtx rtx_insn_ptr;
#endif

#if (GCC_VERSION >= 6000)
typedef gimple *gimple_stmt_ptr;
#else
typedef gimple gimple_stmt_ptr;
#endif

/* Concrete implementation classes. */

class gcc_function_impl : public libir_function
{
public:
  gcc_function_impl (struct function *inner);
  void unref ();
  libir_cfg *get_cfg ();

private:
  struct function *m_inner;
};

class gcc_cfg_impl : public libir_cfg
{
public:
  gcc_cfg_impl (struct control_flow_graph *inner);
  void unref ();
  libir_cfg_block_iter *iter_blocks ();

private:
  struct control_flow_graph *m_inner;
};

class gcc_block_impl : public libir_cfg_block
{
public:
  gcc_block_impl (basic_block inner);

  void unref ();
  libir_stmt_iter *iter_phis ();
  libir_stmt_iter *iter_stmts ();

private:
  basic_block m_inner;
};

class gimple_stmt_impl : public libir_stmt
{
public:
  gimple_stmt_impl (gimple_stmt_ptr inner);
  void unref ();

private:
  gimple_stmt_ptr m_inner;
};

class rtl_stmt_impl : public libir_stmt
{
public:
  rtl_stmt_impl (rtx inner);
  void unref ();

private:
  rtx m_inner;
};

class gcc_block_iter_impl : public libir_cfg_block_iter
{
public:
  gcc_block_iter_impl (struct control_flow_graph *inner);

  void unref ();
  bool is_done () const;
  libir_cfg_block *get_block () const;
  void next ();

private:
  void ensure_nonnull_block ();

  inline basic_block get_inner_bb () const {
    return GCC_COMPAT_VEC_INDEX (basic_block,
                                 m_inner->x_basic_block_info,
                                 m_index);
  }

private:
  struct control_flow_graph *m_inner;
  int m_index;
};

class gimple_iter_impl : public libir_stmt_iter
{
public:
  gimple_iter_impl (gimple_seq seq);

  void unref ();
  bool is_done () const;
  libir_stmt *get_stmt () const;
  void next ();

private:
  gimple_stmt_iterator m_gsi;
};

class rtl_iter_impl : public libir_stmt_iter
{
public:
  rtl_iter_impl (rtx_insn_ptr insn);

  void unref ();
  bool is_done () const;
  libir_stmt *get_stmt () const;
  void next ();

private:
  rtx m_insn;
};

class gcc_plugin_context : public libir_plugin_context
{
 public:
  void
  register_callback (const char *name,
                     void (*cb) (libir_pass *pass,
                                 libir_function *fn,
                                 void *user_data),
                     void *user_data);
};

class gcc_pass : public libir_pass
{
public:
  gcc_pass (opt_pass *impl): m_impl (impl) {}

  const char *get_name () const { return m_impl->name; }

private:
  opt_pass *m_impl;
};

/* Implementations */

/* class gcc_function_impl : public ir::function::impl.  */

gcc_function_impl::gcc_function_impl (struct function *inner)
  : m_inner (inner)
{
}

void
gcc_function_impl::unref ()
{
  delete this;
}

libir_cfg *
gcc_function_impl::get_cfg ()
{
  gcc_assert (m_inner->cfg);
  return new gcc_cfg_impl (m_inner->cfg);
}

/* class gcc_cfg_impl : public ir::cfg::graph::impl.  */

gcc_cfg_impl::gcc_cfg_impl (struct control_flow_graph *inner)
  : m_inner (inner)
{
}

void
gcc_cfg_impl::unref ()
{
  delete this;
}

libir_cfg_block_iter *
gcc_cfg_impl::iter_blocks ()
{
  gcc_assert (m_inner);
  return new gcc_block_iter_impl (m_inner);
}

/* class gcc_block_iter_impl : public libir_block_iter.  */

gcc_block_iter_impl::gcc_block_iter_impl (struct control_flow_graph *inner)
  : m_inner (inner),
    m_index (0)
{
  ensure_nonnull_block ();
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

libir_cfg_block *
gcc_block_iter_impl::get_block () const
{
  gcc_assert (!is_done ());

  basic_block bb = GCC_COMPAT_VEC_INDEX (basic_block,
                                         m_inner->x_basic_block_info,
                                         m_index);
  return new gcc_block_impl (bb);
}

void
gcc_block_iter_impl::next ()
{
  m_index++;
  ensure_nonnull_block ();
}

void
gcc_block_iter_impl::ensure_nonnull_block ()
{
  /* Skip past any leading NULLs within the table of basic blocks.  */
  while (get_inner_bb () == NULL && m_index < m_inner->x_n_basic_blocks)
    m_index++;
}

/* class gimple_stmt_impl : public libir_stmt.  */

gimple_stmt_impl::gimple_stmt_impl (gimple_stmt_ptr inner) :
  m_inner (inner)
{
}

void
gimple_stmt_impl::unref ()
{
  delete this;
}

/* class rtl_stmt_impl : public libir_stmt.  */

rtl_stmt_impl::rtl_stmt_impl (rtx inner) :
  m_inner (inner)
{
}

void
rtl_stmt_impl::unref ()
{
  delete this;
}

/* class gcc_block_impl : public libir_cfg_block.  */

gcc_block_impl::gcc_block_impl (basic_block inner) :
  m_inner (inner)
{
  gcc_assert (m_inner);
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

libir_stmt_iter *
gcc_block_impl::iter_phis ()
{
  struct gimple_bb_info *info;
  info = checked_get_gimple_info (m_inner);

  if (!info)
    return null_stmt_iter_impl::get();

  return new gimple_iter_impl (info->phi_nodes);
}

libir_stmt_iter *
gcc_block_impl::iter_stmts ()
{
  if (m_inner->flags & BB_RTL)
    {
      return new rtl_iter_impl (m_inner->il.x.head_);
    }
  else
    {
      struct gimple_bb_info *info;
      info = checked_get_gimple_info (m_inner);

      if (!info)
        return null_stmt_iter_impl::get();

      return new gimple_iter_impl (info->seq);
    }
}

/* class gimple_iter_impl : public libir_stmt_iter.  */

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

libir_stmt *
gimple_iter_impl::get_stmt () const
{
  gcc_assert (!is_done ());

  gimple_stmt_ptr stmt = gsi_stmt (m_gsi);
  return new gimple_stmt_impl (stmt);
}

void
gimple_iter_impl::next ()
{
  gsi_next (&m_gsi);
}

/* class rtl_iter_impl : public libir_stmt_iter.  */

rtl_iter_impl::rtl_iter_impl (rtx_insn_ptr insn) :
  m_insn (insn)
{
}

void
rtl_iter_impl::unref ()
{
  delete this;
}

bool
rtl_iter_impl::is_done () const
{
  return m_insn == NULL;
}

libir_stmt *
rtl_iter_impl::get_stmt () const
{
  gcc_assert (!is_done ());

  return new rtl_stmt_impl (m_insn);
}

void
rtl_iter_impl::next ()
{
  m_insn = NEXT_INSN (m_insn);
}

extern "C" {
  libir_function *
  cfun_as_ir_function ();
}

libir_function *
cfun_as_ir_function ()
{
  return new gcc_function_impl (cfun);
}

/* class gcc_plugin_context : public libir_plugin_context.  */

void
gcc_plugin_context::register_callback (const char *name,
                                       void (*cb) (libir_pass *pass,
                                                   libir_function *fn,
                                                   void *user_data),
                                       void *user_data)
{
  class callback {
  public:
    callback (void (*cb) (libir_pass *pass,
                          libir_function *fn,
                          void *user_data),
              void *user_data)
      : m_cb (cb),
        m_user_data (user_data)
    {}

    /* Implements gcc's plugin_callback_func.  */
    static void impl_for_gcc (void *gcc_data, void *user_data)
    {
      struct opt_pass *pass = (struct opt_pass *)gcc_data;
      libir_pass *ir_pass = new gcc_pass (pass);
      callback *self = reinterpret_cast <callback *> (user_data);
      libir_function *fn = cfun_as_ir_function ();
      self->m_cb (ir_pass, fn, self->m_user_data);
    }

  private:
    void (*m_cb) (libir_pass *pass,
                  libir_function *fn,
                  void *user_data);
    void *m_user_data;
  };

  ::register_callback (name,
                       PLUGIN_PASS_EXECUTION,
                       callback::impl_for_gcc,
                       new callback (cb, user_data));
}

extern "C" {
extern int
plugin_init (struct plugin_name_args *plugin_info,
             struct plugin_gcc_version *version) __attribute__((nonnull));
};

int
plugin_init (struct plugin_name_args *plugin_info,
             struct plugin_gcc_version *version)
{
  libir_plugin_context *ctxt = new class gcc_plugin_context ();
  init_libir_plugin (ctxt);
}
