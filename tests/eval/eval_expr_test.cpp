/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE evaluator expr

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "mocks.hpp"
#include "evtree/exenv/expr_eval.hpp"
#include "parser/grammar/expr.hpp"
#include "parser/parse.hpp"

using namespace std::literals;


BOOST_AUTO_TEST_SUITE(phase_evaluate)
using mocks::mock_exenv_fixture;
BOOST_AUTO_TEST_SUITE(expr)
namespace txt = cppjinja::text;
namespace ext = cppjinja::text::expr_ops;
namespace est = cppjinja::ast::expr_ops;
using eeval = cppjinja::evt::expr_eval;
std::string to_str(const absd::data& d)
{
	std::stringstream out;
	absd::to_json_printer{out}(d);
	return out.str();
}
BOOST_AUTO_TEST_SUITE(term)
BOOST_FIXTURE_TEST_CASE(string,  mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "'ok'"))->solve();
	BOOST_TEST(res == "ok"s);
	res = eeval(&env)(txt::parse(ext::expr, "1"))->solve();
	BOOST_TEST(res == 1);
}
BOOST_FIXTURE_TEST_CASE(math,  mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "3+5-1*0.5"))->solve();
	BOOST_TEST(res == 7.5);
	eeval str_e(&env);
	BOOST_CHECK_THROW(str_e(txt::parse(ext::expr, "0.5+'1'")), std::runtime_error);

	res = eeval(&env)(txt::parse(ext::expr, "'ok' + 'ok'"))->solve();
	BOOST_TEST(res == "okok"s);

	eeval str2_e(&env);
	BOOST_CHECK_THROW(str2_e(txt::parse(ext::expr, "'ok'-'ups'")), std::runtime_error);
}
BOOST_FIXTURE_TEST_CASE(concat, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "3~5~10*2"))->solve();
	BOOST_TEST(res == "3520"s);
}
BOOST_FIXTURE_TEST_CASE(negate, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "!!true"))->solve();
	BOOST_TEST(res == true);
	eeval str_e(&env);
	BOOST_CHECK_THROW(str_e(txt::parse(ext::expr, "!'1'")), std::runtime_error);
}
BOOST_FIXTURE_TEST_CASE(cmp_check, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "1 == 1"))->solve();
	BOOST_TEST(res == true);
	res = eeval(&env)(txt::parse(ext::expr, "1 < 2"))->solve();
	BOOST_TEST(res == true);
	res = eeval(&env)(txt::parse(ext::expr, "'cc' < 'aa'"))->solve();
	BOOST_TEST(res == false);
}
BOOST_FIXTURE_TEST_CASE(in_check, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr_bool, "1 in [1, 2]"));
	BOOST_TEST(res == true);
	res = eeval(&env)(txt::parse(ext::expr_bool, "1 in [2, 3]"));
	BOOST_TEST(res == false);
	res = eeval(&env)(txt::parse(ext::expr_bool, "'cc' in {'aa':1, 'cc':2}"));
	BOOST_TEST(res == true);
}
BOOST_FIXTURE_TEST_CASE(logic_check, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "true and true"))->solve();
	BOOST_TEST(res == true);
	res = eeval(&env)(txt::parse(ext::expr, "true and false"))->solve();
	BOOST_TEST(res == false);
	res = eeval(&env)(txt::parse(ext::expr, "'a' and 'b'"))->solve();
	BOOST_TEST(res == true);
//	res = eeval(&env)(txt::parse(ext::expr, "'' and ''"))->solve();
//	BOOST_TEST(res == true);
	res = eeval(&env)(txt::parse(ext::expr, "'b' and ''"))->solve();
	BOOST_TEST(res == false);
}
BOOST_FIXTURE_TEST_CASE(op_if, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "'ok' if 1==1 else 'not ok'"))->solve();
	BOOST_TEST(res == "ok");
	res = eeval(&env)(txt::parse(ext::expr, "'not ok' if 1==2 else 'ok'"))->solve();
	BOOST_TEST(res == "ok");
	res = eeval(&env)(txt::parse(ext::expr, "'not ok' if 1==2"))->solve();
	BOOST_TEST(res == "");
}
BOOST_FIXTURE_TEST_CASE(array, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "[1, 3, 5]"))->solve();
	BOOST_TEST(to_str(res) == "[1,3,5]");
}
BOOST_FIXTURE_TEST_CASE(tuple, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "(1, 3, 'ok')"))->solve();
	BOOST_TEST(to_str(res) == "[1,3,\"ok\"]");
}
BOOST_AUTO_TEST_SUITE_END() // term
BOOST_AUTO_TEST_SUITE(reduce)
BOOST_FIXTURE_TEST_CASE(dict, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "{'name':1,'name2':'value'}"))->solve();
	BOOST_TEST(to_str(res) == "{\"name\":1,\"name2\":\"value\"}");
}
BOOST_FIXTURE_TEST_CASE(single_var, mock_exenv_fixture)
{
	auto a = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"a"}).returns(a);
	auto res = eeval(&env)(txt::parse(ext::expr, "a"));
	BOOST_TEST(res.get() == a.get());
}

BOOST_AUTO_TEST_SUITE(point)
BOOST_FIXTURE_TEST_CASE(call_at_end, mock_exenv_fixture)
{
	auto a = std::make_shared<mocks::context_object>();
	auto b = std::make_shared<mocks::context_object>();
	auto c = std::make_shared<mocks::context_object>();
	auto abe = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"a"}).returns(a);
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"c"}).returns(c);
	MOCK_EXPECT(a->find).with(cppjinja::east::var_name{"b"}).returns(b);
	MOCK_EXPECT(b->find).with(cppjinja::east::var_name{"e"}).returns(abe);
	expect_solve(*c, "e");
	auto res = eeval(&env)(txt::parse(ext::expr, "a.b[c]"));
	BOOST_TEST(res.get() == abe.get());
}
BOOST_FIXTURE_TEST_CASE(two_calls, mock_exenv_fixture)
{
	auto a = std::make_shared<mocks::context_object>();
	auto b = std::make_shared<mocks::context_object>();
	auto c = std::make_shared<mocks::context_object>();
	auto abe = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"a"}).returns(a);
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"c"}).returns(c);
	expect_solve(*c, "e");
	MOCK_EXPECT(a->find).with(cppjinja::east::var_name{"b"}).returns(b);
	MOCK_EXPECT(b->find).with(cppjinja::east::var_name{"e"}).returns(abe);
	auto res = eeval(&env)(txt::parse(ext::expr, "a['b'][c]"));
	BOOST_TEST(res.get() == abe.get());
}
BOOST_AUTO_TEST_SUITE_END() // point

BOOST_AUTO_TEST_SUITE(fnc_call)
BOOST_FIXTURE_TEST_CASE(without_args, mock_exenv_fixture)
{
	std::pmr::vector<cppjinja::evt::context_object::function_parameter> params;
	auto a = std::make_shared<mocks::context_object>();
	auto call = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"a"}).returns(a);
	MOCK_EXPECT(a->call).with(params).returns(call);
	auto res = eeval(&env)(txt::parse(ext::expr, "a()"));
	BOOST_TEST(res.get() == call.get());
}
BOOST_FIXTURE_TEST_CASE(with_args, mock_exenv_fixture)
{
	auto a = std::make_shared<mocks::context_object>();
	auto call = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"a"}).returns(a);
	MOCK_EXPECT(a->call).calls([call](std::pmr::vector<cppjinja::evt::context_object::function_parameter> params){
		BOOST_TEST(params.size() == 1);
		BOOST_TEST(params.at(0).name.has_value() == false);
		BOOST_REQUIRE(params.at(0).value);
		BOOST_TEST(params.at(0).value->solve() == "ok");
		return call;
	});

	auto res = eeval(&env)(txt::parse(ext::expr, "a('ok')"));
	BOOST_TEST(res.get() == call.get());
}
BOOST_FIXTURE_TEST_CASE(points, mock_exenv_fixture)
{
	auto p1_val = std::make_shared<mocks::context_object>();
	std::pmr::vector<cppjinja::evt::context_object::function_parameter> params;
	params.emplace_back(cppjinja::evt::context_object::function_parameter{
	                        std::nullopt, p1_val});
	expect_solve(*p1_val, "ok");

	auto a = std::make_shared<mocks::context_object>();
	auto b = std::make_shared<mocks::context_object>();
	auto c = std::make_shared<mocks::context_object>();
	auto d = std::make_shared<mocks::context_object>();
	auto call = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"a"s}).returns(a);
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"c"s}).returns(c);
	MOCK_EXPECT(a->find).with(cppjinja::east::var_name{"b"}).returns(b);
	MOCK_EXPECT(c->find).with(cppjinja::east::var_name{"d"}).returns(d);
	MOCK_EXPECT(b->call).with(params).returns(call);
	expect_solve(*d, "ok");

	auto res = eeval(&env)(txt::parse(ext::expr, "a.b(c.d)"));
	BOOST_TEST(res.get() == call.get());
}
BOOST_AUTO_TEST_SUITE_END() // fnc_call

BOOST_AUTO_TEST_SUITE(filter_call)
BOOST_FIXTURE_TEST_CASE(without_args, mock_exenv_fixture)
{
	auto a = std::make_shared<mocks::context_object>();
	auto b = std::make_shared<mocks::context_object>();
	auto result = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"a"}).returns(a);
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"b"}).returns(b);

	MOCK_EXPECT(b->call).calls([result,a](std::pmr::vector<cppjinja::evt::context_object::function_parameter> params){
		BOOST_TEST_REQUIRE(params.size() == 1);
		BOOST_TEST(*params[0].name == "$");
		BOOST_TEST(params[0].value == a);
		return result;
	});

	auto res = eeval(&env)(txt::parse(ext::expr, "a | b"));
	BOOST_TEST(res == result);
}
BOOST_FIXTURE_TEST_CASE(without_args_few, mock_exenv_fixture)
{
	using fparam_t = cppjinja::evt::context_object::function_parameter;
	auto a = std::make_shared<mocks::context_object>();
	auto b = std::make_shared<mocks::context_object>();
	auto c = std::make_shared<mocks::context_object>();
	auto result_b = std::make_shared<mocks::context_object>();
	auto result_c = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"a"}).returns(a);
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"b"}).returns(b);
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"c"}).returns(c);

	MOCK_EXPECT(b->call).calls([result_b,a](std::pmr::vector<fparam_t> params){
		BOOST_TEST_REQUIRE(params.size() == 1);
		BOOST_TEST(*params[0].name == "$");
		BOOST_TEST(params[0].value == a);
		return result_b;
	});
	MOCK_EXPECT(c->call).calls([result_b,result_c,a,b](std::pmr::vector<fparam_t> params){
		BOOST_TEST_REQUIRE(params.size() == 1);
		BOOST_TEST(*params[0].name == "$");
		BOOST_TEST(params[0].value == result_b);
		return result_c;
	});

	auto res = eeval(&env)(txt::parse(ext::expr, "a | b | c"));
	BOOST_TEST(res == result_c);
}
BOOST_FIXTURE_TEST_CASE(with_args, mock_exenv_fixture)
{
	auto a = std::make_shared<mocks::context_object>();
	auto b = std::make_shared<mocks::context_object>();
	auto c = std::make_shared<mocks::context_object>();
	auto result = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"a"}).returns(a);
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"b"}).returns(b);
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"c"}).returns(c);

	MOCK_EXPECT(b->call).calls([result,a,c](std::pmr::vector<cppjinja::evt::context_object::function_parameter> params){
		BOOST_TEST_REQUIRE(params.size() == 2);
		BOOST_TEST(*params[0].name == "$");
		BOOST_TEST(params[0].value == a);
		BOOST_TEST(params[1].value == c);
		return result;
	});

	auto res = eeval(&env)(txt::parse(ext::expr, "a | b(c)"));
	BOOST_TEST(res == result);
}
BOOST_AUTO_TEST_SUITE_END() // filter_call

BOOST_FIXTURE_TEST_CASE(expr_bool, mock_exenv_fixture)
{
	BOOST_TEST(eeval(&env)(txt::parse(ext::expr_bool, "'a'")) == true);
	BOOST_TEST(eeval(&env)(txt::parse(ext::expr_bool, "''")) == false);
	BOOST_TEST(eeval(&env)(txt::parse(ext::expr_bool, "false")) == false);
	BOOST_TEST(eeval(&env)(txt::parse(ext::expr_bool, "true")) == true);
	BOOST_TEST(eeval(&env)(txt::parse(ext::expr_bool, "1")) == true);
	BOOST_TEST(eeval(&env)(txt::parse(ext::expr_bool, "0.0")) == false);
}

BOOST_AUTO_TEST_SUITE_END() // reduce

BOOST_AUTO_TEST_SUITE(tests)
BOOST_FIXTURE_TEST_CASE(wihout_args, mock_exenv_fixture)
{
	auto tests = std::make_shared<mocks::context_object>();
	auto int_check = std::make_shared<mocks::context_object>();
	auto check_result = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"$tests"s}).returns(tests);
	MOCK_EXPECT(tests->find)
	        .exactly(2)
	        .with(cppjinja::east::var_name{"integer"s})
	        .returns(int_check);
	MOCK_EXPECT(int_check->call).returns(check_result);
	MOCK_EXPECT(check_result->solve).once().returns(create_absd_data(true));
	BOOST_TEST(eeval(&env)(txt::parse(ext::expr, "7 if 2 is integer"))->solve() == 7);
	MOCK_EXPECT(check_result->solve).once().returns(create_absd_data(false));
	BOOST_TEST(eeval(&env)(txt::parse(ext::expr, "7 if 2 is integer else 5"))->solve() == 5);
}
BOOST_AUTO_TEST_SUITE_END() // tests

BOOST_AUTO_TEST_SUITE_END() // expr
BOOST_AUTO_TEST_SUITE_END() // phase_evaluate
