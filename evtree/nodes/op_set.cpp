/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "op_set.hpp"
#include "exenv.hpp"
#include "evtree/exenv/expr_eval.hpp"
#include "evtree/exenv/context_objects/value.hpp"

using namespace cppjinja::evtnodes;
using namespace std::literals;

void op_set::set_names(evt::exenv& env, const cppjinja::ast::expr_ops::tuple& tval) const
{
	for(std::size_t i=0;i<op.value.names.size();++i) {
		const auto& name =
		        boost::get<ast::expr_ops::single_var_name>(op.value.names[i]);
		auto val = std::make_shared<evt::context_objects::value>(
		            evt::expr_eval(&env)(tval.items.at(i)));
		env.locals().add(tps(name.name), std::move(val));
	}
}

op_set::op_set(cppjinja::ast::op_set o) : op(std::move(o))
{
	if(op.value.names.empty())
		throw std::runtime_error("no names in set");
}

cppjinja::evt::render_info op_set::rinfo() const
{
	return {op.open.trim, op.close.trim};
}

void op_set::render(evt::exenv& env) const
{
	env.current_node(this);
	const ast::expr_ops::tuple* tval =
	        boost::get<ast::expr_ops::tuple>(&op.value.value.get());
	if(tval) set_names(env, *tval);
	else {
		const ast::expr_ops::single_var_name& name =
		        boost::get<ast::expr_ops::single_var_name>(op.value.names.at(0));
		auto val = std::make_shared<evt::context_objects::value>(
		            evt::expr_eval(&env)(op.value.value.get()));
		env.locals().add(tps(name.name), std::move(val));
	}
}
