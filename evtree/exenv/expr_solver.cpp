/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "expr_solver.hpp"
#include "context.hpp"
#include "callstack.hpp"

#include "evtree/exenv.hpp"
#include "eval/ast_cvt.hpp"
#include "evtree/helpers/binary_op.hpp"
#include "evtree/nodes/callable.hpp"
#include "obj_holder.hpp"

using namespace cppjinja::details;

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
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::function_call& obj)
{
	for(auto& params:env->params()) {
		auto val = params->call(obj);
		if(val) return (*this)(*val);
	}
	auto ctx_val = env->locals().call(obj);
	if(ctx_val) return (*this)(*ctx_val);
	auto glob_val = env->globals().call(obj);
	if(glob_val) return (*this)(*glob_val);
	return solve_in_data(obj);
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::var_name& obj)
{
	for(auto& params:env->params()) {
		auto val = params->solve(obj);
		if(val) return (*this)(*val);
	}
	auto ctx_val = env->locals().solve(obj);
	if(ctx_val) return (*this)(*ctx_val);
	auto glob_val = env->globals().solve(obj);
	if(glob_val) return (*this)(*glob_val);
	return env->data()->value(east_cvt::cvt(obj));
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
	throw std::runtime_error("no calling node in context");
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

cppjinja::east::value_term cppjinja::evt::expr_solver::solve_in_tmpl(
          const cppjinja::ast::function_call& obj
        , const evtnodes::callable* node)
{
	assert(env);
	return env->calls().call(env, node, obj.params);
}
