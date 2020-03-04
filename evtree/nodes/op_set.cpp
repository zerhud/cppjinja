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

cppjinja::evt::render_info op_set::rinfo() const
{
	return {op.open.trim, op.close.trim};
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
	render_value(ctx, param(ctx, pname).value());
	return true;
}

std::optional<cppjinja::ast::value_term> op_set::param(
          cppjinja::evt::context& ctx
        , const cppjinja::ast::var_name& name
        ) const
{
	(void) ctx; //TODO: why ctx unused?

	if(name.empty()) throw std::runtime_error("var name is empty");
	if(name.back()!=op.name)
	{
		auto message = "var name is "s;
		for(auto& n:name) message += n + '.';
		message.back() = ' ';
		message += "but called "s + op.name;
		throw std::runtime_error(message);
	}

	return op.value;
}
