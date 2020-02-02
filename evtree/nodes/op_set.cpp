/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "op_set.hpp"
#include "../context.hpp"

using namespace cppjinja::evtnodes;
using namespace std::literals;

op_set::op_set(cppjinja::ast::op_set o) : op(std::move(o))
{
}

cppjinja::evt::node::render_info op_set::rinfo() const
{
	return render_info{
		{op.open.trim, op.close.trim},
		{op.open.trim, op.close.trim}
	};
}

cppjinja::ast::string_t op_set::name() const
{
	return op.name;
}

bool op_set::is_leaf() const
{
	return true;
}

void op_set::render( evt::context& ctx ) const
{
	const node* cur_node = ctx.current_node();
	if(!cur_node || cur_node->name() != "op_out"sv) return;

	ctx.current_node(this);
	render_value(ctx, op.value);
}
