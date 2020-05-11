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


using namespace std::literals;

cppjinja::east::value_term cppjinja::evt::expr_eval::cvt(const cppjinja::ast::expr_ops::term& v) const
{
	auto cvt = [](const auto& v) { return east::value_term{v}; };
	return boost::apply_visitor(cvt, v);
}

cppjinja::ast::string_t cppjinja::evt::expr_eval::to_str(const cppjinja::ast::expr_ops::term& v) const
{
	auto cvt = [](const auto& v) {
		if constexpr(std::is_same_v<std::decay_t<decltype(v)>, ast::string_t>) return v;
		else return std::to_string(v);
	};
	return boost::apply_visitor(cvt, v.var);
}

cppjinja::ast::string_t cppjinja::evt::expr_eval::to_str(const cppjinja::east::value_term& v) const
{
	std::stringstream out;
	out << v;
	return out.str();
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
	result = result->find(east::var_name{to_str(solved->solve())});
}

void cppjinja::evt::expr_eval::solve_point_arg(cppjinja::ast::expr_ops::point& left) const
{
	(*this)(left);
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
	return std::make_shared<context_objects::value>(cvt(solved));
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(cppjinja::ast::expr_ops::term& t) const
{
	return t;
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
	std::stringstream out;
	std::optional<std::size_t> ind;
	for(auto& item:t.items) {
		auto solved = visit(item);
		if(!ind) ind = solved.var.type().hash_code();
		else if(*ind != solved.var.type().hash_code())
			throw std::runtime_error("array can contains only items with same type");
		print_quoted(out, std::move(solved)) << ',';
	}
	return '[' + replace_back(std::move(out), ']');
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::tuple& t) const
{
	std::stringstream out;
	for(auto& item:t.items) print_quoted(out, visit(item)) << ',';
	return '(' + replace_back(std::move(out), ')');
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::dict& t) const
{
	expr_reduce rdc(env);
	auto reduced = rdc(t);
	std::stringstream out;
	for(auto& item:reduced.items) {
		print_quoted(out, eval_type{item.first}) << ':';
		print_quoted(out, *item.second) << ',';
	}
	return '{' + replace_back(std::move(out), '}');
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::eq_assign& t) const
{ return false; }

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::in_assign& t) const
{ return false; }

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::math& t) const
{
	return boost::apply_visitor(
	            expr_evals::math(t.op)
	          , visit(t.left)
	          , visit(t.right));
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::concat& t) const
{
	return to_str(visit(t.left)) + to_str(visit(t.right));
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::in_check& t) const
{ return false; }

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::cmp_check& t) const
{
	eval_type left = boost::apply_visitor(*this, t.left.get().var);
	eval_type right = boost::apply_visitor(*this, t.right.get().var);
	return boost::apply_visitor(expr_evals::cmp_check(t.op), left, right);
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::logic_check& t) const
{
	eval_type left = boost::apply_visitor(*this, t.left.get().var);
	eval_type right = boost::apply_visitor(*this, t.right.get().var);
	return boost::apply_visitor(expr_evals::logic_check(t.op), left, right);
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::negate& t) const
{
	auto ng = [](const auto& v) -> bool {
		if constexpr(std::is_same_v<std::decay_t<decltype(v)>, ast::string_t>)
		        throw std::runtime_error("cannot negate string");
		else return !v;
	};
	return boost::apply_visitor(ng, boost::apply_visitor(*this, t.arg.get().var));
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::fnc_call& t) const
{
	boost::apply_visitor(*this, t.ref.get());
	result = result->call({});
	return false;
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::filter& t) const
{ return false; }

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
	auto check = [](auto& v)->bool{
		if constexpr(std::is_same_v<std::decay_t<decltype(v)>,ast::string_t>) return !v.empty();
		else return v;
	};
	auto cond = boost::apply_visitor(*this, t.cond.get().var);
	if(boost::apply_visitor(check, cond))
		return boost::apply_visitor(*this, t.term.get().var);
	if(t.alternative)
		return boost::apply_visitor(*this, t.alternative->get().var);
	return ""s;
}

std::string cppjinja::evt::expr_eval::replace_back(std::stringstream src, char nback) const
{
	auto ret = src.str();
	if(ret.empty()) ret = ']';
	else ret.back() = nback;
	return ret;
}

std::ostream& cppjinja::evt::expr_eval::print_quoted(std::stringstream& out, ast::expr_ops::term v) const
{
	auto prt = [&out](auto& v) -> std::ostream& {
		if constexpr (std::is_same_v<std::decay_t<decltype(v)>, ast::string_t>)
		        return out << std::quoted(v);
		else return out << v;
	};
	return boost::apply_visitor(prt, v);
}

std::ostream& cppjinja::evt::expr_eval::print_quoted(std::stringstream& out, east::value_term v) const
{
	auto prt = [&out](auto& v) -> std::ostream& {
		if constexpr (std::is_same_v<std::decay_t<decltype(v)>, ast::string_t>)
		        return out << std::quoted(v);
		else return out << v;
	};
	return std::visit(prt, (east::value_term_var&)v);
}
