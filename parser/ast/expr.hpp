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
enum class logic_op { nop, op_and, op_or, op_not };

struct expr;

using term_var = x3::variant<bool, std::int64_t, double, string_t>;
struct term : term_var {
	using base_type::base_type;
	using base_type::operator=;
};

struct list { std::vector<forward_ast<expr>> items; };
struct tuple { std::vector<forward_ast<expr>> items; };
struct dict {
	std::vector<forward_ast<expr>> names;
	std::vector<forward_ast<expr>> values;
};

struct assign {
	std::vector<forward_ast<expr>> name;
	forward_ast<expr> value;
};

struct in_assign : assign {};

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

struct login_check {
	logic_op op;
	forward_ast<expr> left;
	forward_ast<expr> right;
};

struct negate {
	forward_ast<expr> arg;
};

struct fnc_call {
	forward_ast<expr> ref;
	std::vector<forward_ast<expr>> pos_args;
	std::vector<forward_ast<expr>> named_args;
};

struct filter {
	forward_ast<expr> ref;
	std::optional<std::vector<forward_ast<expr>>> pos_args;
	std::optional<std::vector<forward_ast<expr>>> named_args;
};

struct point {
	forward_ast<expr> left;
	forward_ast<expr> right;
};

struct op_if {
	forward_ast<expr> term;
	forward_ast<expr> cond;
	std::optional<forward_ast<expr>> alternative;
};

using expr_var = x3::variant<
term, list, tuple, dict,
assign, in_assign,
math, concat,
in_check, cmp_check, login_check, negate,
fnc_call, filter, point, op_if
>;
struct expr : expr_var {
	using base_type::base_type;
	using base_type::operator=;
};

} // namespace cppjinja::ast::expr_ops
