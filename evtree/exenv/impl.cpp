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

std::ostream& cppjinja::evt::operator << (std::ostream& out, const render_info& ri)
{
	out << '{' << ri.trim_left << ',' << ri.trim_right << '}';
	return out;
}

bool cppjinja::evt::operator == (const render_info& left, const render_info& right)
{
	return std::tie(left.trim_left, left.trim_right)
	        == std::tie(right.trim_left, right.trim_right);
}

cppjinja::evt::exenv_impl::exenv_impl(
          const cppjinja::data_provider* prov
        , const cppjinja::evtree* tree
        )
    : compiled_template(tree)
    , given_data(prov)
{
}

const cppjinja::evtree& cppjinja::evt::exenv_impl::tmpl() const
{
	assert(compiled_template);
	return *compiled_template;
}

std::vector<const cppjinja::evt::node*>
cppjinja::evt::exenv_impl::children(const cppjinja::evt::node* selected) const
{
	assert(compiled_template);
	return compiled_template->children(selected);
}

std::vector<const cppjinja::evtnodes::callable*>
cppjinja::evt::exenv_impl::roots(const cppjinja::evtnodes::tmpl* tmpl) const
{
	return compiled_template->roots(tmpl);
}

const cppjinja::data_provider* cppjinja::evt::exenv_impl::data() const
{
	return given_data.data();
}

std::ostream& cppjinja::evt::exenv_impl::out()
{
	return ctx().out();
}

cppjinja::east::string_t cppjinja::evt::exenv_impl::result() const
{
	return rfmt(ctx().result());
}

cppjinja::evt::context& cppjinja::evt::exenv_impl::ctx()
{
	return exectx;
}

const cppjinja::evt::context& cppjinja::evt::exenv_impl::ctx() const
{
	return exectx;
}

void cppjinja::evt::exenv_impl::current_node(const cppjinja::evt::node* n)
{
	ctx().current_node(n);
}

cppjinja::evt::context_object& cppjinja::evt::exenv_impl::locals()
{
	return ctx().cur_namespace();
}

cppjinja::evt::context_object& cppjinja::evt::exenv_impl::globals()
{
	return global_namespace;
}

const cppjinja::evt::context_objects::queue cppjinja::evt::exenv_impl::params() const
{
	return execalls.current_params(exectx.maker());
}

const cppjinja::evt::context_object& cppjinja::evt::exenv_impl::user_data() const
{
	return given_data;
}

const cppjinja::evt::context_objects::queue cppjinja::evt::exenv_impl::all_ctx() const
{
	const auto& l = const_cast<exenv_impl*>(this)->locals();
	return {params(), {&l, &global_namespace, &builtins, &user_data()}};
}

cppjinja::evt::callstack& cppjinja::evt::exenv_impl::calls()
{
	return execalls;
}

cppjinja::evt::result_formatter& cppjinja::evt::exenv_impl::render_format()
{
	return rfmt;
}

std::optional<cppjinja::evt::render_info> cppjinja::evt::exenv_impl::rinfo() const
{
	return cur_rinfo;
}

void cppjinja::evt::exenv_impl::rinfo(std::optional<cppjinja::evt::render_info> ri)
{
	cur_rinfo = ri;
}
