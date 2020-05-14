/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3.hpp>
#include "fusion/expr.hpp"

namespace cppjinja::text::expr_ops {

namespace x3 = boost::spirit::x3;

class term_class;
class bool_class;
class quoted_string_class;

class list_class;
class tuple_class;
class dict_class;

class eq_assign_class;
class in_assign_class;
class math_class;
class mathop_class;
class concat_class;
class in_check_class;
class cmp_check_class;
class logic_check_class;
class negate_class;
class fnc_call_class;
class filter_call_class;
class filter_class;
class single_var_name_class;
class point_class;
class op_if_class;

class expr_class;
class expr_bool_class;

const x3::rule<bool_class, bool> bool_rule = "bool_rule";
const x3::rule<term_class, ast::expr_ops::term> term = "term";
const x3::rule<quoted_string_class, ast::string_t> quoted_string = "quoted_string";
const x3::real_parser<double, x3::strict_real_policies<double> > double_ = {};

const x3::rule<math_class, ast::expr_ops::math> math = "math";
const x3::rule<mathop_class, ast::expr_ops::math_op> mathop = "mathop";
const x3::rule<point_class, ast::expr_ops::point> point = "point";
const x3::rule<single_var_name_class, ast::expr_ops::single_var_name> single_var_name = "single_var_name";

const x3::rule<list_class, ast::expr_ops::list> list = "list";
const x3::rule<tuple_class, ast::expr_ops::tuple> tuple = "tuple";
const x3::rule<dict_class, ast::expr_ops::dict> dict = "dict";
const x3::rule<eq_assign_class, ast::expr_ops::eq_assign> eq_assign = "eq_assign";
const x3::rule<in_assign_class, ast::expr_ops::in_assign> in_assign = "in_assign";
const x3::rule<concat_class, ast::expr_ops::concat> concat = "concat";
const x3::rule<in_check_class, ast::expr_ops::in_check> in_check = "in_check";
const x3::rule<cmp_check_class, ast::expr_ops::cmp_check> cmp_check = "cmp_check";
const x3::rule<logic_check_class, ast::expr_ops::logic_check> logic_check = "logic_check";
const x3::rule<negate_class, ast::expr_ops::negate> negate = "negate";
const x3::rule<fnc_call_class, ast::expr_ops::fnc_call> fnc_call = "fnc_call";
const x3::rule<filter_call_class, ast::expr_ops::filter_call> filter_call = "filter_call";
const x3::rule<filter_class, ast::expr_ops::filter> filter = "filter";
const x3::rule<op_if_class, ast::expr_ops::op_if> op_if = "op_if";

const x3::rule<expr_class, ast::expr_ops::expr> expr = "expr";
const x3::rule<expr_bool_class, ast::expr_ops::expr_bool> expr_bool = "expr_bool";

BOOST_SPIRIT_DECLARE(decltype(term))
BOOST_SPIRIT_DECLARE(decltype(bool_rule));
BOOST_SPIRIT_DECLARE(decltype(quoted_string));
BOOST_SPIRIT_DECLARE(decltype(list));
BOOST_SPIRIT_DECLARE(decltype(tuple));
BOOST_SPIRIT_DECLARE(decltype(dict));
BOOST_SPIRIT_DECLARE(decltype(eq_assign));
BOOST_SPIRIT_DECLARE(decltype(in_assign));
BOOST_SPIRIT_DECLARE(decltype(math));
BOOST_SPIRIT_DECLARE(decltype(mathop));
BOOST_SPIRIT_DECLARE(decltype(concat));
BOOST_SPIRIT_DECLARE(decltype(in_check));
BOOST_SPIRIT_DECLARE(decltype(cmp_check));
BOOST_SPIRIT_DECLARE(decltype(logic_check));
BOOST_SPIRIT_DECLARE(decltype(negate));
BOOST_SPIRIT_DECLARE(decltype(fnc_call));
BOOST_SPIRIT_DECLARE(decltype(filter_call));
BOOST_SPIRIT_DECLARE(decltype(filter));
BOOST_SPIRIT_DECLARE(decltype(point));
BOOST_SPIRIT_DECLARE(decltype(single_var_name));
BOOST_SPIRIT_DECLARE(decltype(op_if));

BOOST_SPIRIT_DECLARE(decltype(expr));
BOOST_SPIRIT_DECLARE(decltype(expr_bool));

} // namespace cppjinja::text::expr_ops
