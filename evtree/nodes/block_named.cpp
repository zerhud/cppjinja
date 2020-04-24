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

bool cppjinja::evtnodes::block_named::has_nondefaulted_params() const
{
	for(auto& p:block.params) if(!p.value.has_value()) return true;
	return false;
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

std::optional<cppjinja::ast::value_term> cppjinja::evtnodes::block_named::param(
        const evt::callstack& ctx, const cppjinja::ast::var_name& name) const
{
	return param(block.params, name, ctx);
}

void cppjinja::evtnodes::block_named::render(evt::exenv& env) const
{
	if(has_nondefaulted_params())
		env.current_node(this);
	else {
		evt::raii_push_callstack call(this, this, &env.calls());
		env.out() << evaluate(env);
	}
}

cppjinja::east::string_t cppjinja::evtnodes::block_named::evaluate(cppjinja::evt::exenv& env) const
{
	env.current_node(this);
	evt::raii_push_ctx ctx_maker(this, &env.ctx());
	evt::render_info ri{ block.left_close.trim, block.right_open.trim };
	auto children = env.children(this);
	render_children(children, env, ri);
	return env.result();
}
