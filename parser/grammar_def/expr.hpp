/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <iostream>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>
#include <boost/spirit/home/support/utf8.hpp>
#include "grammar/expr.hpp"

namespace cppjinja::text::expr_ops {


const x3::rule<class math1_class, ast::expr_ops::math> math1 = "math1";
const x3::rule<class math2_class, ast::expr_ops::math> math2 = "math2";
const x3::rule<class math_pow_class, ast::expr_ops::math> math_pow = "math_pow";
const x3::rule<class mathop2_class, ast::expr_ops::math_op> mathop2 = "mathop2";
const x3::rule<class single_var_name_helper_class, ast::string_t> single_var_name_helper = "single_var_name_helper";
const x3::rule<class dict_item_class, ast::expr_ops::dict_item> dict_item = "dict_item";
const x3::rule<class lvalue_class, ast::expr_ops::lvalue> lvalue = "lvalue";
const x3::rule<class cmp_op_class, ast::expr_ops::cmp_op> cmp_op = "cmp_op";
const x3::rule<class logic_op_class, ast::expr_ops::logic_op> logic_op = "logic_op";
const x3::rule<class point_element_class, ast::expr_ops::point_element> point_element = "point_element";
const x3::rule<class point_right_element_class, ast::expr_ops::point_element> point_element_right = "point_right_element";

const x3::rule<class expr_math_class, ast::expr_ops::expr> expr_math = "expr_math";
const x3::rule<class expr_math_right_class, ast::expr_ops::expr> expr_math_right = "expr_math_right";
const x3::rule<class expr_math2_class, ast::expr_ops::expr> expr_math2 = "expr_math";
const x3::rule<class expr_math_pow_class, ast::expr_ops::expr> expr_math_pow = "expr_math";
const x3::rule<class expr_concat_left_right_class, ast::expr_ops::expr> expr_concat_left = "expr_concat_left";
const x3::rule<class expr_concat_right_class, ast::expr_ops::expr> expr_concat_right = "expr_concat_right";
const x3::rule<class expr_cmp_check_class, ast::expr_ops::expr> expr_cmp_check = "expr_cmp_check";
const x3::rule<class expr_logic_check_class, ast::expr_ops::expr> expr_logic_check = "expr_logic_check";
const x3::rule<class expr_op_if_class, ast::expr_ops::expr> expr_op_if = "expr_op_if";
const x3::rule<class expr_in_pan_class, ast::expr_ops::expr> expr_in_pan = "expr_in_pan";
const x3::rule<class expr_filter_class, ast::expr_ops::expr> expr_filter = "expr_filter";

BOOST_SPIRIT_DECLARE(decltype(math1));
BOOST_SPIRIT_DECLARE(decltype(math2));
BOOST_SPIRIT_DECLARE(decltype(math_pow));
BOOST_SPIRIT_DECLARE(decltype(mathop2));
BOOST_SPIRIT_DECLARE(decltype(single_var_name_helper));
BOOST_SPIRIT_DECLARE(decltype(dict_item));
BOOST_SPIRIT_DECLARE(decltype(lvalue));

BOOST_SPIRIT_DECLARE(decltype(expr_math));
BOOST_SPIRIT_DECLARE(decltype(expr_concat_right));



using boost::spirit::x3::lit;

auto const quoted_string_1_def = *(x3::char_ >> !lit('\'') | lit("\\'") >> x3::attr('\'')) >> x3::char_;
auto const quoted_string_2_def = *(x3::char_ >> !lit('"') | lit("\\\"") >> x3::attr('"')) >> x3::char_;
auto const quoted_string_def =
        (x3::lexeme[lit("'") >> -quoted_string_1_def >> lit("'")])
      | (x3::lexeme[lit("\"") >> -quoted_string_2_def >> lit("\"")]);

auto const bool_rule_def = lit("true") >> x3::attr(true) | lit("false") >> x3::attr(false);
auto const term_def = bool_rule | double_ | x3::int64 | quoted_string;
auto const keywords_def = bool_rule|lit("if")|lit("else")|lit("in")|lit("and")|lit("or")|lit("is");
auto const single_var_name_helper_def = x3::lexeme[ !keywords_def >> x3::char_("A-Za-z_") >> *x3::char_("0-9A-Za-z_") ];
auto const single_var_name_def = !keywords_def >> single_var_name_helper;

auto const math_pow_def = expr_math_pow >> lit("**") >> x3::attr(ast::expr_ops::math_op::pow) >> expr_math_pow;

auto const math_def = math1 | math2 | math_pow ;
auto const math1_def = expr_math >> mathop >> expr_math_right;
auto const math2_def = expr_math2 >> mathop2 >> expr_math;
auto const mathop_def =
        lit("+") >> x3::attr(ast::expr_ops::math_op::pls)
      | lit("-") >> x3::attr(ast::expr_ops::math_op::mns)
                    ;
auto const mathop2_def =
        lit("*") >> x3::attr(ast::expr_ops::math_op::mul)
      | lit("/") >> x3::attr(ast::expr_ops::math_op::dev)
      | lit("%") >> x3::attr(ast::expr_ops::math_op::mod)
      | lit("//") >> x3::attr(ast::expr_ops::math_op::trunc_dev)
                    ;

auto const point_element_def = single_var_name | (lit('[') >> expr >> lit(']'));
auto const point_element_right_def = point | (lit('[') >> expr >> lit(']')) | single_var_name;
auto const point_def = point_element >> -lit('.') >> point_element_right ;

auto const list_def = lit("[") >> expr % ',' >> lit("]");
auto const tuple_def = lit("(") >> expr % ',' >> lit(")"); ;
auto const dict_item_def = quoted_string >> lit(':') >> expr;
auto const dict_def = lit("{") >> dict_item % ',' >> lit("}"); ;

auto const lvalue_def = point | single_var_name;
auto const eq_assign_def = lvalue % ',' >> lit('=') >> expr ;
auto const in_assign_def = lvalue % ',' >> lit("in") >> expr ;

auto const concat_def = expr_concat_left >> lit('~') >> expr_concat_right;

auto const in_check_def = expr >> lit("in") >> expr;

auto const cmp_check_def = expr_cmp_check >> cmp_op >> expr_logic_check;
auto const cmp_op_def =
        lit("==") >> x3::attr(ast::expr_ops::cmp_op::eq)
      | lit("is") >> x3::attr(ast::expr_ops::cmp_op::eq)
      | lit("!=") >> x3::attr(ast::expr_ops::cmp_op::neq)
      | lit("<") >> x3::attr(ast::expr_ops::cmp_op::less)
      | lit(">") >> x3::attr(ast::expr_ops::cmp_op::more)
      | lit("<=") >> x3::attr(ast::expr_ops::cmp_op::less_eq)
      | lit(">=") >> x3::attr(ast::expr_ops::cmp_op::more_eq) ;

auto const logic_check_def = expr_logic_check >> logic_op >> expr_op_if;
auto const logic_op_def =
        lit("and") >> x3::attr(ast::expr_ops::logic_op::op_and)
      | lit("or") >> x3::attr(ast::expr_ops::logic_op::op_or) ;

auto const negate_def = lit("!") >> expr_concat_left;

auto const fnc_call_def = lvalue >> lit('(') >> -(expr % ',') >> lit(')');

auto const filter_call_def = lvalue >> -( lit('(') >> -expr % ',' >> lit(')') );
auto const filter_def = expr_filter >> lit('|') >> filter_call % '|';

auto const op_if_def = expr_op_if >> lit("if") >> expr >> -(lit("else") >> expr);

auto const expr_in_pan_def = lit('(') >> expr >> lit(')');
auto const expr_def = eq_assign | op_if | logic_check | cmp_check | filter | list | tuple | dict | concat | math | negate | fnc_call | point | single_var_name | term;
auto const expr_op_if_def =               logic_check | cmp_check | filter | list | tuple | dict | concat | math | negate | fnc_call | point | single_var_name | term | expr_in_pan;
auto const expr_logic_check_def =                       cmp_check | filter | list | tuple | dict | concat | math | negate | fnc_call | point | single_var_name | term | expr_in_pan;
auto const expr_cmp_check_def =                                     filter | list | tuple | dict | concat | math | negate | fnc_call | point | single_var_name | term | expr_in_pan;
auto const expr_filter_def =                                                 list | tuple | dict | concat | math | negate | fnc_call | point | single_var_name | term | expr_in_pan;
auto const expr_math_right_def =                                                                   concat | math | negate | fnc_call | point | single_var_name | term | expr_in_pan;
auto const expr_concat_right_def =                                                                 concat | math | negate | fnc_call | point | single_var_name | term | expr_in_pan;
auto const expr_math_def =                                                                                 math2 | negate | fnc_call | point | single_var_name | term | expr_in_pan;
auto const expr_math2_def =                                                                             math_pow | negate | fnc_call | point | single_var_name | term | expr_in_pan;
auto const expr_concat_left_def =                                                                           math | negate | fnc_call | point | single_var_name | term | expr_in_pan;
auto const expr_math_pow_def =                                                                                              fnc_call | point | single_var_name | term;

BOOST_SPIRIT_DEFINE( math )
BOOST_SPIRIT_DEFINE( math1 )
BOOST_SPIRIT_DEFINE( math2 )
BOOST_SPIRIT_DEFINE( math_pow )
BOOST_SPIRIT_DEFINE( mathop )
BOOST_SPIRIT_DEFINE( mathop2 )
BOOST_SPIRIT_DEFINE( bool_rule )
BOOST_SPIRIT_DEFINE( term )
BOOST_SPIRIT_DEFINE( quoted_string )

BOOST_SPIRIT_DEFINE( single_var_name )
BOOST_SPIRIT_DEFINE( single_var_name_helper );
BOOST_SPIRIT_DEFINE( dict_item );
BOOST_SPIRIT_DEFINE( lvalue );
BOOST_SPIRIT_DEFINE( point )
BOOST_SPIRIT_DEFINE( point_element )
BOOST_SPIRIT_DEFINE( point_element_right )
BOOST_SPIRIT_DEFINE( concat )
BOOST_SPIRIT_DEFINE( in_check )
BOOST_SPIRIT_DEFINE( cmp_check )
BOOST_SPIRIT_DEFINE( cmp_op )
BOOST_SPIRIT_DEFINE( logic_check )
BOOST_SPIRIT_DEFINE( logic_op )
BOOST_SPIRIT_DEFINE( negate )
BOOST_SPIRIT_DEFINE( fnc_call )
BOOST_SPIRIT_DEFINE( filter_call )
BOOST_SPIRIT_DEFINE( filter )
BOOST_SPIRIT_DEFINE( expr_filter )
BOOST_SPIRIT_DEFINE( op_if )

BOOST_SPIRIT_DEFINE( list )
BOOST_SPIRIT_DEFINE( tuple )
BOOST_SPIRIT_DEFINE( dict )
BOOST_SPIRIT_DEFINE( eq_assign )
BOOST_SPIRIT_DEFINE( in_assign )

BOOST_SPIRIT_DEFINE( expr )
BOOST_SPIRIT_DEFINE( expr_math )
BOOST_SPIRIT_DEFINE( expr_math_right )
BOOST_SPIRIT_DEFINE( expr_math2 )
BOOST_SPIRIT_DEFINE( expr_math_pow )
BOOST_SPIRIT_DEFINE( expr_concat_right )
BOOST_SPIRIT_DEFINE( expr_concat_left )
BOOST_SPIRIT_DEFINE( expr_cmp_check )
BOOST_SPIRIT_DEFINE( expr_logic_check )
BOOST_SPIRIT_DEFINE( expr_op_if )
BOOST_SPIRIT_DEFINE( expr_in_pan )

} // namespace cppjinja::text::expr_ops
