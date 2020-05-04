/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "expr_solver.hpp"

#include "evtree/exenv.hpp"
#include "eval/ast_cvt.hpp"
#include "evtree/helpers/binary_op.hpp"
#include "context_object.hpp"

using namespace cppjinja::details;

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::value_term& val)
{
	return boost::apply_visitor(*this, val.var);
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::var_name& obj)
{
	auto found = env->all_ctx().find(reduce(obj));
	return found->solve();
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::function_call& obj)
{
	auto found = env->all_ctx().find(reduce(obj.ref));
	return found->call(reduce(obj.params));
}

cppjinja::east::var_name cppjinja::evt::expr_solver::reduce(const cppjinja::ast::var_name& obj)
{
	return east_cvt::cvt(obj);
}

cppjinja::east::function_parameter cppjinja::evt::expr_solver::reduce(const cppjinja::ast::function_call_parameter& obj)
{
	east::function_parameter ret;
	ret.name = obj.name;
	ret.val = (*this)(obj.value.get());
	return ret;
}

std::vector<cppjinja::east::function_parameter> cppjinja::evt::expr_solver::reduce(const std::vector<cppjinja::ast::function_call_parameter>& obj)
{
	std::vector<cppjinja::east::function_parameter> ret;
	for(auto& p:obj) ret.emplace_back(reduce(p));
	return ret;
}

cppjinja::evt::expr_solver::expr_solver(exenv* e)
    : env(e)
{
	if(!env) throw std::runtime_error("cannot crate solver without environment");
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::binary_op& op)
{
	auto left = (*this)(op.left);
	auto right = (*this)(op.right);
	evtnodes::binary_op_helper cmp(op.op);
	return std::visit(
	            cmp,
	            (east::value_term_var&)left,
	            (east::value_term_var&)right);
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::array_calls& op)
{
	(void)op;
	return ret_t{""};
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::array_v& obj)
{
	return make_array(obj.fields);
}

cppjinja::east::value_term cppjinja::evt::expr_solver::make_array(
        const std::vector<ast::forward_ast<cppjinja::ast::value_term> >& fields)
{
	east::array_v ret;
	for(auto& i:fields)
		ret.items.push_back(
		            std::make_unique<east::value_term>(
		                (*this)(i.get())));
	return ret;
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::tuple_v& obj)
{
	return make_array(obj.fields);
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::string_t& obj) const
{
	return obj;
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const double& obj) const
{
	return obj;
}
