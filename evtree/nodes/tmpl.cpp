/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "tmpl.hpp"
#include "evtree/evtree.hpp"
#include "evtree/exenv/context_objects/tree.hpp"
#include "evtree/exenv/context_objects/callable_node.hpp"
#include "evtree/exenv/context_objects/callable_params.hpp"

cppjinja::evtnodes::tmpl::tmpl(cppjinja::ast::tmpl t)
    : itmpl_(std::move(t))
{
}

cppjinja::evt::render_info cppjinja::evtnodes::tmpl::rinfo() const
{
	return {false, false};
}

std::pmr::string cppjinja::evtnodes::tmpl::file_name() const
{
	return {itmpl_.file_name.begin(), itmpl_.file_name.end()};
}

std::pmr::vector<cppjinja::ast::op_import> cppjinja::evtnodes::tmpl::imports() const
{
	std::pmr::vector<cppjinja::ast::op_import> ret;
	for(auto& i:itmpl_.file_imports) ret.emplace_back(i);
	return ret;
}

std::string cppjinja::evtnodes::tmpl::ast_name() const
{
	return itmpl_.name;
}

std::pmr::string cppjinja::evtnodes::tmpl::name() const
{
	return tps(ast_name());
}

void cppjinja::evtnodes::tmpl::render(evt::exenv& env) const
{
	env.current_node(this);

	auto children = env.roots(this);
	for(auto&& child:children) {
		if(child->name().empty()) {
			evt::context_objects::callable_params params({}, {});
			evt::raii_callstack_push stack(&env.calls(), child, std::move(params));
			env.out() << child->evaluate(env);
		}
	}
}
