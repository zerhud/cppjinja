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

const cppjinja::ast::block_with_name& cppjinja::evtnodes::block_call::cur_ast() const
{
	return ast;
}

cppjinja::evtnodes::block_call::block_call(cppjinja::ast::block_call nb)
    : ast(std::move(nb))
{
}

void cppjinja::evtnodes::block_call::render(cppjinja::evt::exenv& env) const
{
	auto obj = env.all_ctx().find(east::var_name{tps(ast.name)});
	auto self = std::make_shared<evt::context_objects::callable_node>(&env, this);
	std::pmr::vector<evt::context_object::function_parameter> params;
	params.emplace_back().value = self;
	for(auto& p:ast.call_params) {
		assert(p.value.has_value());
		auto& cur = params.emplace_back();
		cur.name = p.name;
		cur.value = evt::expr_eval(&env)(*p.value);
	}
	env.out() << obj->call(params)->solve();
}

absd::data cppjinja::evtnodes::block_call::evaluate(
        cppjinja::evt::exenv& env) const
{
	env.current_node(this);
	evt::raii_push_shadow_ctx ctx_maker(this, &env.ctx());
	auto fmt = inner_evaluate(env);
	return env.result();
}

std::pmr::string cppjinja::evtnodes::block_call::name() const
{
	return tps(ast.name + "_call"s);
}
