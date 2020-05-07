/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE rules_expr

#include <sstream>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parser/parse.hpp"
#include "parser/ast/expr.hpp"
#include "parser/operators/expr.hpp"
#include "parser/grammar/expr.hpp"

using namespace std::literals;

namespace ut = boost::unit_test;
namespace utd = boost::unit_test::data;
namespace ast = cppjinja::ast;
namespace est = cppjinja::ast::expr_ops;
namespace txt = cppjinja::text;
namespace ext = cppjinja::text::expr_ops;

BOOST_AUTO_TEST_SUITE(phase_parse)
BOOST_AUTO_TEST_SUITE(expr_ops)
BOOST_AUTO_TEST_CASE(qutoed_string)
{
	BOOST_TEST(txt::parse(ext::quoted_string, "'a\"'"sv) == "a\""s);
	BOOST_TEST(txt::parse(ext::quoted_string, "\"a'\""sv) == "a'"s);
}
BOOST_AUTO_TEST_CASE(bool_rule)
{
	BOOST_TEST(txt::parse(ext::bool_rule, "true"sv) == true);
	BOOST_TEST(txt::parse(ext::bool_rule, "false"sv) == false);
}
BOOST_AUTO_TEST_CASE(term)
{
	BOOST_TEST(txt::parse(ext::term, "1"sv).var == est::term{1}.var);
	BOOST_TEST(txt::parse(ext::term, "1"sv).var.type().name() == typeid(std::int64_t).name());
	BOOST_TEST(txt::parse(ext::term, "1.1"sv).var == est::term{1.1}.var);
	BOOST_TEST(txt::parse(ext::term, "true"sv).var == est::term{true}.var);
	BOOST_TEST(txt::parse(ext::term, "'ok'"sv).var == est::term{"ok"s}.var);
	BOOST_TEST(txt::parse(ext::expr, "true"sv) == est::expr{est::term{true}});
}
BOOST_AUTO_TEST_CASE(single_var_name)
{
	BOOST_TEST(txt::parse(ext::single_var_name, "a"sv) == est::single_var_name{"a"s});
	BOOST_TEST(txt::parse(ext::expr, "a"sv) == est::expr{est::single_var_name{"a"s}});
}
BOOST_AUTO_TEST_CASE(math)
{
	auto op = est::math_op::mns;
	est::math result = txt::parse(ext::math, "5 - 7"sv);
	BOOST_TEST(result.op == op);
	BOOST_TEST(result.left.get() == est::expr{est::term{5}});
	BOOST_TEST(result.right.get() == est::expr{est::term{7}});

	est::math result2 = txt::parse(ext::math, "3 + 5 - 7"sv);
	BOOST_TEST(result2.op == est::math_op::pls);
	BOOST_TEST(result2.left.get() == est::expr{est::term{3}});
	BOOST_TEST(result2.right.get() == est::expr{result});

	est::math result4 = txt::parse(ext::math, "7**1 * 3 + 11**5"sv);
	BOOST_TEST(result4.op == est::math_op::pls);
	est::math& result4_right = boost::get<est::math>(result4.right.get().var);
	BOOST_TEST(result4_right.op == est::math_op::pow);

	est::math result3 = txt::parse(ext::math, "(5 - 7) * 3 + 11"sv);
	BOOST_TEST(result3.op == est::math_op::pls);
	BOOST_TEST(result3.left.get().var.type().name() == typeid(est::math).name());
	BOOST_TEST(result3.right.get() == est::expr{est::term{11}});
	est::math& result3_left = boost::get<est::math>(result3.left.get().var);
	BOOST_TEST(result3_left.op == est::math_op::mul);
	BOOST_TEST(result3_left.left.get() == est::expr{result});
	BOOST_TEST(result3_left.right.get() == est::expr{est::term{3}});
}
BOOST_AUTO_TEST_CASE(point)
{
	auto result = txt::parse(ext::point, "a.b");
	BOOST_TEST(result.left == est::expr{est::single_var_name{"a"s}});

	auto result2 = txt::parse(ext::point, "c.a.b");
	BOOST_TEST(result2.left.get() == est::expr{est::single_var_name{"c"s}});
	BOOST_TEST(result2.right.get() == est::expr{result});

	BOOST_TEST(txt::parse(ext::expr, "c.a.b") == est::expr{result2});

	result = txt::parse(ext::point, "a['b']");
	BOOST_TEST(result.left == est::expr{est::single_var_name{"a"s}});
	BOOST_TEST(result.right == est::expr{est::term{"b"s}});
}
BOOST_AUTO_TEST_CASE(list)
{
	auto result = txt::parse(ext::list, "['a', b]");
	BOOST_TEST(result.items.size() == 2);
	BOOST_TEST(txt::parse(ext::list, "[c, ['a', b]]").items[1] == est::expr{result});
}
BOOST_AUTO_TEST_CASE(tuple)
{
	auto result = txt::parse(ext::tuple, "('a', b)");
	BOOST_TEST(result.items.size() == 2);
	BOOST_TEST(txt::parse(ext::tuple, "(c, ('a', b))").items[1] == est::expr{result});
}
BOOST_AUTO_TEST_CASE(dict)
{
	auto result = txt::parse(ext::dict, "{'a':1, 'b':10 + 2}");
	BOOST_TEST(result.items.size() == 2);
	BOOST_TEST(result.items[1].name == "b"s);
	BOOST_TEST(result.items[1].value == txt::parse(ext::expr, "10+2"sv));
}
BOOST_AUTO_TEST_CASE(assign)
{
	auto result = txt::parse(ext::eq_assign, "a = 10");
	BOOST_TEST(result.names.size() == 1);
	BOOST_TEST(result.names.front() == est::lvalue{est::single_var_name{"a"s}});
	auto in_result = txt::parse(ext::in_assign, "a in 10");
	BOOST_TEST(in_result == result);
}
BOOST_AUTO_TEST_CASE(concat)
{
	auto result_bc = txt::parse(ext::concat, "'b' ~ c");
	BOOST_TEST(result_bc.left == est::expr{est::term{"b"s}});
	BOOST_TEST(result_bc.right == est::expr{est::single_var_name{"c"s}});
	auto result = txt::parse(ext::concat, "a ~ 'b' ~ c");
	BOOST_TEST(result.left == est::expr{est::single_var_name{"a"s}});
	BOOST_TEST(result.right == est::expr{result_bc});
}
BOOST_AUTO_TEST_CASE(in_check)
{
	auto result = txt::parse(ext::in_check, "a in ['b', 'c']");
	BOOST_TEST(result.term.get() == est::expr{est::single_var_name{"a"}});
	BOOST_TEST(result.object.get().var.type().name() == typeid(est::list).name());
}
BOOST_AUTO_TEST_CASE(cmp_check)
{
	auto result = txt::parse(ext::cmp_check, "1 < b"sv);
	BOOST_TEST(result.op == est::cmp_op::less);
	BOOST_TEST(result.left == est::expr{est::term{1}});
	BOOST_TEST(result.right == est::expr{est::single_var_name{"b"s}});
}
BOOST_AUTO_TEST_CASE(logic_check)
{
	auto result = txt::parse(ext::logic_check, "1 and 2");
	BOOST_TEST(result.op == est::logic_op::op_and);
	BOOST_TEST(result.left == est::expr{est::term{1}});
}
BOOST_AUTO_TEST_CASE(negate)
{
	auto result = txt::parse(ext::negate, "!b");
	BOOST_TEST(result.arg == est::expr{est::single_var_name{"b"s}});
}
BOOST_AUTO_TEST_CASE(fnc_call)
{
	auto result1 = txt::parse(ext::fnc_call, "a.b()");
	BOOST_TEST(result1.ref == est::expr{txt::parse(ext::point, "a.b")});
	BOOST_TEST(result1.args.size() == 0);
	auto result2 = txt::parse(ext::fnc_call, "a.b('v')");
	BOOST_TEST(result2.ref == result1.ref);
	BOOST_TEST(result2.args.size() == 1);
	BOOST_TEST(result2.args[0].get() == est::expr{est::term{"v"s}});
	auto result3 = txt::parse(ext::fnc_call, "a.b('v', a='va')");
	BOOST_TEST(result3.ref == result1.ref);
	BOOST_TEST(result3.args.size() == 2);
	auto result4 = txt::parse(ext::fnc_call, "a.b(a='va')");
	BOOST_TEST(result4.ref == result1.ref);
	BOOST_TEST(result4.args.size() == 1);
}
BOOST_AUTO_TEST_CASE(filter)
{
	auto result1 = txt::parse(ext::filter, "a.b | f1");
	BOOST_TEST(result1.base == est::expr{txt::parse(ext::point, "a.b")});
	BOOST_TEST(result1.filters.size() == 1);
	BOOST_TEST(result1.filters.at(0).ref == est::expr{est::single_var_name{"f1"s}});
	auto result2 = txt::parse(ext::filter, "a.b | f1 | f2 | f3(c)");
	BOOST_TEST(result2.base == est::expr{txt::parse(ext::point, "a.b")});
	BOOST_TEST(result2.filters.size() == 3);
	BOOST_TEST(result2.filters.at(0) == result1.filters.at(0));
}
BOOST_AUTO_TEST_CASE(op_if)
{
	auto result1 = txt::parse(ext::op_if, "a.b if c else 'd'");
	BOOST_TEST(result1.term == est::expr{txt::parse(ext::point, "a.b")});
}

BOOST_AUTO_TEST_SUITE(exprs)
BOOST_AUTO_TEST_CASE(math)
{
	auto result = txt::parse(ext::expr, "3 + 5 * 7"sv);
	BOOST_TEST(result.var.type().name() == typeid(est::math).name());
	est::math* res = &boost::get<est::math>(result.var);
	BOOST_TEST(res->right.get().var.type().name() == typeid(est::math).name());

	auto result2 = txt::parse(ext::expr, "a.b + 5 * 7"sv);
	BOOST_TEST(result.var.type().name() == typeid(est::math).name());
	res = &boost::get<est::math>(result.var);
	BOOST_TEST(res->op == est::math_op::pls);
}
BOOST_AUTO_TEST_CASE(point)
{
	auto result = txt::parse(ext::expr, "c.d + 5 * a['b']"sv);
	BOOST_TEST(result.var.type().name() == typeid(est::math).name());
	est::math* res = &boost::get<est::math>(result.var);
	BOOST_TEST(res->right.get().var.type().name() == typeid(est::math).name());
	res = &boost::get<est::math>(res->right.get().var);
	BOOST_TEST(res->right.get().var.type().name() == typeid(est::point).name());
}
BOOST_AUTO_TEST_CASE(assign)
{
	auto result = txt::parse(ext::expr, "c,d = 5 * a['b']"sv);
	BOOST_TEST(result.var.type().name() == typeid(est::eq_assign).name());
}
BOOST_AUTO_TEST_CASE(concat)
{
	auto result = txt::parse(ext::expr, "'a' * 'b' ~ c.d");
	BOOST_TEST(result.var.type().name() == typeid(est::math).name());
	est::math* res = &boost::get<est::math>(result.var);
	BOOST_TEST(res->left.get().var.type().name() == typeid(est::term).name());
	BOOST_TEST(res->right.get().var.type().name() == typeid(est::concat).name());
}
BOOST_AUTO_TEST_CASE(cmp_check)
{
	auto result = txt::parse(ext::expr, "e = 'a' * 'b' ~ c.d < b");
	BOOST_TEST(result.var.type().name() == typeid(est::eq_assign).name());
	est::eq_assign* res = &boost::get<est::eq_assign>(result.var);
	BOOST_TEST(res->value.get().var.type().name() == typeid(est::cmp_check).name());

	result = txt::parse(ext::expr, "e = 'a' + 'b' < b");
	BOOST_TEST(result.var.type().name() == typeid(est::eq_assign).name());
	res = &boost::get<est::eq_assign>(result.var);
	BOOST_TEST(res->value.get().var.type().name() == typeid(est::cmp_check).name());
}
BOOST_AUTO_TEST_CASE(logic_check)
{
	auto result = txt::parse(ext::expr, "z = 'a' * 'b' ~ c.d < b and !f");
	BOOST_TEST(result.var.type().name() == typeid(est::eq_assign).name());
	est::eq_assign* res = &boost::get<est::eq_assign>(result.var);
	BOOST_TEST(res->value.get().var.type().name() == typeid(est::logic_check).name());
	est::logic_check* right = &boost::get<est::logic_check>(res->value.get().var);
	BOOST_TEST(right->right.get().var.type().name() == typeid(est::negate).name());
}
BOOST_AUTO_TEST_CASE(fnc_call)
{
	auto result = txt::parse(ext::expr, "z = 'a' * 'b' ~ c.d('v') < b and !f('v')");
	BOOST_TEST(result.var.type().name() == typeid(est::eq_assign).name());
	est::eq_assign* res = boost::get<est::eq_assign>(&result.var);
	BOOST_REQUIRE(res);
	BOOST_TEST(res->value.get().var.type().name() == typeid(est::logic_check).name());
	est::logic_check* right = boost::get<est::logic_check>(&res->value.get().var);
	BOOST_REQUIRE(right);
	BOOST_TEST(right->right.get().var.type().name() == typeid(est::negate).name());
	est::negate* right_neg = boost::get<est::negate>(&right->right.get().var);
	BOOST_REQUIRE(right_neg);
	BOOST_TEST(right_neg->arg.get().var.type().name() == typeid(est::fnc_call).name());
}
BOOST_AUTO_TEST_CASE(op_if)
{
	auto left_result = txt::parse(ext::expr, "z = 'a' * 'b' ~ c.d('v') < b and !f('v')"sv);
	est::eq_assign* left_res = boost::get<est::eq_assign>(&left_result.var);

	auto result = txt::parse(ext::expr, "z = 'a' * 'b' ~ c.d('v') < b and !f('v') if c"sv);
	BOOST_TEST(result.var.type().name() == typeid(est::eq_assign).name());
	est::eq_assign* res = boost::get<est::eq_assign>(&result.var);
	BOOST_REQUIRE(res);

	BOOST_TEST(res->value.get().var.type().name() == typeid(est::op_if).name());
	est::op_if* right = boost::get<est::op_if>(&res->value.get().var);
	BOOST_REQUIRE(right);
	BOOST_TEST(right->term.get() == left_res->value.get());
}
BOOST_AUTO_TEST_SUITE_END() // exprs

BOOST_AUTO_TEST_SUITE_END() // expr_ops
BOOST_AUTO_TEST_SUITE_END() // phase_parse
