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

cppjinja::evt::node::render_info cppjinja::evtnodes::block_named::rinfo() const
{
	return render_info{
		{block.left_open.trim, block.right_close.trim},
		{block.left_close.trim, block.right_open.trim}
	};
}

cppjinja::ast::string_t cppjinja::evtnodes::block_named::name() const
{
	return block.name;
}

bool cppjinja::evtnodes::block_named::is_leaf() const
{
	return true;
}

void cppjinja::evtnodes::block_named::render(evt::context& ctx) const
{
/*
using block_content = x3::variant
<
	  string_t
	, op_out
	, op_comment
	, op_set
	, forward_ast<block_raw>
	, forward_ast<block_if>
	, forward_ast<block_for>
	, forward_ast<block_macro>
	, forward_ast<block_named>
	, forward_ast<block_filtered>
	, forward_ast<block_set>
	, forward_ast<block_call>
>;
*/

	//node* cur_cnt = nullptr;
	//overloaded renderer {
	    //[&out](const ast::string_t& obj){ out << obj; },
	    //[&out](const ast::op_out& obj) { out << "op_out"; },
	    //[&out](const auto&){ out << "not ready yet"; }
	//};
	//for(auto& c:block.content) boost::apply_visitor(renderer, c.var);

	ctx.current_node(this);
	ctx.push_callstack(this);
	auto children = ctx.tree().children(this);
	for(auto&& child:children) ctx.add_context(child);
	for(auto&& child:children) child->render(ctx);
	ctx.pop_callstack(this);
}
