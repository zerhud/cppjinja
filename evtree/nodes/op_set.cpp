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
	(void) ctx;
}

bool op_set::render_param(
          cppjinja::evt::context& ctx
        , const cppjinja::ast::var_name& pname
        ) const
{
	if(pname.empty()) throw std::runtime_error("var name is empty");
	if(pname.back()!=op.name)
	{
		auto message = "var name is "s;
		for(auto& n:pname) message += n + '.';
		message.back() = ' ';
		message += "but called "s + op.name;
		throw std::runtime_error(message);
	}

	ctx.current_node(this);
	render_value(ctx, op.value);
	return true;
}
