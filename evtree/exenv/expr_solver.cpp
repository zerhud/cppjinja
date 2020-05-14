/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "expr_solver.hpp"

#include <exception>

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
	assert(found);
	return found->solve();
}

cppjinja::evt::expr_solver::ret_t
cppjinja::evt::expr_solver::operator()(const cppjinja::ast::function_call& obj)
{
	auto found = env->all_ctx().find(reduce(obj.ref));
	assert(found);
	found = found->call(reduce(obj.params));
	assert(found);
	return found->solve();
}

cppjinja::east::var_name cppjinja::evt::expr_solver::reduce(const cppjinja::ast::var_name& obj)
{
	return east_cvt::cvt(obj);
}

cppjinja::east::function_parameter cppjinja::evt::expr_solver::reduce(const cppjinja::ast::function_call_parameter& obj)
{
	east::function_parameter ret;
	ret.name = obj.name;
	return ret;
}

std::vector<cppjinja::east::function_parameter> cppjinja::evt::expr_solver::reduce(const std::vector<cppjinja::ast::function_call_parameter>& obj)
{
	std::vector<cppjinja::east::function_parameter> ret;
	for(auto& p:obj) ret.emplace_back(reduce(p));
	return ret;
}

cppjinja::east::var_name cppjinja::evt::expr_solver::reduce(const boost::spirit::x3::variant<cppjinja::ast::var_name, cppjinja::ast::array_calls>& obj)
{
	auto r = [this](const auto& v){return reduce(v);};
	return boost::apply_visitor(r, obj.var);
}

cppjinja::east::var_name cppjinja::evt::expr_solver::reduce(const cppjinja::ast::array_calls& obj)
{
	east::var_name ret;
	for(auto& np:obj) {
		if(np.name) for(auto& nn:*np.name) ret.emplace_back(nn);
		ret.emplace_back(reduce_to_string((*this)(np.call.get())));
	}
	return ret;
}

cppjinja::east::function_call cppjinja::evt::expr_solver::reduce(const cppjinja::ast::function_call& obj)
{
	return east::function_call{reduce(obj.ref), reduce(obj.params)};
}

cppjinja::east::string_t cppjinja::evt::expr_solver::reduce_to_string(const cppjinja::east::value_term& obj)
{
	if(std::holds_alternative<east::string_t>(obj))
		return std::get<east::string_t>(obj);
	if(std::holds_alternative<double>(obj)) {
		double val = std::get<double>(obj);
		if(val == std::trunc(val))
			return std::to_string((std::int64_t)std::trunc(val));
		return std::to_string(val);
	}
	if(std::holds_alternative<std::int64_t>(obj))
		return std::to_string(std::get<std::int64_t>(obj));
	throw std::logic_error("canno solve neested array yet");
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
	east::var_name solved = reduce(op);
	return (*this)(solved);
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
