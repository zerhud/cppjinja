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
    , user_data(prov)
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

const cppjinja::evtnodes::callable*
cppjinja::evt::exenv_impl::search_callable(std::string_view name) const
{
	const node* found = compiled_template->search(
	            ast::var_name{ast::string_t(name)}, exectx.nth_node_on_stack(0));
	return dynamic_cast<const evtnodes::callable*>(found);
}

std::vector<const cppjinja::evtnodes::callable*>
cppjinja::evt::exenv_impl::roots(const cppjinja::evtnodes::tmpl* tmpl) const
{
	return compiled_template->roots(tmpl);
}

const cppjinja::data_provider* cppjinja::evt::exenv_impl::data() const
{
	assert(user_data);
	return user_data;
}

std::ostream& cppjinja::evt::exenv_impl::out()
{
	return ctx().out();
}

cppjinja::east::string_t cppjinja::evt::exenv_impl::result() const
{
	return ctx().result();
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

cppjinja::evt::obj_holder& cppjinja::evt::exenv_impl::locals()
{
	return ctx().cur_namespace();
}

cppjinja::evt::obj_holder& cppjinja::evt::exenv_impl::globals()
{
	return global_namespace;
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
