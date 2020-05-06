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
const x3::rule<class math3_class, ast::expr_ops::math> math3 = "math3";
const x3::rule<class mathop2_class, ast::expr_ops::math_op> mathop2 = "mathop2";
const x3::rule<class single_var_name_helper_class, ast::string_t> single_var_name_helper = "single_var_name_helper";
const x3::rule<class dict_item_class, ast::expr_ops::dict_item> dict_item = "dict_item";
const x3::rule<class lvalue_class, ast::expr_ops::lvalue> lvalue = "lvalue";

const x3::rule<class expr_math_class, ast::expr_ops::expr> expr_math = "expr_math";
const x3::rule<class expr_math2_class, ast::expr_ops::expr> expr_math2 = "expr_math";
const x3::rule<class expr_math3_class, ast::expr_ops::expr> expr_math3 = "expr_math";
const x3::rule<class expr_point_class, ast::expr_ops::expr> expr_point = "expr_point";
const x3::rule<class expr_concat_class, ast::expr_ops::expr> expr_concat = "expr_concat";

BOOST_SPIRIT_DECLARE(decltype(math1));
BOOST_SPIRIT_DECLARE(decltype(math2));
BOOST_SPIRIT_DECLARE(decltype(math3));
BOOST_SPIRIT_DECLARE(decltype(mathop2));
BOOST_SPIRIT_DECLARE(decltype(single_var_name_helper));
BOOST_SPIRIT_DECLARE(decltype(dict_item));
BOOST_SPIRIT_DECLARE(decltype(lvalue));

BOOST_SPIRIT_DECLARE(decltype(expr_math));
BOOST_SPIRIT_DECLARE(decltype(expr_point));
BOOST_SPIRIT_DECLARE(decltype(expr_concat));



using boost::spirit::x3::lit;

auto print_clear = [](const auto& v) { std::cout << "cl: " <<  v << std::endl;};
auto print = [](const auto& vv){
	auto v = x3::_attr(vv);
	std::cout << "v == " << typeid(decltype(v)).name() << " | ";
	boost::apply_visitor(print_clear, v.var);
	x3::_val(vv) = v; };

auto const quoted_string_1_def = *(x3::char_ >> !lit('\'') | lit("\\'") >> x3::attr('\'')) >> x3::char_;
auto const quoted_string_2_def = *(x3::char_ >> !lit('"') | lit("\\\"") >> x3::attr('"')) >> x3::char_;
auto const quoted_string_def =
        (x3::lexeme[lit("'") >> -quoted_string_1_def >> lit("'")])
      | (x3::lexeme[lit("\"") >> -quoted_string_2_def >> lit("\"")]);

auto const bool_rule_def = lit("true") >> x3::attr(true) | lit("false") >> x3::attr(false);
auto const term_def = bool_rule | (!single_var_name >> double_) | x3::int64 | quoted_string;
auto const single_var_name_helper_def = x3::lexeme[ !bool_rule >> x3::char_("A-Za-z_") >> *x3::char_("0-9A-Za-z_") ];

auto const math_def = math1 | math2 | math3 ;
auto const math1_def = expr_math >> mathop >> expr;
auto const math2_def = expr_math2 >> mathop2 >> expr_math;
auto const math3_def = expr_math3 >> lit("**") >> x3::attr(ast::expr_ops::math_op::pow) >> expr_math3;
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
auto const expr_math_def =  math2 | math3 | concat | point | term | (lit('(') >> expr >> lit(')'));
auto const expr_math2_def =  math3 | concat | point | term | (lit('(') >> expr >> lit(')'));
auto const expr_math3_def =  term | point;

auto const point_def = expr_point >> ((lit('[') >> expr >> lit(']')) | (lit('.') >> expr));
auto const single_var_name_def = single_var_name_helper;
auto const expr_point_def = single_var_name | term | (lit('(') >> expr >> lit(')'));

auto const list_def = lit("[") >> expr % ',' >> lit("]");
auto const tuple_def = lit("(") >> expr % ',' >> lit(")"); ;
auto const dict_item_def = quoted_string >> lit(':') >> expr;
auto const dict_def = lit("{") >> dict_item % ',' >> lit("}"); ;

auto const lvalue_def = point | single_var_name;
auto const eq_assign_def = lvalue % ',' >> lit('=') >> expr ;
auto const in_assign_def = lvalue % ',' >> lit("in") >> expr ;

auto const concat_def = expr_concat >> '~' >> expr_concat;
auto const expr_concat_def = point | single_var_name | term | (lit('(') >> expr >> lit(')'));

auto const in_check_def = expr >> lit("in") >> expr;

auto const expr_def = eq_assign | list | tuple | math | concat | point | single_var_name | term;

BOOST_SPIRIT_DEFINE( math )
BOOST_SPIRIT_DEFINE( math1 )
BOOST_SPIRIT_DEFINE( math2 )
BOOST_SPIRIT_DEFINE( math3 )
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
BOOST_SPIRIT_DEFINE( concat )
BOOST_SPIRIT_DEFINE( in_check )

BOOST_SPIRIT_DEFINE( list )
BOOST_SPIRIT_DEFINE( tuple )
BOOST_SPIRIT_DEFINE( dict )
BOOST_SPIRIT_DEFINE( eq_assign )
BOOST_SPIRIT_DEFINE( in_assign )

BOOST_SPIRIT_DEFINE( expr )
BOOST_SPIRIT_DEFINE( expr_math )
BOOST_SPIRIT_DEFINE( expr_math2 )
BOOST_SPIRIT_DEFINE( expr_math3 )
BOOST_SPIRIT_DEFINE( expr_point )
BOOST_SPIRIT_DEFINE( expr_concat )

} // namespace cppjinja::text::expr_ops
