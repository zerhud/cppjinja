/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "block_call.hpp"


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
	std::vector<east::function_parameter> params;
	auto obj = env.all_ctx().find(east::var_name{ast.name});
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
	return {};
}

cppjinja::ast::string_t cppjinja::evtnodes::block_call::name() const
{
	return "call_" + ast.name;
}
