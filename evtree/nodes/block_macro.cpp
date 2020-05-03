/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <istream>
#include "block_macro.hpp"
#include "../evtree.hpp"
#include "evtree/exenv/ctx_object.hpp"
#include "evtree/exenv/expr_solver.hpp"

cppjinja::evt::render_info cppjinja::evtnodes::block_macro::inner_ri() const
{
	return evt::render_info{ block.left_close.trim, block.right_open.trim };
}

cppjinja::evt::raii_result_format
cppjinja::evtnodes::block_macro::result_format_raii(cppjinja::evt::exenv& env) const
{
	return  evt::raii_result_format(
	            &env.render_format(),
	            block.left_close.bsign?*block.left_close.bsign:0,
	            block.right_open.bsign?*block.right_open.bsign:0
	            );
}

cppjinja::evtnodes::block_macro::block_macro(cppjinja::ast::block_macro nb)
    : block(std::move(nb))
{
}

cppjinja::evt::render_info cppjinja::evtnodes::block_macro::rinfo() const
{
	return {block.left_open.trim, block.right_close.trim};
}

cppjinja::ast::string_t cppjinja::evtnodes::block_macro::name() const
{
	return block.name;
}

void cppjinja::evtnodes::block_macro::render(evt::exenv& env) const
{
	env.current_node(this);
}

cppjinja::east::string_t
cppjinja::evtnodes::block_macro::evaluate(cppjinja::evt::exenv& env) const
{
	env.current_node(this);
	auto fmt = result_format_raii(env);
	auto children = env.children(this);
	render_children(children, env, inner_ri());
	return east::string_t();
}

std::vector<cppjinja::ast::macro_parameter>
cppjinja::evtnodes::block_macro::params() const
{
	return block.params;
}

std::vector<cppjinja::east::function_parameter>
cppjinja::evtnodes::block_macro::solved_params(cppjinja::evt::exenv& env) const
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
