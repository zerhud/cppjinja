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
#include "obj_holder.hpp"

using namespace cppjinja::details;


cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::solve_queue(const ast::function_call& obj)
{
	return solve_in_data(obj);
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::solve_queue(const ast::var_name& obj)
{
	return env->data()->value(east_cvt::cvt(obj));
}

template<typename Obj, typename Cont, typename... Conts>
cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::solve_queue(const Obj& obj, const Cont& cont, const Conts&... conts)
{
	auto ret = cont(obj);
	if(ret) return (*this)(*ret);
	return solve_queue(obj, conts...);
}

template<typename Obj>
std::optional<cppjinja::evt::expr_solver::ret_t>
cppjinja::evt::expr_solver::solve_in_params(const Obj& obj)
{
	for(auto& params:env->params()) {
		auto val = (*params)(obj);
		if(val) return (*this)(*val);
	}
	return std::nullopt;
}

template<typename Obj>
cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::solve_calls(const Obj& obj)
{
	auto in_params = solve_in_params(obj);
	return in_params ? *in_params : solve_queue(obj, env->locals(), env->globals());
}

cppjinja::evt::expr_solver::expr_solver(exenv* e)
    : env(e)
{
	if(!env) throw std::runtime_error("cannot crate solver without environment");
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::value_term& val)
{
	return boost::apply_visitor(*this, val.var);
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
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::array_call& op)
{
	(void)op;
	return ret_t{""};
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::function_call& obj)
{
	return solve_calls(obj);
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::var_name& obj)
{
	return solve_calls(obj);
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::array_v& obj)
{
	return make_array(obj.fields);
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

cppjinja::east::value_term cppjinja::evt::expr_solver::solve_in_data(
        const cppjinja::ast::function_call& obj)
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
