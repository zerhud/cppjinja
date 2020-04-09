/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "impl.hpp"
#include "evtree.hpp"
#include "context.hpp"
#include "callstack.hpp"

#include "helpers/binary_op.hpp"
#include "exenv/expr_solver.hpp"
#include "exenv/expr_filter.hpp"

cppjinja::evt::exenv_impl::exenv_impl(
          const cppjinja::data_provider* prov
        , const cppjinja::evtree* tree
        , std::ostream& out
        )
    : compiled_template(tree)
    , user_data(prov)
    , ostream(out)
{
}

const cppjinja::evtree& cppjinja::evt::exenv_impl::tmpl() const
{
	assert(compiled_template);
	return *compiled_template;
}

const cppjinja::data_provider* cppjinja::evt::exenv_impl::data() const
{
	assert(user_data);
	return user_data;
}

std::ostream& cppjinja::evt::exenv_impl::out()
{
	return ostream;
}

cppjinja::evt::context& cppjinja::evt::exenv_impl::ctx()
{
	return exectx;
}

const cppjinja::evt::context& cppjinja::evt::exenv_impl::ctx() const
{
	return exectx;
}

cppjinja::evt::callstack& cppjinja::evt::exenv_impl::calls()
{
	return execalls;
}

const cppjinja::evt::callstack& cppjinja::evt::exenv_impl::calls() const
{
	return execalls;
}

std::optional<cppjinja::evt::render_info> cppjinja::evt::exenv_impl::rinfo() const
{
	return cur_rinfo;
}

void cppjinja::evt::exenv_impl::rinfo(std::optional<cppjinja::evt::render_info> ri)
{
	cur_rinfo = ri;
}
