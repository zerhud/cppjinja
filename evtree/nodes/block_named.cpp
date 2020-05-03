/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "block_named.hpp"
#include "parser/helpers.hpp"
#include "../evtree.hpp"
#include "evtree/exenv/callstack.hpp"
#include "evtree/exenv/expr_solver.hpp"

bool cppjinja::evtnodes::block_named::has_nondefaulted_params() const
{
	for(auto& p:block.params) if(!p.value.has_value()) return true;
	return false;
}

cppjinja::evt::raii_result_format
cppjinja::evtnodes::block_named::result_format_raii(cppjinja::evt::exenv& env) const
{
	return  evt::raii_result_format(
	            &env.render_format(),
	            block.left_close.bsign?*block.left_close.bsign:0,
	            block.right_open.bsign?*block.right_open.bsign:0
	            );
}

cppjinja::evtnodes::block_named::block_named(cppjinja::ast::block_named nb)
    : block(std::move(nb))
{
}

cppjinja::evt::render_info cppjinja::evtnodes::block_named::rinfo() const
{
	return {block.left_open.trim, block.right_close.trim};
}

cppjinja::ast::string_t cppjinja::evtnodes::block_named::name() const
{
	return block.name;
}

std::vector<cppjinja::ast::macro_parameter>
cppjinja::evtnodes::block_named::params() const
{
	return block.params;
}

std::vector<cppjinja::east::function_parameter>
cppjinja::evtnodes::block_named::solved_params(cppjinja::evt::exenv& env) const
{
	std::vector<cppjinja::east::function_parameter> ret;
	evt::expr_solver slv(&env);
	for(auto& p:block.params){
		auto& i = ret.emplace_back();
		i.name = p.name;
		if(p.value)
			i.val = slv(*p.value);
	}
	return ret;
}

void cppjinja::evtnodes::block_named::render(evt::exenv& env) const
{
	if(has_nondefaulted_params()) env.current_node(this);
	else  env.out() << env.calls().call(&env, this, {});
}

cppjinja::east::string_t cppjinja::evtnodes::block_named::evaluate(cppjinja::evt::exenv& env) const
{
	env.current_node(this);
	auto fmt = result_format_raii(env);
	evt::raii_push_ctx ctx_maker(this, &env.ctx());
	evt::render_info ri{ block.left_close.trim, block.right_open.trim };
	auto children = env.children(this);
	render_children(children, env, ri);
	return env.result();
}
