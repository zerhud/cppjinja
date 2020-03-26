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

bool cppjinja::evtnodes::block_named::is_leaf() const
{
	return false;
}

bool cppjinja::evtnodes::block_named::render_param(
		  evt::context& ctx
		, const ast::var_name& pname
		) const
{
	auto found_param = param(ctx, pname);
	if(found_param) render_value(ctx, *found_param);
	return found_param.has_value();
}

std::optional<cppjinja::ast::value_term> cppjinja::evtnodes::block_named::param(
        evt::context& ctx,
        const cppjinja::ast::var_name& name) const
{
	if(name.size()!=1) return std::nullopt;
	for(std::size_t outer = 0;outer<block.params.size();++outer)
	{
		auto& p = block.params[outer];
		if(p.name != name[0]) continue;

		auto params = ctx.call_params();
		for(std::size_t inner = 0;inner<params.size();++inner)
		{
			auto& cp = params[inner];
			bool found = cp.name.has_value() && *cp.name == name[0];
			if(!found && !cp.name) found = outer == inner;
			if(found) return cp.value.get();
		}

		if(p.value) return p.value;
		else throw std::runtime_error("requried parameter not specified");
	}

	return std::nullopt;
}

void cppjinja::evtnodes::block_named::render(evt::context& ctx) const
{
	ctx.current_node(this);
	ctx.push_context(this);
	auto children = ctx.tree().children(this, false);
	for(auto&& child:children) ctx.add_context(child);
	children = ctx.tree().children(this, false);
	for(auto&& child:children) child->render(ctx);
	ctx.pop_context(this);
}
