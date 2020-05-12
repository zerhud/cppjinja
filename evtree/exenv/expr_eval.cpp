/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "expr_eval.hpp"

#include <iomanip>

#include "evtree/exenv.hpp"
#include "context_objects/value.hpp"

#include "expr/math.hpp"
#include "expr/cmp_check.hpp"
#include "expr/logic_check.hpp"
#include "expr_reduce.hpp"
#include "expr_filter.hpp"


using namespace std::literals;

cppjinja::json cppjinja::evt::expr_eval::cvt(const cppjinja::ast::expr_ops::term& v) const
{
	auto cvt = [](const auto& v) -> eval_type { return v; };
	return boost::apply_visitor(cvt, v);
}

cppjinja::ast::string_t cppjinja::evt::expr_eval::to_str(const json& v) const
{
	if(v.is_string()) return v.get<std::string>();
	if(v.is_number_integer()) return std::to_string(v.get<std::int64_t>());
	if(v.is_number_float()) return std::to_string(v.get<double>());
	if(v.is_boolean()) return v.get<bool>() ? "true" : "false";
	if(v.is_null()) throw std::runtime_error("cannot convert null to string");
	return v.dump();
}

cppjinja::ast::expr_ops::term cppjinja::evt::expr_eval::to_term(const cppjinja::json& j) const
{
	using ast::expr_ops::term;
	if(j.is_number_integer()) return term{j.get<std::int64_t>()};
	if(j.is_number_float()) return term{j.get<double>()};
	if(j.is_string()) return term{j.get<std::string>()};
	if(j.is_boolean()) return term{j.get<bool>()};
	throw std::runtime_error("cannot apply this operator on such type");
}

void cppjinja::evt::expr_eval::solve_point_arg(cppjinja::ast::expr_ops::point_element& left) const
{
	auto setter = [this](auto& l) { solve_point_arg(l); };
	boost::apply_visitor(setter, left);
}

void cppjinja::evt::expr_eval::solve_point_arg(cppjinja::ast::expr_ops::single_var_name& left) const
{
	if(!result) result = env->all_ctx().find(east::var_name{left.name});
	else result = result->find(east::var_name{left.name});
}

void cppjinja::evt::expr_eval::solve_point_arg(ast::expr_ops::expr& left) const
{
	if(!result) throw std::runtime_error("wrong access element");
	auto solved = expr_eval(env)(left);
	result = result->find(east::var_name{to_str(solved->jval())});
}

void cppjinja::evt::expr_eval::solve_point_arg(cppjinja::ast::expr_ops::point& left) const
{
	(*this)(left);
}

cppjinja::east::function_parameter cppjinja::evt::expr_eval::make_param(cppjinja::ast::expr_ops::expr& pexpr) const
{
	east::function_parameter ret;
	if(auto* asg = boost::get<ast::expr_ops::eq_assign>(&pexpr); asg) {
		ret.name = make_param_name(asg->names.at(0));
		ret.jval = (*this)(asg->value.get())->jval();
	}
	else
		ret.jval = (*this)(pexpr)->jval();
	return ret;
}

std::optional<cppjinja::east::string_t> cppjinja::evt::expr_eval::make_param_name(ast::expr_ops::lvalue& name) const
{
	return boost::get<ast::expr_ops::single_var_name>(name).name;
}

void cppjinja::evt::expr_eval::filter_content(cppjinja::ast::expr_ops::filter_call& call) const
{
	assert(result);

	auto base = result->jval();
	result.reset();
	auto filter = solve_ref(call.ref);

	std::vector<east::function_parameter> params;
	params.emplace_back(east::function_parameter{"$", std::nullopt, std::move(base)});
	for(auto& p:call.args) params.emplace_back(make_param(p.get()));

	result = filter->call(params);
}

std::shared_ptr<cppjinja::evt::context_object> cppjinja::evt::expr_eval::solve_ref(cppjinja::ast::expr_ops::lvalue& ref) const
{
	assert(!result);
	boost::apply_visitor([this](auto& r){solve_point_arg(r);}, ref);
	if(!result) throw std::runtime_error("cannot solve filter");
	auto ret = result;
	result.reset();
	return ret;
}

cppjinja::evt::expr_eval::expr_eval(const exenv* e)
    : env(e)
{
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::expr_eval::operator ()(cppjinja::ast::expr_ops::expr t) const
{
	auto solved = boost::apply_visitor(*this, t);
	if(result) return result;
	return std::make_shared<context_objects::value>(solved,1);
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(cppjinja::ast::expr_ops::term& t) const
{
	return cvt(t);
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::single_var_name& t) const
{
	result = env->all_ctx().find(east::var_name{t.name});
	return false;
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::list& t) const
{
	auto ret = json::array();
	for(auto& item:t.items) ret.emplace_back(visit(item));
	return ret;
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::tuple& t) const
{
	auto ret = json::array();
	for(auto& item:t.items) ret.emplace_back(visit(item));
	return ret;
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::dict& t) const
{
	json ret;
	for(auto& item:t.items) ret[item.name] = visit(item.value);
	return ret;
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::eq_assign&) const
{
	throw std::logic_error("cannot solve eq_assign expression");
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::in_assign&) const
{
	throw std::logic_error("cannot solve in_assign expression");
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::math& t) const
{
	return cvt(boost::apply_visitor(
	            expr_evals::math(t.op),
	            to_term(visit(t.left)),
	            to_term(visit(t.right))
	            ));
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::concat& t) const
{
	return to_str(visit(t.left)) + to_str(visit(t.right));
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::cmp_check& t) const
{
	return cvt(boost::apply_visitor(
	            expr_evals::cmp_check(t.op),
	            to_term(visit(t.left)),
	            to_term(visit(t.right))
	            ));
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::logic_check& t) const
{
	return cvt(boost::apply_visitor(
	            expr_evals::logic_check(t.op),
	            to_term(visit(t.left)),
	            to_term(visit(t.right))
	            ));
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::negate& t) const
{
	auto solved = visit(t.arg);
	if(solved.is_boolean()) return !solved.get<bool>();
	if(solved.is_number()) return !(solved.get<double>());
	throw std::runtime_error("cannot nagate such value");
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::fnc_call& t) const
{
	auto call_obj = solve_ref(t.ref);
	std::vector<east::function_parameter> params;
	for(auto& p:t.args) params.emplace_back(make_param(p.get()));
	result = call_obj->call(params);
	return false;
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::filter& t) const
{
	result = (*this)(t.base.get());
	for(auto& f:t.filters) filter_content(f);
	return false;
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::point& t) const
{
	solve_point_arg(t.left);
	if(!result) throw std::runtime_error("cannot find");
	solve_point_arg(t.right);
	if(!result) throw std::runtime_error("cannot find");
	return eval_type{""s};
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::op_if& t) const
{
	auto cond = visit(t.cond);
	bool result_cond = false;
	if(cond.is_boolean()) result_cond = cond.get<bool>();
	else if(cond.is_number()) result_cond = cond.get<double>();
	if(result_cond) return visit(t.term);
	if(t.alternative) return visit(*t.alternative);
	return nullptr;
}
