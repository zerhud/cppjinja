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

cppjinja::evt::render_info cppjinja::evtnodes::block_macro::inner_ri() const
{
	return evt::render_info{ block.left_close.trim, block.right_open.trim };
}

std::vector<cppjinja::evt::raii_inject_obj>
cppjinja::evtnodes::block_macro::make_injections(evt::exenv& env) const
{
	std::vector<cppjinja::evt::raii_inject_obj> ret;
	for(auto& p:block.params) {
		auto val = param(env.calls(), ast::var_name{p.name});
		assert(val.has_value());
		auto obj = std::make_unique<evt::delay_solver>(&env, &val.value());
		ret.emplace_back(p.name, std::move(obj), &env.ctx());
	}
	return ret;
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

bool cppjinja::evtnodes::block_macro::is_leaf() const
{
	return false;
}

void cppjinja::evtnodes::block_macro::render(evt::exenv& env) const
{
	env.current_node(this);
}

cppjinja::east::string_t
cppjinja::evtnodes::block_macro::evaluate(cppjinja::evt::exenv& env) const
{
	env.current_node(this);
	auto children = env.children(this);
	auto raii_injections = make_injections(env);
	render_children(children, env, inner_ri());
	return east::string_t();
}

std::optional<cppjinja::ast::value_term>
cppjinja::evtnodes::block_macro::param(const evt::callstack& ctx
        , const cppjinja::ast::var_name& name
        ) const
{
	return callable::param(block.params, name, ctx);
}
