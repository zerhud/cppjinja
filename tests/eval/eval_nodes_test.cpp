/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE evaluator nodes

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "mocks.hpp"
#include "evtree/node.hpp"
#include "evtree/nodes/tmpl.hpp"
#include "evtree/nodes/op_set.hpp"
#include "evtree/nodes/op_out.hpp"
#include "evtree/nodes/content_block.hpp"
#include "evtree/nodes/content.hpp"
#include "evtree/nodes/block_named.hpp"
#include "evtree/nodes/block_macro.hpp"
#include "evtree/nodes/block_if.hpp"
#include "evtree/exenv/obj_holder.hpp"
#include "evtree/evtree.hpp"

using namespace std::literals;
namespace tdata = boost::unit_test::data;

using evt_node = cppjinja::evt::node;
namespace evtnodes = cppjinja::evtnodes;
namespace ast = cppjinja::ast;
using ast::value_term;
using mocks::mock_exenv_fixture; // qtcreator cannot parse test with namespace

struct mock_callable_fixture : mock_exenv_fixture {
	mocks::node child1, child2;

	template<typename Ast>
	void prepare_ast_for_params(Ast& ast_bl)
	{
		ast_bl.params.emplace_back(ast::macro_parameter{"p1", value_term{41}});
		ast_bl.params.emplace_back(ast::macro_parameter{"p2", value_term{42}});
		ast_bl.params.emplace_back(ast::macro_parameter{"p3", std::nullopt});
	}
	template<typename B>
	void check_params_not_in_stack(B& bl)
	{
		MOCK_EXPECT(calls.get_params).returns(std::vector<ast::function_call_parameter>{});
		BOOST_REQUIRE(bl.param(calls, ast::var_name{"p1"}).has_value());
		BOOST_TEST(*bl.param(calls, ast::var_name{"p1"}) == value_term{41});
		BOOST_REQUIRE(bl.param(calls, ast::var_name{"p2"}).has_value());
		BOOST_TEST(*bl.param(calls, ast::var_name{"p2"}) == value_term{42});
		BOOST_CHECK_THROW(bl.param(calls, ast::var_name{"p3"}), std::exception);
	}

	template<typename Ast>
	void prepare_for_render_two_childrend(Ast& ast_bl)
	{
		ast_bl.left_close.trim = true;
		ast_bl.right_open.trim = true;
	}

	template<typename B, typename Ast>
	void check_getters(B& bl, const Ast& ast_bl)
	{
		BOOST_TEST(bl.is_leaf() == false);
		BOOST_TEST(bl.name() == ast_bl.name);
		BOOST_TEST(bl.rinfo().trim_left == ast_bl.left_open.trim);
		BOOST_TEST(bl.rinfo().trim_right == ast_bl.right_close.trim);
		BOOST_CHECK(!bl.param(calls, ast::var_name{"a"}).has_value());
	}

	void prepare_for_render_two_childrend()
	{
		expect_children({&child1, &child2});
		cppjinja::evt::render_info rinfo1{true,false};
		cppjinja::evt::render_info rinfo2{false,true};
		mock::sequence render_seq;
		MOCK_EXPECT(env.rinfo).once().in(render_seq).with(rinfo1);
		MOCK_EXPECT(child1.render).once().in(render_seq);
		MOCK_EXPECT(env.rinfo).once().in(render_seq).with(rinfo2);
		MOCK_EXPECT(child2.render).once().in(render_seq);
		MOCK_EXPECT(child1.rinfo).once().returns(cppjinja::evt::render_info{false,false});
		MOCK_EXPECT(child2.rinfo).once().returns(cppjinja::evt::render_info{false,false});
	}
};

template<typename Node, typename Ast>
void test_name_equals(const Node& evtn, const Ast& astnode)
{
	BOOST_TEST( evtn.name() == astnode.name );
}

BOOST_AUTO_TEST_SUITE(nodes)

BOOST_AUTO_TEST_SUITE(tmpl)
BOOST_AUTO_TEST_CASE(getters)
{
	ast::tmpl t;
	t.name = "test_name";
	evtnodes::tmpl tmpl(t);
	BOOST_CHECK( !tmpl.rinfo().trim_left );
	BOOST_CHECK( !tmpl.rinfo().trim_right );
	BOOST_CHECK( !tmpl.is_leaf() );
	test_name_equals( tmpl, t );
}
BOOST_FIXTURE_TEST_CASE(no_children_no_render, mock_exenv_fixture)
{
	expect_roots({});
	evtnodes::tmpl tmpl(ast::tmpl{});
	mock::sequence ctx_seq;
	MOCK_EXPECT(env.current_node).once().in(ctx_seq).with(&tmpl);
	BOOST_CHECK_NO_THROW(tmpl.render(env));
}
BOOST_FIXTURE_TEST_CASE(rendered_only_empty_name, mock_exenv_fixture)
{
	using cppjinja::evt::obj_holder;
	obj_holder globals;
	evtnodes::tmpl tmpl(ast::tmpl{});
	mock::sequence ctx_seq;
	mocks::callable_node child_with_name, child_empty_name;
	expect_roots({&child_with_name, &child_empty_name});
	expect_call(&tmpl, &child_empty_name, {});
	MOCK_EXPECT(env.current_node).with(&tmpl);
	MOCK_EXPECT(child_empty_name.name).at_least(1).returns("");
	MOCK_EXPECT(child_with_name.name).at_least(1).returns("tn");
	MOCK_EXPECT(child_empty_name.evaluate).once().returns("test");
	MOCK_EXPECT(env.globals).calls([&globals]()->obj_holder&{return globals;});
	BOOST_CHECK_NO_THROW(tmpl.render(env));
	BOOST_TEST(out.str() == "test");
}
BOOST_FIXTURE_TEST_CASE(creates_self, mock_exenv_fixture)
{
	using cppjinja::evt::obj_holder;
	obj_holder globals;
	evtnodes::tmpl tmpl(ast::tmpl{});
	mocks::callable_node child1, child2;
	expect_roots({&child1, &child2});
	MOCK_EXPECT(env.current_node).with(&tmpl);
	MOCK_EXPECT(child1.name).at_least(1).returns("ch1");
	MOCK_EXPECT(child2.name).at_least(1).returns("ch2");
	MOCK_EXPECT(child1.evaluate).returns("ok_ch1");
	MOCK_EXPECT(child2.evaluate).returns("ok_ch2");
	MOCK_EXPECT(env.globals).calls([&globals]()->obj_holder&{return globals;});
	BOOST_CHECK_NO_THROW(tmpl.render(env));

	mocks::node caller;
	cppjinja::ast::function_call call;
	call.ref.emplace_back("ch1");
	expect_call(&caller, &child1, {});
	MOCK_EXPECT(ctx.nth_node_on_stack).with(0).returns(&caller);
	BOOST_TEST(globals.call(call) == value_term{"ok_ch1"});
	call.ref = {"self", "ch1"};
	expect_call(&caller, &child1, {});
	MOCK_EXPECT(ctx.nth_node_on_stack).with(0).returns(&caller);
	BOOST_TEST(globals.call(call) == value_term{"ok_ch1"});
	call.ref.back() = "ch2";
	expect_call(&caller, &child2, {});
	MOCK_EXPECT(ctx.nth_node_on_stack).with(0).returns(&caller);
	BOOST_TEST(globals.call(call) == value_term{"ok_ch2"});
}
BOOST_AUTO_TEST_SUITE_END() // tmpl

BOOST_AUTO_TEST_SUITE(op_set)
BOOST_AUTO_TEST_CASE(getters)
{
	ast::op_set ast_node{ {1,1}, "tname", value_term{42}, {{1,1},false}, {{1,1},true} };
	evtnodes::op_set snode(ast_node);
	BOOST_TEST(snode.rinfo().trim_left == false);
	BOOST_TEST(snode.rinfo().trim_right == true);
	BOOST_TEST(snode.is_leaf() == true);
}
BOOST_AUTO_TEST_SUITE(render)
BOOST_FIXTURE_TEST_CASE(value, mock_exenv_fixture)
{
	ast::op_set ast_node{ {1,1}, "tname", value_term{42}, {{1,1},false}, {{1,1},true} };
	evtnodes::op_set snode(ast_node);
	MOCK_EXPECT(env.current_node).once().with(&snode);
	MOCK_EXPECT(ctx.inject_obj).once().calls(
	[](const std::string& n, std::shared_ptr<cppjinja::evt::ctx_object> obj){
		BOOST_TEST(n == "tname");
		BOOST_TEST(dynamic_cast<cppjinja::evt::ctx_object*>(obj.get()) != nullptr);
		BOOST_TEST(obj->solve(ast::var_name{}) == value_term{42});
	});
	BOOST_CHECK_NO_THROW(snode.render(env));
}
BOOST_FIXTURE_TEST_CASE(name, mock_exenv_fixture)
{
	using namespace cppjinja::ast;
	using cppjinja::evt::obj_holder;

	obj_holder locals;
	ast::op_set ast_node{ {1,1}, "tname", value_term{var_name{"a", "b"}}, {{1,1},false}, {{1,1},true} };
	evtnodes::op_set snode(ast_node);
	auto obj = std::make_shared<mocks::ctx_object>();
	locals.add("a", obj);
	MOCK_EXPECT(env.current_node).once().with(&snode);
	MOCK_EXPECT(env.locals).calls([&locals]()->obj_holder&{return locals;});
	MOCK_EXPECT(ctx.inject_obj).once().calls(
	[&obj](const std::string& n, std::shared_ptr<cppjinja::evt::ctx_object> in){
		BOOST_TEST(n == "tname");
		BOOST_TEST(obj == in);
	});
	snode.render(env);
}
BOOST_AUTO_TEST_SUITE_END() // render
BOOST_AUTO_TEST_SUITE_END() // op_set

BOOST_AUTO_TEST_SUITE(op_out)
BOOST_AUTO_TEST_CASE(getters)
{
	ast::op_out ast_out{ {1,1}, value_term{42}, {}, {{1,1},false}, {{1,1},true} };
	evtnodes::op_out snode(ast_out);
	BOOST_TEST(snode.rinfo().trim_left == false);
	BOOST_TEST(snode.rinfo().trim_right == true);
	BOOST_TEST(snode.is_leaf() == true);
}
BOOST_FIXTURE_TEST_CASE(render, mock_exenv_fixture)
{
	ast::op_out ast_out{ {1,1}, value_term{42}, {}, {{1,1},false}, {{1,1},true} };
	evtnodes::op_out snode(ast_out);
	MOCK_EXPECT(env.current_node).once().with(&snode);
	BOOST_CHECK_NO_THROW(snode.render(env));
}
BOOST_FIXTURE_TEST_CASE(render_with_filters, mock_exenv_fixture)
{
	ast::op_out ast_out{ {1,1}, value_term{42}, {}, {{1,1},false}, {{1,1},true} };
	ast_out.filters.push_back(ast::filter_call{ast::var_name{"a"}});
	ast_out.filters.push_back(ast::filter_call{ast::function_call(ast::var_name{"fnc"}, {})});
	evtnodes::op_out snode(ast_out);
	MOCK_EXPECT(env.current_node).once().with(&snode);
	mock::sequence filter_seq;
	MOCK_EXPECT(data.filter).once().in(filter_seq).returns("first");
	MOCK_EXPECT(data.filter).once().in(filter_seq).returns("ok");
	BOOST_CHECK_NO_THROW(snode.render(env));
	BOOST_TEST(out.str() == "ok");
}
BOOST_AUTO_TEST_SUITE_END() // op_out

BOOST_AUTO_TEST_SUITE(content_block)
BOOST_FIXTURE_TEST_CASE(getters, mock_exenv_fixture)
{
	evtnodes::content_block cnt_bl({false, true}, "test_name");
	BOOST_TEST(cnt_bl.is_leaf() == false);
	BOOST_TEST(cnt_bl.rinfo().trim_left == false);
	BOOST_TEST(cnt_bl.rinfo().trim_right == true);
}
BOOST_FIXTURE_TEST_CASE(render, mock_exenv_fixture)
{
	evtnodes::content_block cnt_bl({false, true}, "test_name");
	expect_children({});
	MOCK_EXPECT(env.crinfo).once().returns(cppjinja::evt::render_info{true, false});
	MOCK_EXPECT(env.current_node).once().with(&cnt_bl);
	BOOST_CHECK_NO_THROW(cnt_bl.render(env));
	BOOST_TEST(out.str() == "");
}
BOOST_FIXTURE_TEST_CASE(render_two, mock_exenv_fixture)
{
	evtnodes::content_block cnt_bl({false, true}, "test_name");
	mocks::node child1, child2;
	expect_children({&child1, &child2});
	MOCK_EXPECT(env.crinfo).returns(cppjinja::evt::render_info{true, false});
	MOCK_EXPECT(env.rinfo).exactly(2);
	MOCK_EXPECT(env.current_node).once().with(&cnt_bl);
	MOCK_EXPECT(child1.render).once();
	MOCK_EXPECT(child1.rinfo).once().returns(cppjinja::evt::render_info{false,false});
	MOCK_EXPECT(child2.render).once();
	MOCK_EXPECT(child2.rinfo).once().returns(cppjinja::evt::render_info{false,false});
	BOOST_CHECK_NO_THROW(cnt_bl.render(env));
	BOOST_TEST(out.str() == "");
}
BOOST_AUTO_TEST_SUITE_END() // content_block

BOOST_AUTO_TEST_SUITE(content)
BOOST_FIXTURE_TEST_CASE(getters, mock_exenv_fixture)
{
	evtnodes::content cnt("test_content");
	BOOST_TEST(cnt.is_leaf() == true);
	BOOST_TEST(cnt.rinfo().trim_left == false);
	BOOST_TEST(cnt.rinfo().trim_right == false);
}
BOOST_FIXTURE_TEST_CASE(render, mock_exenv_fixture)
{
	evtnodes::content cnt("test_content");
	MOCK_EXPECT(env.current_node).once().with(&cnt);
	MOCK_EXPECT(env.crinfo).once().returns(cppjinja::evt::render_info{false,false});
	BOOST_CHECK_NO_THROW(cnt.render(env));
	BOOST_TEST(out.str() == "test_content");
}
BOOST_DATA_TEST_CASE_F(mock_exenv_fixture, trims_leftright,
           tdata::make(
                cppjinja::evt::render_info{true, false},
                cppjinja::evt::render_info{false, true},
                cppjinja::evt::render_info{true, true})
         ^ tdata::make("rst  "s, "  rst"s, "rst"s)
         * tdata::monomorphic::singleton("  rst  "s)
         , rinfo, result, src)
{
	evtnodes::content cnt(src);
	MOCK_EXPECT(env.current_node).with(&cnt);
	MOCK_EXPECT(env.crinfo).once().returns(rinfo);
	cnt.render(env);
	BOOST_TEST(out.str() == result);
}
BOOST_AUTO_TEST_SUITE_END() // content

BOOST_AUTO_TEST_SUITE(block_named)
BOOST_FIXTURE_TEST_CASE(render_no_children, mock_exenv_fixture)
{
	ast::block_named ast_bl;
	evtnodes::block_named cnt(ast_bl);
	expect_children({});
	expect_cxt_settings(&cnt);
	expect_call(&cnt, &cnt, {});
	MOCK_EXPECT(env.result).once().returns("result");
	BOOST_CHECK_NO_THROW(cnt.render(env));
	BOOST_TEST(out.str() == "result");
}
BOOST_FIXTURE_TEST_CASE(render_no_params, mock_callable_fixture)
{
	ast::block_named ast_bl;
	ast_bl.params.emplace_back(ast::macro_parameter{"a", std::nullopt});
	evtnodes::block_named cnt(ast_bl);
	MOCK_EXPECT(env.current_node).once().with(&cnt);
	BOOST_CHECK_NO_THROW(cnt.render(env));
	BOOST_TEST(out.str() == "");
}
BOOST_FIXTURE_TEST_CASE(render_two_children, mock_callable_fixture)
{
	ast::block_named ast_bl;
	prepare_for_render_two_childrend(ast_bl);
	evtnodes::block_named cnt(ast_bl);
	expect_cxt_settings(&cnt);
	expect_call(&cnt, &cnt, {});
	prepare_for_render_two_childrend();
	MOCK_EXPECT(env.result).once().returns("result");
	BOOST_CHECK_NO_THROW(cnt.render(env));
	BOOST_TEST(out.str() == "result");
}
BOOST_FIXTURE_TEST_CASE(evaluate_two_children, mock_callable_fixture)
{
	ast::block_named ast_bl;
	prepare_for_render_two_childrend(ast_bl);
	ast_bl.params.emplace_back(ast::macro_parameter{"a", std::nullopt});
	evtnodes::block_named cnt(ast_bl);
	expect_cxt_settings(&cnt);
	prepare_for_render_two_childrend();
	MOCK_EXPECT(env.result).once().returns("result");
	BOOST_TEST(cnt.evaluate(env) == "result");
}
BOOST_AUTO_TEST_SUITE_END() // block_named

BOOST_AUTO_TEST_SUITE(block_macro)
BOOST_FIXTURE_TEST_CASE(render_do_nothing, mock_callable_fixture)
{
	ast::block_macro ast_bl;
	evtnodes::block_macro cnt(ast_bl);
	MOCK_EXPECT(env.current_node).with(&cnt);
	BOOST_CHECK_NO_THROW(cnt.render(env));
	BOOST_TEST(out.str() == "");
}
BOOST_FIXTURE_TEST_CASE(evaluate_two_children, mock_callable_fixture)
{
	using cppjinja::evt::ctx_object;
	using cppjinja::evt::delay_solver;
	ast::block_macro ast_bl;
	prepare_for_render_two_childrend(ast_bl);
	ast_bl.params.emplace_back(ast::macro_parameter{"a", std::nullopt});
	evtnodes::block_macro cnt(ast_bl);
	prepare_for_render_two_childrend();
	mock::sequence ctx_seq;
	MOCK_EXPECT(calls.get_params).once().returns(
	            std::vector<ast::function_call_parameter>{value_term{42}});
	MOCK_EXPECT(env.current_node).once().with(&cnt).in(ctx_seq);
	MOCK_EXPECT(ctx.inject_obj).once().in(ctx_seq).calls(
	[](ast::string_t n, std::shared_ptr<ctx_object> o){
		BOOST_TEST(n == "a");
		BOOST_TEST(dynamic_cast<delay_solver*>(o.get()) != nullptr);
	});
	MOCK_EXPECT(ctx.takeout_obj).once().with("a").in(ctx_seq);
	BOOST_TEST(cnt.evaluate(env) == "");
}
BOOST_AUTO_TEST_SUITE_END() // block_macro

BOOST_AUTO_TEST_SUITE(callables)
typedef std::tuple<
  std::tuple<ast::block_named, evtnodes::block_named>
, std::tuple<ast::block_macro, evtnodes::block_macro>
> callable_list;
BOOST_FIXTURE_TEST_CASE_TEMPLATE(getters, T, callable_list, mock_callable_fixture)
{
	std::tuple_element_t<0, T> ast_bl;
	ast_bl.name = "test_name";
	prepare_for_render_two_childrend(ast_bl);
	std::tuple_element_t<1, T> bl(ast_bl);
	check_getters(bl, ast_bl);
}
BOOST_FIXTURE_TEST_CASE_TEMPLATE(position_param_with_values, T, callable_list, mock_callable_fixture)
{
	std::tuple_element_t<0,T> ast_bl;
	prepare_ast_for_params(ast_bl);
	std::tuple_element_t<1,T> cnt(ast_bl);
	check_params_not_in_stack(cnt);
}
BOOST_FIXTURE_TEST_CASE_TEMPLATE(position_param_calls, T, callable_list, mock_callable_fixture)
{
	std::tuple_element_t<0,T> ast_bl;
	ast_bl.name = "test_name";
	prepare_ast_for_params(ast_bl);
	std::vector<cppjinja::ast::function_call_parameter> call_params;
	call_params.emplace_back(ast::function_call_parameter(value_term{51}));
	call_params.emplace_back(ast::function_call_parameter("p3", value_term{52}));
	std::tuple_element_t<1,T> cnt(ast_bl);
	MOCK_EXPECT(calls.get_params).returns(call_params);
	BOOST_REQUIRE(cnt.param(calls, ast::var_name{"p1"}).has_value());
	BOOST_TEST(*cnt.param(calls, ast::var_name{"p1"}) == value_term{51});
	BOOST_REQUIRE(cnt.param(calls, ast::var_name{"p2"}).has_value());
	BOOST_TEST(*cnt.param(calls, ast::var_name{"p2"}) == value_term{42});
	BOOST_REQUIRE(cnt.param(calls, ast::var_name{"p3"}).has_value());
	BOOST_TEST(*cnt.param(calls, ast::var_name{"p3"}) == value_term{52});
}
BOOST_AUTO_TEST_SUITE_END() // callables

BOOST_AUTO_TEST_SUITE(block_if)
BOOST_FIXTURE_TEST_CASE(getters, mock_exenv_fixture)
{
	ast::block_if ast;
	ast.left_open.trim = true;
	ast.right_close.trim = true;
	evtnodes::block_if bl(ast);
	BOOST_TEST(bl.is_leaf() == false);
	BOOST_TEST(bl.rinfo().trim_left == ast.left_open.trim);
	BOOST_TEST(bl.rinfo().trim_right == ast.right_close.trim);
}
BOOST_FIXTURE_TEST_CASE(render_true, mock_callable_fixture)
{
	ast::block_if ast_bl;
	ast_bl.condition = ast::binary_op(value_term{1}, ast::comparator::eq, value_term{1});
	prepare_for_render_two_childrend(ast_bl);
	evtnodes::block_if bl(ast_bl);
	expect_children({&child1, &child2});
	MOCK_EXPECT(child1.render).once();
	MOCK_EXPECT(env.rinfo).once();
	MOCK_EXPECT(env.current_node).with(&bl);
	BOOST_CHECK_NO_THROW(bl.render(env));
	BOOST_TEST(out.str() == "");
}
BOOST_FIXTURE_TEST_CASE(render_false_we, mock_callable_fixture)
{
	ast::block_if ast_bl;
	ast_bl.condition = ast::binary_op(value_term{2}, ast::comparator::eq, value_term{1});
	prepare_for_render_two_childrend(ast_bl);
	evtnodes::block_if bl(ast_bl);
	expect_children({&child1});
	MOCK_EXPECT(env.rinfo).once();
	MOCK_EXPECT(env.current_node).with(&bl);
	BOOST_CHECK_NO_THROW(bl.render(env));
	BOOST_TEST(out.str() == "");
}
BOOST_FIXTURE_TEST_CASE(render_false, mock_callable_fixture)
{
	ast::block_if ast_bl;
	ast_bl.condition = ast::binary_op(value_term{2}, ast::comparator::eq, value_term{1});
	prepare_for_render_two_childrend(ast_bl);
	evtnodes::block_if bl(ast_bl);
	expect_children({&child1, &child2});
	MOCK_EXPECT(child2.render).once();
	MOCK_EXPECT(env.rinfo).once();
	MOCK_EXPECT(env.current_node).with(&bl);
	BOOST_CHECK_NO_THROW(bl.render(env));
	BOOST_TEST(out.str() == "");
}
BOOST_AUTO_TEST_SUITE_END() // block_if

BOOST_AUTO_TEST_SUITE_END() // nodes
