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
#include "evtree/nodes/callable.hpp"

using namespace cppjinja::details;

cppjinja::evt::expr_solver::expr_solver(const exenv* e)
    : env(e)
{
	if(!env) throw std::runtime_error("cannot crate solver without environment");
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::value_term& val) const
{
	return boost::apply_visitor(*this, val.var);
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::binary_op& op) const
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
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::function_call& obj) const
{
	east::function_call call;
	call.ref = east_cvt::cvt(obj.ref);
	for(auto& src:obj.params)
		call.params.emplace_back(
		            east::function_parameter{
		                src.name,
		                (*this)(src.value.get())});
	return env->data()->value(call);
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::var_name& obj) const
{
	auto param_val = search_in_params(obj);
	if(param_val) return (*this)(*param_val);
	auto ctx_val = env->ctx().solve_var(obj);
	if(ctx_val) return (*this)(*ctx_val);
	return env->data()->value(east_cvt::cvt(obj));
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::array_v& obj) const
{
	return make_array(obj.fields);
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::tuple_v& obj) const
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

cppjinja::east::value_term cppjinja::evt::expr_solver::make_array(
        const std::vector<ast::forward_ast<cppjinja::ast::value_term> >& fields) const
{
	east::array_v ret;
	for(auto& i:fields)
		ret.items.push_back(
		            std::make_unique<east::value_term>(
		                (*this)(i.get())));
	return ret;
}

std::optional<cppjinja::ast::value_term>
cppjinja::evt::expr_solver::search_in_params(const cppjinja::ast::var_name& var) const
{
	std::vector<const evtnodes::callable*> callings = env->calls().calling_stack();
	assert(!callings.empty());
	for(auto* calling:callings)
	{
		auto cur_param = calling->param(env->calls(), var);
		if(cur_param.has_value()|| calling == env->ctx().maker())
			return cur_param;
	}
	return std::nullopt;
}
