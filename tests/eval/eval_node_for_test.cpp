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
	auto list_val = "[1, 2]"_json;
	MOCK_EXPECT(all_ctx.find).with(evar_name{"list"}).returns(list);
	MOCK_EXPECT(list->jval).returns(list_val);

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
		if(n=="loop"s) return;
		BOOST_TEST(n=="a"s);
		BOOST_REQUIRE(child);
		BOOST_TEST(child->jval() == ++iteration);
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
	auto list_val = "{\"one\":1, \"two\":2}"_json;
	MOCK_EXPECT(all_ctx.find).with(evar_name{"list"}).returns(list);
	MOCK_EXPECT(list->jval).returns(list_val);

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
		BOOST_TEST(child->jval() == "one"s);
	});
	MOCK_EXPECT(locals.add)
	        .once().in(locals_add_seq)
	        .calls( [](
	            cppjinja::east::string_t n
	          , std::shared_ptr<cppjinja::evt::context_object> child){
		BOOST_TEST(n=="v");
		BOOST_REQUIRE(child);
		BOOST_TEST(child->jval() == 1);
	});
	MOCK_EXPECT(locals.add).once().in(locals_add_seq).with("loop"s, mock::any);
	MOCK_EXPECT(ctx.pop).once().in(locals_add_seq).with(&block);
	MOCK_EXPECT(ctx.push).once().in(locals_add_seq).with(&block);
	MOCK_EXPECT(locals.add)
	        .once().in(locals_add_seq)
	        .calls( [](
	            cppjinja::east::string_t n
	          , std::shared_ptr<cppjinja::evt::context_object> child){
		BOOST_TEST(n=="k");
		BOOST_REQUIRE(child);
		BOOST_TEST(child->jval() == "two"s);
	});
	MOCK_EXPECT(locals.add)
	        .once().in(locals_add_seq)
	        .calls( [](
	            cppjinja::east::string_t n
	          , std::shared_ptr<cppjinja::evt::context_object> child){
		BOOST_TEST(n=="v");
		BOOST_REQUIRE(child);
		BOOST_TEST(child->jval() == 2);
	});
	MOCK_EXPECT(locals.add).once().in(locals_add_seq).with("loop"s, mock::any);
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

BOOST_AUTO_TEST_SUITE(ctxobj)
BOOST_AUTO_TEST_CASE(cannot_add_solve)
{
	evtnodes::block_for_object obj(1);
	auto child = std::make_shared<mocks::context_object>();
	BOOST_CHECK_THROW(obj.solve(), std::exception);
	BOOST_CHECK_THROW(obj.add("ok"s, child), std::exception);
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

	MOCK_EXPECT(ctx.push).once().in(ctx_seq).with(&block);
	MOCK_EXPECT(locals.add).once().in(ctx_seq).with("a"s, mock::any);
	MOCK_EXPECT(locals.add).once().in(ctx_seq).calls(
	            [](east::string_t n, std::shared_ptr<cppjinja::evt::context_object> loop){
		BOOST_TEST(n=="loop"s);
		BOOST_REQUIRE(loop);
		auto ind = loop->find({"index"s});
		BOOST_REQUIRE(ind);
		BOOST_TEST(ind->jval() == 1);
		ind = loop->find({"index0"s});
		BOOST_REQUIRE(ind);
		BOOST_TEST(ind->jval() == 0);
		BOOST_TEST(ind->jval() == loop->find({"ind"s})->jval());
		BOOST_TEST(loop->find({"length"})->jval() == 2);
		auto cycobj = std::make_shared<cppjinja::evt::context_objects::value>(R"(["a", "b"])"_json,1);
		BOOST_TEST(loop->find({"cycle"})->call({{std::nullopt,cycobj}})->jval() == "a"s);
	});
	MOCK_EXPECT(ctx.pop).once().in(ctx_seq);

	MOCK_EXPECT(ctx.push).once().in(ctx_seq).with(&block);
	MOCK_EXPECT(locals.add).once().in(ctx_seq).with("a"s, mock::any);
	MOCK_EXPECT(locals.add).once().in(ctx_seq).calls(
	            [](east::string_t n, std::shared_ptr<cppjinja::evt::context_object> loop){
		BOOST_TEST(n=="loop"s);
		BOOST_REQUIRE(loop);
		auto ind = loop->find({"index"s});
		BOOST_REQUIRE(ind);
		BOOST_TEST(ind->jval() == 2);
		ind = loop->find({"index0"s});
		BOOST_REQUIRE(ind);
		BOOST_TEST(ind->jval() == 1);
		BOOST_TEST(ind->jval() == loop->find({"ind"s})->jval());
		auto cycobj = std::make_shared<cppjinja::evt::context_objects::value>(R"(["a", "b"])"_json,1);
		BOOST_TEST(loop->find({"cycle"})->call({{std::nullopt,cycobj}})->jval() == "b"s);
	});
	MOCK_EXPECT(ctx.pop).once().in(ctx_seq);

	block.render(env);
}
BOOST_AUTO_TEST_SUITE_END() // ctxobj

BOOST_AUTO_TEST_SUITE_END() // block_for
BOOST_AUTO_TEST_SUITE_END() // nodes
BOOST_AUTO_TEST_SUITE_END() // phase_evaluate
