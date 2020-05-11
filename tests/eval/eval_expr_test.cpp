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
using cval = cppjinja::east::value_term;
BOOST_AUTO_TEST_SUITE(term)
BOOST_FIXTURE_TEST_CASE(string,  mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "'ok'"))->solve();
	BOOST_TEST(res == cval{"ok"s});
	res = eeval(&env)(txt::parse(ext::expr, "1"))->solve();
	BOOST_TEST(res == cval{1});
}
BOOST_FIXTURE_TEST_CASE(math,  mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "3+5-1*0.5"))->solve();
	BOOST_TEST(res == cval{7.5});
	eeval str_e(&env);
	BOOST_CHECK_THROW(str_e(txt::parse(ext::expr, "0.5+'1'")), std::runtime_error);

	res = eeval(&env)(txt::parse(ext::expr, "'ok' + 'ok'"))->solve();
	BOOST_TEST(res == cval{"okok"});

	eeval str2_e(&env);
	BOOST_CHECK_THROW(str2_e(txt::parse(ext::expr, "'ok'-'ups'")), std::runtime_error);
}
BOOST_FIXTURE_TEST_CASE(concat, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "3~5~10*2"))->solve();
	BOOST_TEST(res == cval{"3520"});
}
BOOST_FIXTURE_TEST_CASE(negate, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "!!true"))->solve();
	BOOST_TEST(res == cval{true});
	eeval str_e(&env);
	BOOST_CHECK_THROW(str_e(txt::parse(ext::expr, "!'1'")), std::runtime_error);
}
BOOST_FIXTURE_TEST_CASE(cmp_check, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "1 == 1"))->solve();
	BOOST_TEST(res == cval{true});
	res = eeval(&env)(txt::parse(ext::expr, "1 < 2"))->solve();
	BOOST_TEST(res == cval{true});
	res = eeval(&env)(txt::parse(ext::expr, "'cc' < 'aa'"))->solve();
	BOOST_TEST(res == cval{false});
}
BOOST_FIXTURE_TEST_CASE(logic_check, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "true and true"))->solve();
	BOOST_TEST(res == cval{true});
	res = eeval(&env)(txt::parse(ext::expr, "true and false"))->solve();
	BOOST_TEST(res == cval{false});
	res = eeval(&env)(txt::parse(ext::expr, "'a' and 'b'"))->solve();
	BOOST_TEST(res == cval{true});
//	res = eeval(&env)(txt::parse(ext::expr, "'' and ''"))->solve();
//	BOOST_TEST(res == cval{true});
	res = eeval(&env)(txt::parse(ext::expr, "'b' and ''"))->solve();
	BOOST_TEST(res == cval{false});
}
BOOST_FIXTURE_TEST_CASE(op_if, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "'ok' if 1==1 else 'not ok'"))->solve();
	BOOST_TEST(res == cval{"ok"});
	res = eeval(&env)(txt::parse(ext::expr, "'not ok' if 1==2 else 'ok'"))->solve();
	BOOST_TEST(res == cval{"ok"});
	res = eeval(&env)(txt::parse(ext::expr, "'not ok' if 1==2"))->solve();
	BOOST_TEST(res == cval{""});
}
BOOST_FIXTURE_TEST_CASE(array, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "[1, 3, 5]"))->solve();
	BOOST_TEST(res == cval{"[1,3,5]"});
	eeval ev(&env);
	BOOST_CHECK_THROW(ev(txt::parse(ext::expr, "[1, 'kuku', 5]")), std::runtime_error);
}
BOOST_FIXTURE_TEST_CASE(tuple, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "(1, 3, 'ok')"))->solve();
	BOOST_TEST(res == cval{"(1,3,\"ok\")"});
}
BOOST_AUTO_TEST_SUITE_END() // term
BOOST_AUTO_TEST_SUITE(reduce)
BOOST_FIXTURE_TEST_CASE(dict, mock_exenv_fixture)
{
	auto res = eeval(&env)(txt::parse(ext::expr, "{'name':1,'name2':'value'}"))->solve();
	BOOST_TEST(res == cval{"{\"name\":1,\"name2\":\"value\"}"});
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
	MOCK_EXPECT(c->solve).returns("e"s);
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
	MOCK_EXPECT(c->solve).returns("e"s);
	MOCK_EXPECT(a->find).with(cppjinja::east::var_name{"b"}).returns(b);
	MOCK_EXPECT(b->find).with(cppjinja::east::var_name{"e"}).returns(abe);
	auto res = eeval(&env)(txt::parse(ext::expr, "a['b'][c]"));
	BOOST_TEST(res.get() == abe.get());
}
BOOST_AUTO_TEST_SUITE_END() // point

BOOST_AUTO_TEST_SUITE(fnc_call)
BOOST_FIXTURE_TEST_CASE(without_args, mock_exenv_fixture)
{
	std::vector<cppjinja::east::function_parameter> params;
	auto a = std::make_shared<mocks::context_object>();
	auto call = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"a"}).returns(a);
	MOCK_EXPECT(a->call).with(params).returns(call);
	auto res = eeval(&env)(txt::parse(ext::expr, "a()"));
	BOOST_TEST(res.get() == call.get());
}
BOOST_FIXTURE_TEST_CASE(with_args, mock_exenv_fixture)
{
	std::vector<cppjinja::east::function_parameter> params;
	params.emplace_back(cppjinja::east::function_parameter{std::nullopt, "ok"});

	auto a = std::make_shared<mocks::context_object>();
	auto call = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"a"}).returns(a);
	MOCK_EXPECT(a->call).with(params).returns(call);
	auto res = eeval(&env)(txt::parse(ext::expr, "a('ok')"));

	BOOST_TEST(res.get() == call.get());
}
BOOST_FIXTURE_TEST_CASE(points, mock_exenv_fixture)
{
	std::vector<cppjinja::east::function_parameter> params;
	params.emplace_back(cppjinja::east::function_parameter{std::nullopt, "ok"});

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
	MOCK_EXPECT(d->solve).returns("ok");

	auto res = eeval(&env)(txt::parse(ext::expr, "a.b(c.d)"));
	BOOST_TEST(res.get() == call.get());
}
BOOST_AUTO_TEST_SUITE_END() // fnc_call

BOOST_AUTO_TEST_SUITE(filter_call)
BOOST_FIXTURE_TEST_CASE(without_args, mock_exenv_fixture)
{
	std::vector<cppjinja::east::function_parameter> params;
	auto a = std::make_shared<mocks::context_object>();
	auto b = std::make_shared<mocks::context_object>();
	auto result = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"a"}).returns(a);
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"b"}).returns(b);
	MOCK_EXPECT(a->solve).returns("ok");

	MOCK_EXPECT(b->call).calls([result](std::vector<cppjinja::east::function_parameter> params){
		BOOST_TEST_REQUIRE(params.size() == 1);
		BOOST_TEST(*params[0].name == "$");
		BOOST_TEST(*params[0].val == cppjinja::east::value_term{"ok"s});
		return result;
	});

	auto res = eeval(&env)(txt::parse(ext::expr, "a | b"));
	BOOST_TEST(res == result);
}
BOOST_FIXTURE_TEST_CASE(with_args, mock_exenv_fixture)
{
	std::vector<cppjinja::east::function_parameter> params;
	auto a = std::make_shared<mocks::context_object>();
	auto b = std::make_shared<mocks::context_object>();
	auto c = std::make_shared<mocks::context_object>();
	auto result = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"a"}).returns(a);
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"b"}).returns(b);
	MOCK_EXPECT(all_ctx.find).with(cppjinja::east::var_name{"c"}).returns(c);
	MOCK_EXPECT(a->solve).returns("ok");
	MOCK_EXPECT(c->solve).returns("cslv");

	MOCK_EXPECT(b->call).calls([result](std::vector<cppjinja::east::function_parameter> params){
		BOOST_TEST_REQUIRE(params.size() == 2);
		BOOST_TEST(*params[0].name == "$");
		BOOST_TEST(*params[0].val == cppjinja::east::value_term{"ok"s});
		BOOST_TEST(*params[1].val == cppjinja::east::value_term{"cslv"s});
		return result;
	});

	auto res = eeval(&env)(txt::parse(ext::expr, "a | b(c)"));
	BOOST_TEST(res == result);
}
BOOST_AUTO_TEST_SUITE_END() // filter_call

BOOST_AUTO_TEST_SUITE_END() // reduce
BOOST_AUTO_TEST_SUITE_END() // expr
BOOST_AUTO_TEST_SUITE_END() // phase_evaluate