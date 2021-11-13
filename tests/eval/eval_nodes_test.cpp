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
#include "evtree/nodes/block_filtered.hpp"
#include "evtree/nodes/block_set.hpp"
#include "evtree/nodes/block_call.hpp"
#include "evtree/evtree.hpp"
#include "parser/operators/blocks.hpp"
#include "parser/parse.hpp"
#include "parser/grammar/expr.hpp"

#include "evtree/exenv/context_objects/callable_node.hpp"

using namespace std::literals;
namespace tdata = boost::unit_test::data;

using evt_node = cppjinja::evt::node;
namespace evtnodes = cppjinja::evtnodes;
namespace ast = cppjinja::ast;
namespace aps = cppjinja::ast::expr_ops;
using mocks::mock_exenv_fixture; // qtcreator cannot parse test with namespace

struct mock_callable_fixture : mock_exenv_fixture {
	mocks::node child1, child2;

	template<typename Ast>
	void prepare_ast_for_params(Ast& ast_bl)
	{
		ast_bl.params.emplace_back(ast::macro_parameter{"p1", aps::expr{aps::term{41}}});
		ast_bl.params.emplace_back(ast::macro_parameter{"p2", aps::expr{aps::term{42}}});
		ast_bl.params.emplace_back(ast::macro_parameter{"p3", std::nullopt});
	}

	template<typename Ast>
	void prepare_for_render_two_childrend(Ast& ast_bl)
	{
		ast_bl.left_close.trim = true;
		ast_bl.right_open.trim = true;
	}

	template<typename B, typename Ast>
	void check_getters(const B& bl, const Ast& ast_bl)
	{
		BOOST_TEST(bl.name().substr(0, ast_bl.name.name.size()) == ast_bl.name.name);
		BOOST_TEST(bl.rinfo().trim_left == ast_bl.left_open.trim);
		BOOST_TEST(bl.rinfo().trim_right == ast_bl.right_close.trim);
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
	t.name.name = "test_name";
	t.file_name = "file_name";
	t.file_imports.emplace_back(ast::op_import{0, 0, "fn", "as", "tn", {}, {}});
	t.extends.emplace_back(ast::extend_st{0,0, {}, ast::expr_ops::single_var_name{.name="par"}});
	evtnodes::tmpl tmpl(t);
	BOOST_CHECK( !tmpl.rinfo().trim_left );
	BOOST_CHECK( !tmpl.rinfo().trim_right );
	BOOST_TEST( tmpl.file_name() == "file_name" );
	BOOST_REQUIRE( tmpl.imports().size() ==1 );
	BOOST_TEST( tmpl.imports()[0].filename == "fn" );
	BOOST_TEST( tmpl.imports()[0].as == "as" );
	BOOST_TEST( tmpl.imports()[0].tmpl_name == "tn" );
	test_name_equals( tmpl, t );
	BOOST_TEST_REQUIRE( tmpl.parents().size() == 1);
	BOOST_TEST( tmpl.parents()[0] == "par");
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
	evtnodes::tmpl tmpl(ast::tmpl{});
	mock::sequence ctx_seq;
	mocks::callable_node child_with_name, child_empty_name;
	expect_glp(0, 0, 0);
	expect_call(&child_empty_name);
	expect_roots({&child_with_name, &child_empty_name});
	MOCK_EXPECT(globals.add);
	MOCK_EXPECT(env.current_node).with(&tmpl);
	MOCK_EXPECT(child_empty_name.name).at_least(1).returns("");
	MOCK_EXPECT(child_with_name.name).at_least(1).returns("tn");
	MOCK_EXPECT(child_empty_name.evaluate).once().returns("test"_ad);
	tmpl.render(env);
	BOOST_TEST(out.str() == "test");
}
BOOST_AUTO_TEST_SUITE_END() // tmpl

BOOST_AUTO_TEST_SUITE(op_set)
BOOST_AUTO_TEST_CASE(getters)
{
	aps::eq_assign asg;
	asg.names.emplace_back(aps::single_var_name{"a"s});
	ast::op_set ast_node{ {1,1}, asg, {{1,1},false}, {{1,1},true} };
	evtnodes::op_set snode(ast_node);
	BOOST_TEST(snode.rinfo().trim_left == false);
	BOOST_TEST(snode.rinfo().trim_right == true);
}
BOOST_AUTO_TEST_SUITE(render)
using cppjinja::evt::context_object;
BOOST_FIXTURE_TEST_CASE(value, mock_exenv_fixture)
{
	aps::eq_assign op;
	op.value = aps::expr{aps::term{42}};
	op.names.emplace_back(aps::single_var_name{"tname"s});
	ast::op_set ast_node{ {1,1}, std::move(op), {{1,1},false}, {{1,1},true} };
	evtnodes::op_set snode(ast_node);
	expect_glp(0, 1, 0);
	auto check = [](std::shared_ptr<context_object> v){return v->solve() == 42;};
	MOCK_EXPECT(env.current_node).once().with(&snode);
	MOCK_EXPECT(locals.add).once().with("tname", check);
	snode.render(env);
}
BOOST_FIXTURE_TEST_CASE(name, mock_exenv_fixture)
{
	using namespace cppjinja::ast;

	aps::eq_assign op;
	op.value = aps::expr{aps::single_var_name{"a"s}};
	op.names.emplace_back(aps::single_var_name{"tname"s});
	ast::op_set ast_node{ {1,1}, op, {{1,1},false}, {{1,1},true} };
	evtnodes::op_set snode(ast_node);
	auto obj = std::make_shared<mocks::context_object>();
	expect_glp(0, 1, 0);
	expect_solve(*obj, "a");
	expect_local_object_created("tname", "a");
	MOCK_EXPECT(all_ctx.find).with("a"_s).returns(obj);
	MOCK_EXPECT(env.current_node).once().with(&snode);
	snode.render(env);
}
BOOST_FIXTURE_TEST_CASE(call, mock_exenv_fixture)
{
	using namespace cppjinja::ast;

	aps::eq_assign op;
	op.value = aps::expr{aps::fnc_call{aps::lvalue{aps::single_var_name{"a"s}}, {}}};
	op.names.emplace_back(aps::single_var_name{"tname"s});
	ast::op_set ast_node{ {1,1}, op, {{1,1},false}, {{1,1},true} };
	evtnodes::op_set snode(ast_node);
	auto obj = std::make_shared<mocks::context_object>();
	auto gotten_obj = std::make_shared<mocks::context_object>();
	expect_glp(0, 1, 0);
	expect_solve(*gotten_obj, "test");
	expect_local_object_created("tname", "test");
	MOCK_EXPECT(all_ctx.find).with("a"_s).returns(obj);
	MOCK_EXPECT(obj->call).once().returns(gotten_obj);
	MOCK_EXPECT(env.current_node).once().with(&snode);
	snode.render(env);
}
BOOST_FIXTURE_TEST_CASE(few_names, mock_exenv_fixture)
{
	aps::eq_assign op;
	op.value = aps::expr{aps::tuple{{aps::expr{aps::term{3}}, aps::expr{aps::term{5}}}}};
	op.names.emplace_back(aps::single_var_name{"a"s});
	op.names.emplace_back(aps::single_var_name{"b"s});
	ast::op_set ast_node{ {1,1}, op, {{1,1},false}, {{1,1},true} };
	evtnodes::op_set snode(ast_node);
	mock::sequence seq;
	expect_glp(0, 1, 0);
	MOCK_EXPECT(locals.add)
	        .in(seq).once()
	        .calls([](std::pmr::string n, std::shared_ptr<cppjinja::evt::context_object> obj)
	{
		BOOST_TEST(n == "a");
		BOOST_TEST(obj->solve() == 3);
	});
	MOCK_EXPECT(locals.add)
	        .in(seq).once()
	        .calls([](std::pmr::string n, std::shared_ptr<cppjinja::evt::context_object> obj)
	{
		BOOST_TEST(n == "b");
		BOOST_TEST(obj->solve() == 5);
	});
	MOCK_EXPECT(env.current_node).once().with(&snode);
	snode.render(env);
}
BOOST_FIXTURE_TEST_CASE(no_names, mock_exenv_fixture)
{
	BOOST_CHECK_THROW(evtnodes::op_set(ast::op_set{}), std::exception);
}
BOOST_AUTO_TEST_SUITE_END() // render
BOOST_AUTO_TEST_SUITE_END() // op_set

BOOST_AUTO_TEST_SUITE(op_out)
BOOST_AUTO_TEST_CASE(getters)
{
	ast::op_out ast_out{ {1,1}, aps::expr{aps::term{42}}, {{1,1},false}, {{1,1},true} };
	evtnodes::op_out snode(ast_out);
	BOOST_TEST(snode.rinfo().trim_left == false);
	BOOST_TEST(snode.rinfo().trim_right == true);
}
BOOST_FIXTURE_TEST_CASE(render, mock_exenv_fixture)
{
	ast::op_out ast_out{ {1,1}, aps::expr{aps::term{42}}, {{1,1},false}, {{1,1},true} };
	evtnodes::op_out snode(ast_out);
	MOCK_EXPECT(env.current_node).once().with(&snode);
	BOOST_CHECK_NO_THROW(snode.render(env));
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
	expect_call(&cnt);
	MOCK_EXPECT(env.result).once().returns("result"_ad);
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
	expect_call(&cnt);
	prepare_for_render_two_childrend();
	MOCK_EXPECT(env.result).once().returns("result"_ad);
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
	MOCK_EXPECT(env.result).once().returns("result"_ad);
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
	expect_call(&cnt);
	expect_cxt_settings(&cnt);
	MOCK_EXPECT(env.result).once().calls([this](){
		BOOST_TEST(rfmt("\na"_s) == "\n\t\ta"_s);
		return "ok"_ad;
	});
	cnt.render(env);
	BOOST_TEST(rfmt("\na"_s) == "\n\ta"_s);
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
	expect_transporent_cxt(&cnt);
	MOCK_EXPECT(env.rinfo);
	MOCK_EXPECT(child1.rinfo).returns(render_info{false,false});
	MOCK_EXPECT(child1.render).once().calls([this](cppjinja::evt::exenv&){
		BOOST_TEST(rfmt("\na"_s) == "\n\t\ta"_s);
	});
	cnt.evaluate(env);
	BOOST_TEST(rfmt("\na"_s) == "\n\ta"_s);
}
BOOST_AUTO_TEST_SUITE_END() // block_macro

BOOST_AUTO_TEST_SUITE(callables)
typedef std::tuple<
  std::tuple<ast::block_named*, evtnodes::block_named*>
, std::tuple<ast::block_macro*, evtnodes::block_macro*>
, std::tuple<ast::block_call*, evtnodes::block_call*>
> callable_list;
BOOST_FIXTURE_TEST_CASE_TEMPLATE(getters, T, callable_list, mock_callable_fixture)
{
	std::remove_pointer_t<std::tuple_element_t<0, T>> ast_bl;
	ast_bl.name.name = "test_name";
	std::remove_pointer_t<std::tuple_element_t<1, T>> bl(ast_bl);
	check_getters(bl, ast_bl);
}
BOOST_FIXTURE_TEST_CASE_TEMPLATE(sovled_params, T, callable_list, mock_callable_fixture)
{
	std::remove_pointer_t<std::tuple_element_t<0, T>> ast_bl;
	ast_bl.name.name = "test_name";
	std::remove_pointer_t<std::tuple_element_t<1, T>> bl(ast_bl);

	ast_bl.params.emplace_back(ast::macro_parameter{"a", aps::expr{aps::term{"a"s}}});
	ast_bl.params.emplace_back(ast::macro_parameter{"b", aps::expr{aps::single_var_name{"a"s}}});
	std::remove_pointer_t<std::tuple_element_t<1, T>> blp(ast_bl);
	auto obj = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).with("a"_s).returns(obj);
	MOCK_EXPECT(obj->solve).returns("b"_ad);

	auto solved = blp.solved_params(env);
	BOOST_TEST_REQUIRE(solved.size() == 2);
	BOOST_TEST(*solved[0].name == "a"_s);
	BOOST_TEST(*solved[1].name == "b"_s);
	BOOST_TEST(*solved[0].val == "a"_s);
	BOOST_TEST(*solved[1].val == "b"_s);
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
	ast_bl.condition = ast::expr_ops::expr_bool{ast::expr_ops::term{true}};
	prepare_for_render_two_childrend(ast_bl);
	evtnodes::block_if bl(ast_bl);
	expect_transporent_cxt(&bl);
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
	ast_bl.condition = ast::expr_ops::expr_bool{ast::expr_ops::term{false}};
	prepare_for_render_two_childrend(ast_bl);
	evtnodes::block_if bl(ast_bl);
	expect_transporent_cxt(&bl);
	expect_children({&child1});
	BOOST_CHECK_NO_THROW(bl.render(env));
	BOOST_TEST(out.str() == "");
}
BOOST_FIXTURE_TEST_CASE(render_false, mock_callable_fixture)
{
	ast::block_if ast_bl;
	ast_bl.condition = ast::expr_ops::expr_bool{ast::expr_ops::term{false}};
	ast_bl.else_block.emplace().left_open.trim = true;
	prepare_for_render_two_childrend(ast_bl);
	evtnodes::block_if bl(ast_bl);
	expect_transporent_cxt(&bl);
	expect_children({&child1, &child2});
	MOCK_EXPECT(child2.render).once();
	MOCK_EXPECT(env.rinfo).once();
	BOOST_CHECK_NO_THROW(bl.render(env));
	BOOST_TEST(out.str() == "");
}
BOOST_FIXTURE_TEST_CASE(render_if_rinfo, mock_callable_fixture)
{
	ast::block_if ast_bl;
	ast_bl.condition = ast::expr_ops::expr_bool{ast::expr_ops::term{true}};
	ast_bl.left_close.trim = true;
	ast_bl.right_open.trim = false;
	evtnodes::block_if cnt(ast_bl);
	expect_children({&child1, &child2});
	MOCK_EXPECT(child1.render);
	MOCK_EXPECT(env.rinfo).once().calls([](std::optional<cppjinja::evt::render_info> ri){
		BOOST_REQUIRE(ri.has_value());
		BOOST_TEST((bool)ri->trim_left == true);
		BOOST_TEST((bool)ri->trim_right == false);
	});
	expect_transporent_cxt(&cnt);
	cnt.render(env);

	ast_bl.else_block.emplace().left_open.trim = true;
	ast_bl.else_block->left_close.trim = false;
	cnt = evtnodes::block_if(ast_bl);
	MOCK_EXPECT(env.rinfo).once().calls([](std::optional<cppjinja::evt::render_info> ri){
		BOOST_REQUIRE(ri.has_value());
		BOOST_TEST((bool)ri->trim_left == true);
		BOOST_TEST((bool)ri->trim_right == true);
	});
	expect_transporent_cxt(&cnt);
	cnt.render(env);

	ast_bl.condition = ast::expr_ops::expr_bool{ast::expr_ops::term{false}};
	cnt = evtnodes::block_if(ast_bl);
	MOCK_EXPECT(child2.render).once();
	MOCK_EXPECT(env.rinfo).once().calls([](std::optional<cppjinja::evt::render_info> ri){
		BOOST_REQUIRE(ri.has_value());
		BOOST_TEST((bool)ri->trim_left == false);
		BOOST_TEST((bool)ri->trim_right == false);
	});
	expect_transporent_cxt(&cnt);
	cnt.render(env);
}
BOOST_FIXTURE_TEST_CASE(render_if_tabshift, mock_callable_fixture)
{
	ast::block_if ast_bl;
	ast_bl.condition = ast::expr_ops::expr_bool{ast::expr_ops::term{true}};
	ast_bl.left_close.bsign = 2;
	ast_bl.right_open.bsign = -1;
	evtnodes::block_if cnt(ast_bl);
	expect_children({&child1, &child2});
	MOCK_EXPECT(env.rinfo);
	MOCK_EXPECT(child1.render).once().calls([this](cppjinja::evt::exenv&){
		BOOST_TEST(rfmt("\na"_s) == "\n\t\ta"_s);
	});
	expect_transporent_cxt(&cnt);
	cnt.render(env);
	BOOST_TEST(rfmt("\na"_s) == "\n\ta"_s);

	ast_bl.else_block.emplace().left_open.bsign = -3;
	cnt = evtnodes::block_if(ast_bl);
	MOCK_EXPECT(child1.render).once().calls([this](cppjinja::evt::exenv&){
		BOOST_TEST(rfmt("\na"_s) == "\n\t\t\ta"_s);
	});
	expect_transporent_cxt(&cnt);
	cnt.render(env);
	BOOST_TEST(rfmt("\na"_s) == "\na"_s);
}
BOOST_FIXTURE_TEST_CASE(render_else_tabshift, mock_callable_fixture)
{
	ast::block_if ast_bl;
	ast_bl.condition = ast::expr_ops::expr_bool{ast::expr_ops::term{false}};
	ast_bl.else_block.emplace().left_close.bsign = 2;
	ast_bl.right_open.bsign = -1;
	evtnodes::block_if cnt(ast_bl);
	expect_children({&child1, &child2});
	MOCK_EXPECT(env.rinfo);
	MOCK_EXPECT(child2.render).once().calls([this](cppjinja::evt::exenv&){
		BOOST_TEST(rfmt("\na"_s) == "\n\t\ta"_s);
	});
	expect_transporent_cxt(&cnt);
	cnt.render(env);
	BOOST_TEST(rfmt("\na"_s) == "\n\ta"_s);
}
BOOST_FIXTURE_TEST_CASE(render_elif, mock_callable_fixture)
{
	ast::block_if ast_bl;
	ast_bl.condition = ast::expr_ops::expr_bool{ast::expr_ops::term{false}};
	evtnodes::block_if if1(ast_bl);
	ast_bl.condition = ast::expr_ops::expr_bool{ast::expr_ops::term{false}};
	evtnodes::block_if if2(ast_bl);
	ast_bl.condition = ast::expr_ops::expr_bool{ast::expr_ops::term{true}};
	evtnodes::block_if if3(ast_bl);
	mocks::node if1_cnt, if2_cnt, if3_cnt;
	expect_children(&if1, {&if1_cnt, &if2});
	expect_children(&if2, {&if2_cnt, &if3});
	expect_children(&if3, {&if3_cnt});
	expect_transporent_cxt(&if1);
	expect_transporent_cxt(&if2);
	expect_transporent_cxt(&if3);
	MOCK_EXPECT(if3_cnt.render).once(); // the enity of test: render the first child of if3
	MOCK_EXPECT(env.rinfo);

	if1.render(env);
}
BOOST_AUTO_TEST_SUITE_END() // block_if

BOOST_AUTO_TEST_SUITE(block_filter)
BOOST_FIXTURE_TEST_CASE(getters, mock_callable_fixture)
{
	ast::block_filtered ast_bl;
	ast_bl.left_open.trim = true;
	ast_bl.right_close.trim = true;
	cppjinja::evtnodes::block_filtered block(ast_bl);
	BOOST_TEST(block.rinfo().trim_left == true);
	BOOST_TEST(block.rinfo().trim_right == true);
}
BOOST_FIXTURE_TEST_CASE(render, mock_callable_fixture)
{
	using fnc_param_t = cppjinja::evt::context_object::function_parameter;
	using val_obj_t = cppjinja::evt::context_objects::value;
	ast::block_filtered ast_bl;
	ast_bl.left_close.trim = true;
	ast_bl.right_open.trim = true;
	ast_bl.flt = ast::expr_ops::filter_call{ast::expr_ops::lvalue{ast::expr_ops::single_var_name{"flt"}}, {}};
	ast_bl.content.emplace_back(ast::block_content{"base"s});
	cppjinja::evtnodes::block_filtered block(ast_bl);

	mocks::node child;
	MOCK_EXPECT(child.render).once();
	MOCK_EXPECT(env.result).once().returns("base"_ad);

	expect_transporent_cxt(&block);
	expect_children({&child});
	expect_call(
	              "flt"_s
	            , {fnc_param_t{"$"_s, std::make_shared<val_obj_t>("base"_ad)}}
	            , "after"_ad);
	MOCK_EXPECT(env.rinfo).with(cppjinja::evt::render_info{true, true});
	block.render(env);
	BOOST_TEST(out.str() == "after"s);
}
BOOST_AUTO_TEST_SUITE_END() // block_filter

BOOST_AUTO_TEST_SUITE(block_set)
using cppjinja::evt::context_object;
BOOST_FIXTURE_TEST_CASE(getters, mock_callable_fixture)
{
	ast::block_set ast_bl;
	ast_bl.left_open.trim = true;
	ast_bl.right_close.trim = true;
	cppjinja::evtnodes::block_set block(ast_bl);
	BOOST_TEST(block.rinfo().trim_left == true);
	BOOST_TEST(block.rinfo().trim_right == true);
}
BOOST_FIXTURE_TEST_CASE(render, mock_callable_fixture)
{
	ast::block_set ast_bl;
	ast_bl.left_close.trim = true;
	ast_bl.right_open.trim = true;
	ast_bl.name.name = "tname"s;
	cppjinja::evtnodes::block_set block(ast_bl);

	mocks::node child;
	expect_glp(0, 1, 0);
	expect_children({&child});
	expect_transporent_cxt(&block);
	MOCK_EXPECT(env.rinfo).once().with(cppjinja::evt::render_info{true, true});
	MOCK_EXPECT(child.render).once().calls([](auto& e){e.out() << "kuku";});
	MOCK_EXPECT(env.result).returns("ok"_ad);

	auto check = [](std::shared_ptr<context_object> v){
		return v->solve() == "ok"s;};
	MOCK_EXPECT(locals.add).once().with("tname", check);

	block.render(env);
	BOOST_TEST(out.str() == "kuku"s);
}
BOOST_AUTO_TEST_SUITE_END() // block_set

BOOST_AUTO_TEST_SUITE(block_call)
BOOST_FIXTURE_TEST_CASE(render, mock_callable_fixture)
{
	ast::block_call ast_bl;
	ast_bl.name.name = "test";
	ast_bl.call_params.emplace_back(
	            cppjinja::ast::macro_parameter{
	                "cp1n"s,
	                cppjinja::ast::expr_ops::expr{cppjinja::ast::expr_ops::term{"cp1v"s}}
	            });
	cppjinja::evtnodes::block_call block(ast_bl);

	// calls other block
	auto calling_result = std::make_shared<mocks::context_object>();
	auto calling = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(all_ctx.find).once().with("test"_s).returns(calling);
	MOCK_EXPECT(calling_result->solve).returns("calling_result"_ad);
	MOCK_EXPECT(calling->call).once().calls([&block,calling_result](auto params){
		BOOST_TEST_REQUIRE(params.size()==2);
		BOOST_TEST(params[1].name.value_or(""_s) == "cp1n"_s);
		BOOST_TEST(params[1].value->solve() == "cp1v"_s);
		auto* fp = dynamic_cast<cppjinja::evt::context_objects::callable_node*>(params[0].value.get());
		BOOST_REQUIRE(fp);
		BOOST_CHECK(fp->is_it(&block));
		return calling_result;
	});

	block.render(env);
	BOOST_TEST(out.str() == "calling_result"s);
}
BOOST_FIXTURE_TEST_CASE(evaluate, mock_callable_fixture)
{
	ast::block_call ast_bl;
	ast_bl.name.name = "test";
	ast_bl.left_close.trim = true;
	ast_bl.right_open.trim = true;
	cppjinja::evtnodes::block_call block(ast_bl);
	expect_cxt_settings(&block);
	prepare_for_render_two_childrend();
	MOCK_EXPECT(env.result).once().returns("result"_ad);
	BOOST_TEST(block.evaluate(env) == "result"s);
}
BOOST_AUTO_TEST_SUITE_END() // block_call

BOOST_AUTO_TEST_SUITE_END() // nodes
BOOST_AUTO_TEST_SUITE_END() // phase_evaluate
