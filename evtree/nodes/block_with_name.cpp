/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "block_with_name.hpp"
#include "evtree/exenv/expr_eval.hpp"

cppjinja::evt::render_info cppjinja::evtnodes::block_with_name::inner_ri() const
{
	auto& block = cur_ast();
	return evt::render_info{ block.left_close.trim, block.right_open.trim };
}

cppjinja::evt::raii_result_format
cppjinja::evtnodes::block_with_name::result_format_raii(cppjinja::evt::exenv& env) const
{
	auto& block = cur_ast();
	return  evt::raii_result_format(
	            &env.render_format(),
	            block.left_close.bsign?*block.left_close.bsign:0,
	            block.right_open.bsign?*block.right_open.bsign:0
	                                   );
}

cppjinja::evt::raii_result_format cppjinja::evtnodes::block_with_name::inner_evaluate(cppjinja::evt::exenv& env) const
{
	auto fmt = result_format_raii(env);
	auto children = env.children(this);
	render_children(children, env, inner_ri());
	return fmt;
}

cppjinja::evt::render_info cppjinja::evtnodes::block_with_name::rinfo() const
{
	auto& block = cur_ast();
	return {block.left_open.trim, block.right_close.trim};
}

std::pmr::string cppjinja::evtnodes::block_with_name::name() const
{
	return tps(cur_ast().name);
}

std::pmr::vector<cppjinja::east::function_parameter>
cppjinja::evtnodes::block_with_name::solved_params(cppjinja::evt::exenv& env) const
{
	std::pmr::vector<cppjinja::east::function_parameter> ret;
	evt::expr_eval slv(&env);
	for(auto& p:cur_ast().params){
		auto& i = ret.emplace_back();
		i.name = p.name;
		if(p.value) i.val = slv(*p.value);
	}
	return ret;
}
