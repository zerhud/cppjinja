/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "block_call.hpp"
#include "evtree/exenv/context_objects/callable_node.hpp"
#include "evtree/exenv/expr_eval.hpp"

using namespace std::literals;

cppjinja::evt::render_info cppjinja::evtnodes::block_call::inner_ri() const
{
	return evt::render_info{ ast.left_close.trim, ast.right_open.trim };
}

cppjinja::evtnodes::block_call::block_call(cppjinja::ast::block_call nb)
    : ast(std::move(nb))
{
}

void cppjinja::evtnodes::block_call::render(cppjinja::evt::exenv& env) const
{
	auto obj = env.all_ctx().find(east::var_name{ast.name});
	auto self = std::make_shared<evt::context_objects::callable_node>(&env, this);
	std::vector<evt::context_object::function_parameter> params;
	params.emplace_back().value = self;
	for(auto& p:ast.call_params) {
		assert(p.value.has_value());
		auto& cur = params.emplace_back();
		cur.name = p.name;
		cur.value = evt::expr_eval(&env)(*p.value);
	}
	env.out() << obj->call(params)->solve();
}

cppjinja::evt::render_info cppjinja::evtnodes::block_call::rinfo() const
{
	return {ast.left_open.trim, ast.right_close.trim};
}

cppjinja::east::string_t cppjinja::evtnodes::block_call::evaluate(
        cppjinja::evt::exenv& env) const
{
	return "";
}

std::vector<cppjinja::east::function_parameter>
cppjinja::evtnodes::block_call::solved_params(cppjinja::evt::exenv& env) const
{
	std::vector<cppjinja::east::function_parameter> ret;
	evt::expr_eval slv(&env);
	for(auto& p:ast.params){
		auto& i = ret.emplace_back();
		i.name = p.name;
		if(p.value) i.jval = slv(*p.value)->jval();
	}
	return ret;
}

cppjinja::ast::string_t cppjinja::evtnodes::block_call::name() const
{
	return ast.name + "_call"s;
}
