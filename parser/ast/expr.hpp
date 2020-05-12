/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <optional>

#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include "../common.hpp"

namespace cppjinja::ast::expr_ops {

enum class math_op { nop, pls, mns, mul, dev, mod, pow, trunc_dev };
enum class cmp_op { nop, eq, neq, less, more, less_eq, more_eq };
enum class logic_op { nop, op_and, op_or };

struct expr;


using term_var = x3::variant<bool, std::int64_t, double, string_t>;
struct term : term_var {
	using base_type::base_type;
	using base_type::operator=;
	term(bool v) : term_var(v) {}
	term(double v) : term_var(v) {}
	term(string_t v) : term_var(v) {}
	template<typename Int>
	explicit term(Int v) : term_var((std::int64_t)v) {}
};

struct list { std::vector<forward_ast<expr>> items; };
struct tuple { std::vector<forward_ast<expr>> items; };
struct dict_item { string_t name; forward_ast<expr> value; };
struct dict { std::vector<dict_item> items; };

struct point;
struct single_var_name { string_t name; };
using point_element = x3::variant<single_var_name, forward_ast<point>, forward_ast<expr>> ;

struct point {
	point_element left;
	point_element right;
};

struct lvalue : x3::variant<single_var_name, point> {
	using base_type::base_type;
	using base_type::operator=;
};

struct any_assign {
	std::vector<lvalue> names;
	forward_ast<expr> value;
};

struct in_assign : any_assign {};
struct eq_assign : any_assign {};

struct math {
	math_op op;
	forward_ast<expr> left;
	forward_ast<expr> right;
};

struct concat {
	forward_ast<expr> left;
	forward_ast<expr> right;
};

struct in_check {
	forward_ast<expr> term;
	forward_ast<expr> object;
};

struct cmp_check {
	cmp_op op;
	forward_ast<expr> left;
	forward_ast<expr> right;
};

struct logic_check {
	logic_op op;
	forward_ast<expr> left;
	forward_ast<expr> right;
};

struct negate {
	forward_ast<expr> arg;
};

struct fnc_call {
	lvalue ref;
	std::vector<forward_ast<expr>> args;
};

struct filter_call {
	lvalue ref;
	std::vector<forward_ast<expr>> args;
};

struct filter {
	forward_ast<expr> base;
	std::vector<filter_call> filters;
};

struct op_if {
	forward_ast<expr> term;
	forward_ast<expr> cond;
	std::optional<forward_ast<expr>> alternative;
};

using expr_var = x3::variant<
term, single_var_name, list, tuple, dict,
eq_assign, in_assign,
math, concat,
cmp_check, logic_check, negate,
fnc_call, filter, point, op_if
>;
struct expr : expr_var {
	using base_type::base_type;
	using base_type::operator=;
};

} // namespace cppjinja::ast::expr_ops
