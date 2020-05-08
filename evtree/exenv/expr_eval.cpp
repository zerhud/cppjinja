/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "expr_eval.hpp"
#include "context_objects/value.hpp"

#include "expr/math.hpp"

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

cppjinja::evt::expr_eval::expr_eval(cppjinja::evt::exenv* e)
    : env(e)
{
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::expr_eval::operator ()(cppjinja::ast::expr_ops::expr t) const
{
	auto result = boost::apply_visitor(*this, t);
	return std::make_shared<context_objects::value>(cvt(result));
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(cppjinja::ast::expr_ops::term& t) const
{
	return t;
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::single_var_name& t) const
{ return false; }

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::list& t) const
{ return false; }

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::tuple& t) const
{ return false; }

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::dict& t) const
{ return false; }

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::eq_assign& t) const
{ return false; }

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::in_assign& t) const
{ return false; }

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::math& t) const
{
	eval_type left = boost::apply_visitor(*this, t.left.get().var);
	eval_type right = boost::apply_visitor(*this, t.right.get().var);
	return boost::apply_visitor(expr_evals::math(t.op), left, right);
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::concat& t) const
{
	eval_type left = boost::apply_visitor(*this, t.left.get().var);
	eval_type right = boost::apply_visitor(*this, t.right.get().var);
	return to_str(left) + to_str(right);
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::in_check& t) const
{ return false; }

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::cmp_check& t) const
{ return false; }

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::logic_check& t) const
{ return false; }

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
{ return false; }

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::filter& t) const
{ return false; }

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::point& t) const
{ return false; }

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::op_if& t) const
{ return false; }
