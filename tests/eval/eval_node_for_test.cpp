/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE evaluator node_for

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "mocks.hpp"
#include "evtree/nodes/block_for.hpp"
#include "parser/parse.hpp"
#include "parser/grammar/expr.hpp"

namespace ast = cppjinja::ast;
namespace east = cppjinja::east;
namespace aps = cppjinja::ast::expr_ops;
namespace evtnodes = cppjinja::evtnodes;

using namespace std::literals;

using evar_name = cppjinja::east::var_name;
using obj_val_t = cppjinja::evt::context_objects::value;

struct mock_for_fixture : mocks::mock_exenv_fixture {};

BOOST_AUTO_TEST_SUITE(phase_evaluate)
BOOST_AUTO_TEST_SUITE(nodes)
BOOST_AUTO_TEST_SUITE(block_for)

BOOST_FIXTURE_TEST_CASE(one_or_two, mock_for_fixture)
{
	ast::block_for ast_bl;
	ast_bl.value = aps::expr{aps::single_var_name{"list"}};
	BOOST_CHECK_THROW(cppjinja::evtnodes::block_for{ast_bl}, std::exception);
	ast_bl.vars.emplace_back("a"s);
	BOOST_CHECK_NO_THROW(cppjinja::evtnodes::block_for{ast_bl});
	ast_bl.vars.emplace_back("b"s);
	BOOST_CHECK_NO_THROW(cppjinja::evtnodes::block_for{ast_bl});
	ast_bl.vars.emplace_back("c"s);
	BOOST_CHECK_THROW(cppjinja::evtnodes::block_for{ast_bl}, std::exception);
}
BOOST_FIXTURE_TEST_CASE(render_one, mock_for_fixture)
{
	ast::block_for ast_bl;
	ast_bl.vars.emplace_back("a"s);
	ast_bl.value = aps::expr{aps::single_var_name{"list"}};
	cppjinja::evtnodes::block_for block(ast_bl);

	mocks::node child;
	auto list = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).with("list"_s).returns(list);
	auto list_val = std::make_shared<absd::simple_data_holder>();
	list_val->push_back() = 1;
	list_val->push_back() = 2;
	MOCK_EXPECT(list->solve).returns(absd::data{list_val});

	int iteration = 0;
	expect_glp(0, 2, 0);
	expect_children({&child});
	MOCK_EXPECT(env.current_node);
	MOCK_EXPECT(ctx.pop).with(&block);
	MOCK_EXPECT(ctx.push).with(&block);
	MOCK_EXPECT(env.rinfo);
	MOCK_EXPECT(child.render).exactly(2);
	MOCK_EXPECT(locals.add)
	        .exactly(2 + 2)
	        .calls( [&iteration](
	            cppjinja::east::string_t n
	          , std::shared_ptr<cppjinja::evt::context_object> child){
		if(n=="loop"_s) return;
		BOOST_TEST(n=="a"_s);
		BOOST_REQUIRE(child);
		BOOST_TEST(child->solve() == ++iteration);
	});
	block.render(env);
	BOOST_TEST(out.str() == ""s);
}
BOOST_FIXTURE_TEST_CASE(render_two, mock_for_fixture)
{
	ast::block_for ast_bl;
	ast_bl.vars.emplace_back("k"s);
	ast_bl.vars.emplace_back("v"s);
	ast_bl.value = aps::expr{aps::single_var_name{"list"}};
	cppjinja::evtnodes::block_for block(ast_bl);

	mocks::node child;
	auto list = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).with("list"_s).returns(list);
	auto list_val = std::make_shared<absd::simple_data_holder>();
	list_val->put("one") = 1;
	list_val->put("two") = 2;
	MOCK_EXPECT(list->solve).returns(absd::data{list_val});

	mock::sequence locals_add_seq;
	expect_glp(0, 2, 0);
	expect_children({&child});
	MOCK_EXPECT(env.rinfo);
	MOCK_EXPECT(child.render).exactly(2);
	MOCK_EXPECT(env.current_node).with(&block);
	MOCK_EXPECT(ctx.push).once().in(locals_add_seq).with(&block);
	MOCK_EXPECT(locals.add)
	        .once().in(locals_add_seq)
	        .calls( [](
	            cppjinja::east::string_t n
	          , std::shared_ptr<cppjinja::evt::context_object> child){
		BOOST_TEST(n=="k");
		BOOST_REQUIRE(child);
		BOOST_TEST(child->solve() == "one"_s);
	});
	MOCK_EXPECT(locals.add)
	        .once().in(locals_add_seq)
	        .calls( [](
	            cppjinja::east::string_t n
	          , std::shared_ptr<cppjinja::evt::context_object> child){
		BOOST_TEST(n=="v");
		BOOST_REQUIRE(child);
		BOOST_TEST(child->solve() == 1);
	});
	MOCK_EXPECT(locals.add).once().in(locals_add_seq).with("loop"_s, mock::any);
	MOCK_EXPECT(ctx.pop).once().in(locals_add_seq).with(&block);
	MOCK_EXPECT(ctx.push).once().in(locals_add_seq).with(&block);
	MOCK_EXPECT(locals.add)
	        .once().in(locals_add_seq)
	        .calls( [](
	            cppjinja::east::string_t n
	          , std::shared_ptr<cppjinja::evt::context_object> child){
		BOOST_TEST(n=="k");
		BOOST_REQUIRE(child);
		BOOST_TEST(child->solve() == "two"_s);
	});
	MOCK_EXPECT(locals.add)
	        .once().in(locals_add_seq)
	        .calls( [](
	            cppjinja::east::string_t n
	          , std::shared_ptr<cppjinja::evt::context_object> child){
		BOOST_TEST(n=="v");
		BOOST_REQUIRE(child);
		BOOST_TEST(child->solve() == 2);
	});
	MOCK_EXPECT(locals.add).once().in(locals_add_seq).with("loop"_s, mock::any);
	MOCK_EXPECT(ctx.pop).once().in(locals_add_seq).with(&block);
	block.render(env);
	BOOST_TEST(out.str() == ""s);
}
BOOST_FIXTURE_TEST_CASE(no_value_no_else1, mock_for_fixture)
{
	ast::block_for ast_bl;
	ast_bl.vars.emplace_back("a"s);
	ast_bl.value = cppjinja::text::parse(cppjinja::text::expr_ops::expr, "3 if 0==1" );
	cppjinja::evtnodes::block_for block(ast_bl);

	mocks::node child_main;

	expect_children({&child_main});
	MOCK_EXPECT(env.current_node);
	block.render(env);
}
BOOST_FIXTURE_TEST_CASE(no_value_no_else2, mock_for_fixture)
{
	ast::block_for ast_bl;
	ast_bl.vars.emplace_back("a"s);
	ast_bl.vars.emplace_back("b"s);
	ast_bl.value = cppjinja::text::parse(cppjinja::text::expr_ops::expr, "3 if 0==1" );
	cppjinja::evtnodes::block_for block(ast_bl);

	mocks::node child_main;

	expect_children({&child_main});
	MOCK_EXPECT(env.current_node);
	block.render(env);
}
BOOST_FIXTURE_TEST_CASE(no_value_else1, mock_for_fixture)
{
	ast::block_for ast_bl;
	ast_bl.vars.emplace_back("a"s);
	ast_bl.value = cppjinja::text::parse(cppjinja::text::expr_ops::expr, "3 if 0==1" );
	cppjinja::evtnodes::block_for block(ast_bl);

	mocks::node child_main, child_else;

	expect_children({&child_main, &child_else});
	MOCK_EXPECT(env.current_node);
	MOCK_EXPECT(child_else.render).once();
	block.render(env);
}
BOOST_FIXTURE_TEST_CASE(no_value_else2, mock_for_fixture)
{
	ast::block_for ast_bl;
	ast_bl.vars.emplace_back("a"s);
	ast_bl.vars.emplace_back("b"s);
	ast_bl.value = cppjinja::text::parse(cppjinja::text::expr_ops::expr, "3 if 0==1" );
	cppjinja::evtnodes::block_for block(ast_bl);

	mocks::node child_main, child_else;

	expect_children({&child_main, &child_else});
	MOCK_EXPECT(env.current_node);
	MOCK_EXPECT(child_else.render).once();
	block.render(env);
}
BOOST_FIXTURE_TEST_CASE(no_value_filter, mock_for_fixture)
{
	ast::block_for ast_bl;
	ast_bl.vars.emplace_back("a"s);
	ast_bl.value = cppjinja::text::parse(cppjinja::text::expr_ops::expr, "[4,5,6]" );
	ast_bl.condition = cppjinja::text::parse(cppjinja::text::expr_ops::expr_bool, "a==3");
	cppjinja::evtnodes::block_for block(ast_bl);

	mocks::node child_main;

	expect_glp(0, 4, 0);
	expect_children({&child_main});

	MOCK_EXPECT(env.current_node).with(&block);
	MOCK_EXPECT(ctx.push).with(&block);
	MOCK_EXPECT(ctx.pop).with(&block);

	MOCK_EXPECT(locals.add).exactly(3).with("loop"_s, mock::any);
	MOCK_EXPECT(locals.add)
	        .exactly(3).with("a"_s, mock::any)
	        .calls( [this](
	            cppjinja::east::string_t n
	          , std::shared_ptr<cppjinja::evt::context_object> child){
		BOOST_REQUIRE(child);
		BOOST_CHECK(n=="a" || n=="loop");
		if(n=="a") {
			MOCK_EXPECT(all_ctx.find).once().with("a"_s).returns(child);
		}
	});

	block.render(env);
}

BOOST_AUTO_TEST_SUITE(ctxobj)
BOOST_FIXTURE_TEST_CASE(cannot_add_solve, mock_for_fixture)
{
	evtnodes::block_for_object obj(env.storage(), 1);
	auto child = std::make_shared<mocks::context_object>();
	BOOST_CHECK_THROW(obj.solve(), std::exception);
	BOOST_CHECK_THROW(obj.add("ok"_s, child), std::exception);
}
BOOST_FIXTURE_TEST_CASE(find_throws_if_nonexists, mock_for_fixture)
{
	evtnodes::block_for_object obj(env.storage(), 1);
	BOOST_CHECK_THROW(obj.find("nonexisting"_s), std::runtime_error);
}
BOOST_FIXTURE_TEST_CASE(index, mock_for_fixture)
{
	ast::block_for ast_bl;
	ast_bl.vars.emplace_back("a"s);
	ast_bl.value = cppjinja::text::parse(cppjinja::text::expr_ops::expr, "[1,2] if 0==0" );
	cppjinja::evtnodes::block_for block(ast_bl);

	mocks::node child_main;
	mock::sequence ctx_seq;
	expect_glp(0, 4, 0);
	expect_children({&child_main});
	MOCK_EXPECT(env.rinfo);
	MOCK_EXPECT(env.current_node).with(&block);
	MOCK_EXPECT(child_main.render).exactly(2);

	auto ab_val = std::make_shared<absd::simple_data_holder>();
	ab_val->push_back() = "a";
	ab_val->push_back() = "b";

	MOCK_EXPECT(ctx.push).once().in(ctx_seq).with(&block);
	MOCK_EXPECT(locals.add).once().in(ctx_seq).with("a"_s, mock::any);
	MOCK_EXPECT(locals.add).once().in(ctx_seq).calls(
	            [ab_val](east::string_t n, std::shared_ptr<cppjinja::evt::context_object> loop){
		BOOST_TEST(n=="loop"_s);
		BOOST_REQUIRE(loop);
		auto ind = loop->find("index"_s);
		BOOST_REQUIRE(ind);
		BOOST_TEST(ind->solve() == 1);
		ind = loop->find("index0"_s);
		BOOST_REQUIRE(ind);
		BOOST_TEST(ind->solve() == 0);
		BOOST_TEST(ind->solve() == loop->find("ind"_s)->solve());
		BOOST_TEST(loop->find("length"_s)->solve() == 2);
		auto cycobj = std::make_shared<obj_val_t>(absd::data{ab_val});
		auto cycle = loop->find("cycle"_s)->call({{std::nullopt,cycobj}});
		BOOST_TEST(cycle->solve() == "a"_s);
		auto last = loop->find("last"_s);
		BOOST_REQUIRE(last);
		BOOST_TEST(last->solve() == false);
	});
	MOCK_EXPECT(ctx.pop).once().in(ctx_seq);

	MOCK_EXPECT(ctx.push).once().in(ctx_seq).with(&block);
	MOCK_EXPECT(locals.add).once().in(ctx_seq).with("a"_s, mock::any);
	MOCK_EXPECT(locals.add).once().in(ctx_seq).calls(
	            [ab_val](east::string_t n, std::shared_ptr<cppjinja::evt::context_object> loop){
		BOOST_TEST(n=="loop"_s);
		BOOST_REQUIRE(loop);
		auto ind = loop->find("index"_s);
		BOOST_REQUIRE(ind);
		BOOST_TEST(ind->solve() == 2);
		ind = loop->find("index0"_s);
		BOOST_REQUIRE(ind);
		BOOST_TEST(ind->solve() == 1);
		BOOST_TEST(ind->solve() == loop->find("ind"_s)->solve());
		auto cycobj = std::make_shared<obj_val_t>(absd::data{ab_val});
		BOOST_TEST(loop->find("cycle"_s)->call({{std::nullopt,cycobj}})->solve() == "b"_s);
		auto last = loop->find("last"_s);
		BOOST_REQUIRE(last);
		BOOST_TEST(last->solve() == true);
	});
	MOCK_EXPECT(ctx.pop).once().in(ctx_seq);

	block.render(env);
}
BOOST_AUTO_TEST_SUITE_END() // ctxobj

BOOST_AUTO_TEST_SUITE_END() // block_for
BOOST_AUTO_TEST_SUITE_END() // nodes
BOOST_AUTO_TEST_SUITE_END() // phase_evaluate
