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

#include <absd/simple_data_holder.hpp>

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
    , nav_env(this)
    , nav_tmpl(this, nullptr)
{
}

const cppjinja::evtree& cppjinja::evt::exenv_impl::tmpl() const
{
	assert(compiled_template);
	return *compiled_template;
}

std::pmr::vector<const cppjinja::evt::node*>
cppjinja::evt::exenv_impl::children(const cppjinja::evt::node* selected) const
{
	assert(compiled_template);
	return compiled_template->children(selected);
}

std::pmr::vector<const cppjinja::evtnodes::callable*>
cppjinja::evt::exenv_impl::roots(const cppjinja::evtnodes::tmpl* tmpl) const
{
	return compiled_template->roots(tmpl);
}

std::shared_ptr<std::pmr::memory_resource> cppjinja::evt::exenv_impl::storage() const
{
	return mem;
}

const cppjinja::data_provider* cppjinja::evt::exenv_impl::data() const
{
	return given_data.data();
}

std::ostream& cppjinja::evt::exenv_impl::out()
{
	return ctx().out();
}

absd::data cppjinja::evt::exenv_impl::result() const
{
	auto val = std::allocate_shared<absd::simple_data_holder>(
	            std::pmr::polymorphic_allocator(storage().get()),
	            storage(), rfmt(ctx().result()) );
	return absd::data{ std::move(val) };
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

const cppjinja::evtnodes::tmpl* cppjinja::evt::exenv_impl::current_tmpl() const
{
	return ctx().current_tmpl();
}

cppjinja::evt::context_object& cppjinja::evt::exenv_impl::locals()
{
	return ctx().cur_namespace();
}

cppjinja::evt::context_object& cppjinja::evt::exenv_impl::globals()
{
	return global_namespace;
}

const cppjinja::evt::context_object& cppjinja::evt::exenv_impl::inner_env()
{
	return nav_env;
}

const cppjinja::evt::context_object& cppjinja::evt::exenv_impl::inner_tmpl()
{
	return nav_tmpl;
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
	using context_objects::queue;
	const auto& l = const_cast<exenv_impl*>(this)->locals();
	return {params(), queue::clist{
			&l, &global_namespace,
			&nav_tmpl, &nav_env,
			&builtins, &user_data()
		}};
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
