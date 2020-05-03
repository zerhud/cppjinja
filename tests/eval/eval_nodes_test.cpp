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
#include "parser/operators/blocks.hpp"

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

	template<typename Ast>
	void prepare_for_render_two_childrend(Ast& ast_bl)
	{
		ast_bl.left_close.trim = true;
		ast_bl.right_open.trim = true;
	}

	template<typename B, typename Ast>
	void check_getters(B& bl, const Ast& ast_bl)
	{
		BOOST_TEST(bl.name() == ast_bl.name);
		BOOST_TEST(bl.rinfo().trim_left == ast_bl.left_open.trim);
		BOOST_TEST(bl.rinfo().trim_right == ast_bl.right_close.trim);
		BOOST_TEST(bl.params().size() == 0);
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

BOOST_AUTO_TEST_SUITE( phase_evaluate )
BOOST_AUTO_TEST_SUITE(nodes)

BOOST_AUTO_TEST_SUITE(tmpl)
BOOST_AUTO_TEST_CASE(getters)
{
	ast::tmpl t;
	t.name = "test_name";
	evtnodes::tmpl tmpl(t);
	BOOST_CHECK( !tmpl.rinfo().trim_left );
	BOOST_CHECK( !tmpl.rinfo().trim_right );
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
	expect_call(&child_empty_name, {});
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

	cppjinja::ast::function_call call;
	call.ref.emplace_back("ch1");
	expect_call(&child1, {});
	BOOST_TEST(globals(call) == value_term{"ok_ch1"});
	call.ref = {"self", "ch1"};
	expect_call(&child1, {});
	BOOST_TEST(globals(call) == value_term{"ok_ch1"});
	call.ref.back() = "ch2";
	expect_call(&child2, {});
	BOOST_TEST(globals(call) == value_term{"ok_ch2"});
}
BOOST_AUTO_TEST_SUITE_END() // tmpl

BOOST_AUTO_TEST_SUITE(op_set)
BOOST_AUTO_TEST_CASE(getters)
{
	ast::op_set ast_node{ {1,1}, "tname", value_term{42}, {{1,1},false}, {{1,1},true} };
	evtnodes::op_set snode(ast_node);
	BOOST_TEST(snode.rinfo().trim_left == false);
	BOOST_TEST(snode.rinfo().trim_right == true);
}
BOOST_AUTO_TEST_SUITE(render)
BOOST_FIXTURE_TEST_CASE(value, mock_exenv_fixture)
{
	ast::op_set ast_node{ {1,1}, "tname", value_term{42}, {{1,1},false}, {{1,1},true} };
	evtnodes::op_set snode(ast_node);
	MOCK_EXPECT(env.current_node).once().with(&snode);
	expect_glp(0, 1, 0);
	snode.render(env);
	BOOST_TEST(locals(ast::var_name{"tname"}) == value_term{42});
}
BOOST_FIXTURE_TEST_CASE(name, mock_exenv_fixture)
{
	using namespace cppjinja::ast;

	ast::op_set ast_node{ {1,1}, "tname", value_term{var_name{"a", "b"}}, {{1,1},false}, {{1,1},true} };
	evtnodes::op_set snode(ast_node);
	auto obj = std::make_shared<mocks::ctx_object>();
	locals.add("a", obj);
	expect_glp(0, 1, 0);
	MOCK_EXPECT(env.current_node).once().with(&snode);
	snode.render(env);
	BOOST_TEST(locals.find("tname").get() == obj.get());
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
	evtnodes::content_block cnt_bl({false, true});
	BOOST_TEST(cnt_bl.rinfo().trim_left == false);
	BOOST_TEST(cnt_bl.rinfo().trim_right == true);
}
BOOST_FIXTURE_TEST_CASE(render, mock_exenv_fixture)
{
	evtnodes::content_block cnt_bl({false, true});
	expect_children({});
	MOCK_EXPECT(env.crinfo).once().returns(cppjinja::evt::render_info{true, false});
	MOCK_EXPECT(env.current_node).once().with(&cnt_bl);
	BOOST_CHECK_NO_THROW(cnt_bl.render(env));
	BOOST_TEST(out.str() == "");
}
BOOST_FIXTURE_TEST_CASE(render_two, mock_exenv_fixture)
{
	evtnodes::content_block cnt_bl({false, true});
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
	expect_call(&cnt, {});
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
	expect_call(&cnt, {});
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
BOOST_FIXTURE_TEST_CASE(render_with_tabshift, mock_callable_fixture)
{
	using cppjinja::evt::result_formatter;
	ast::block_named ast_bl;
	ast_bl.right_open.bsign = -1;
	ast_bl.left_close.bsign = 2;
	evtnodes::block_named cnt(ast_bl);
	expect_children({});
	expect_call(&cnt, {});
	expect_cxt_settings(&cnt);
	MOCK_EXPECT(env.result).once().calls([this](){
		BOOST_TEST(rfmt("\na"s) == "\n\t\ta"s);
		return "ok"s;
	});
	cnt.render(env);
	BOOST_TEST(rfmt("\na"s) == "\n\ta"s);
}
BOOST_AUTO_TEST_CASE(params)
{
	ast::block_named ast_bl;
	BOOST_TEST(evtnodes::block_named(ast_bl).params() == ast_bl.params);
	ast_bl.params.emplace_back(ast::macro_parameter{"name"s, std::nullopt});
	BOOST_TEST(evtnodes::block_named(ast_bl).params() == ast_bl.params);
	ast_bl.params.emplace_back(ast::macro_parameter{"name"s, value_term{42}});
	BOOST_TEST(evtnodes::block_named(ast_bl).params() == ast_bl.params);
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
BOOST_FIXTURE_TEST_CASE(render_with_tabshift, mock_callable_fixture)
{
	using cppjinja::evt::render_info;
	using cppjinja::evt::result_formatter;
	ast::block_macro ast_bl;
	ast_bl.right_open.bsign = -1;
	ast_bl.left_close.bsign = 2;
	evtnodes::block_macro cnt(ast_bl);
	expect_children({&child1});
	MOCK_EXPECT(env.current_node);
	MOCK_EXPECT(env.rinfo);
	MOCK_EXPECT(child1.rinfo).returns(render_info{false,false});
	MOCK_EXPECT(child1.render).once().calls([this](cppjinja::evt::exenv&){
		BOOST_TEST(rfmt("\na"s) == "\n\t\ta"s);
	});
	cnt.evaluate(env);
	BOOST_TEST(rfmt("\na"s) == "\n\ta"s);
}
BOOST_AUTO_TEST_CASE(params)
{
	ast::block_macro ast_bl;
	BOOST_TEST(evtnodes::block_macro(ast_bl).params() == ast_bl.params);
	ast_bl.params.emplace_back(ast::macro_parameter{"name"s, std::nullopt});
	BOOST_TEST(evtnodes::block_macro(ast_bl).params() == ast_bl.params);
	ast_bl.params.emplace_back(ast::macro_parameter{"name"s, value_term{42}});
	BOOST_TEST(evtnodes::block_macro(ast_bl).params() == ast_bl.params);
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
	expect_glp(0, 1, 1);
	check_getters(bl, ast_bl);
	cppjinja::ast::macro_parameter p1{"a", value_term{"a"s}};
	cppjinja::ast::macro_parameter p2{"b", value_term{cppjinja::ast::var_name{"a"s}}};
	ast_bl.params = {p1, p2};
	std::tuple_element_t<1, T> blp(ast_bl);
	auto obj = std::make_shared<mocks::ctx_object>();
	locals.add("a", obj);
	MOCK_EXPECT(obj->solve).returns("b");
	auto solved = blp.solved_params(env);
	BOOST_TEST_REQUIRE(solved.size() == 2);
	BOOST_TEST(*solved[0].name == "a"s);
	BOOST_TEST(*solved[1].name == "b"s);
	BOOST_TEST(*solved[0].val == cppjinja::east::value_term{"a"});
	BOOST_TEST(*solved[1].val == cppjinja::east::value_term{"b"});
}
BOOST_AUTO_TEST_SUITE_END() // callables

BOOST_AUTO_TEST_SUITE(block_if)
BOOST_FIXTURE_TEST_CASE(getters, mock_exenv_fixture)
{
	ast::block_if ast;
	ast.left_open.trim = true;
	ast.right_close.trim = true;
	evtnodes::block_if bl(ast);
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
BOOST_FIXTURE_TEST_CASE(render_if_rinfo, mock_callable_fixture)
{
	ast::block_if ast_bl;
	ast_bl.condition = ast::binary_op(value_term{1}, ast::comparator::eq, value_term{1});
	ast_bl.left_close.trim = true;
	ast_bl.right_open.trim = false;
	evtnodes::block_if cnt(ast_bl);
	expect_children({&child1, &child2});
	MOCK_EXPECT(child1.render);
	MOCK_EXPECT(env.current_node);
	MOCK_EXPECT(env.rinfo).once().calls([](std::optional<cppjinja::evt::render_info> ri){
		BOOST_REQUIRE(ri.has_value());
		BOOST_TEST((bool)ri->trim_left == true);
		BOOST_TEST((bool)ri->trim_right == false);
	});
	cnt.render(env);

	ast_bl.else_block.emplace().left_open.trim = true;
	ast_bl.else_block->left_close.trim = false;
	cnt = evtnodes::block_if(ast_bl);
	MOCK_EXPECT(env.rinfo).once().calls([](std::optional<cppjinja::evt::render_info> ri){
		BOOST_REQUIRE(ri.has_value());
		BOOST_TEST((bool)ri->trim_left == true);
		BOOST_TEST((bool)ri->trim_right == true);
	});
	cnt.render(env);

	ast_bl.condition = ast::binary_op(value_term{2}, ast::comparator::eq, value_term{1});
	cnt = evtnodes::block_if(ast_bl);
	MOCK_EXPECT(child2.render).once();
	MOCK_EXPECT(env.rinfo).once().calls([](std::optional<cppjinja::evt::render_info> ri){
		BOOST_REQUIRE(ri.has_value());
		BOOST_TEST((bool)ri->trim_left == false);
		BOOST_TEST((bool)ri->trim_right == false);
	});
	cnt.render(env);
}
BOOST_FIXTURE_TEST_CASE(render_if_tabshift, mock_callable_fixture)
{
	ast::block_if ast_bl;
	ast_bl.condition = ast::binary_op(value_term{1}, ast::comparator::eq, value_term{1});
	ast_bl.left_close.bsign = 2;
	ast_bl.right_open.bsign = -1;
	evtnodes::block_if cnt(ast_bl);
	expect_children({&child1, &child2});
	MOCK_EXPECT(env.rinfo);
	MOCK_EXPECT(env.current_node);
	MOCK_EXPECT(child1.render).once().calls([this](cppjinja::evt::exenv&){
		BOOST_TEST(rfmt("\na"s) == "\n\t\ta"s);
	});
	cnt.render(env);
	BOOST_TEST(rfmt("\na"s) == "\n\ta"s);

	ast_bl.else_block.emplace().left_open.bsign = -3;
	cnt = evtnodes::block_if(ast_bl);
	MOCK_EXPECT(child1.render).once().calls([this](cppjinja::evt::exenv&){
		BOOST_TEST(rfmt("\na"s) == "\n\t\t\ta"s);
	});
	cnt.render(env);
	BOOST_TEST(rfmt("\na"s) == "\na"s);
}
BOOST_FIXTURE_TEST_CASE(render_else_tabshift, mock_callable_fixture)
{
	ast::block_if ast_bl;
	ast_bl.condition = ast::binary_op(value_term{2}, ast::comparator::eq, value_term{1});
	ast_bl.else_block.emplace().left_close.bsign = 2;
	ast_bl.right_open.bsign = -1;
	evtnodes::block_if cnt(ast_bl);
	expect_children({&child1, &child2});
	MOCK_EXPECT(env.rinfo);
	MOCK_EXPECT(env.current_node);
	MOCK_EXPECT(child2.render).once().calls([this](cppjinja::evt::exenv&){
		BOOST_TEST(rfmt("\na"s) == "\n\t\ta"s);
	});
	cnt.render(env);
	BOOST_TEST(rfmt("\na"s) == "\n\ta"s);
}
BOOST_AUTO_TEST_SUITE_END() // block_if

BOOST_AUTO_TEST_SUITE_END() // nodes
BOOST_AUTO_TEST_SUITE_END() // phase_evaluate
