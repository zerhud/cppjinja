/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_OLD_TOOLS
#define BOOST_TEST_MODULE evaluator exenv

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "mocks.hpp"
#include "evtree/node.hpp"
#include "evtree/evtree.hpp"
#include "evtree/exenv/impl.hpp"
#include "evtree/exenv/context_impl.hpp"
#include "evtree/exenv/callstack_impl.hpp"
#include "evtree/exenv/context_objects/tree.hpp"
#include "evtree/exenv/context_objects/value.hpp"
#include "evtree/exenv/context_objects/callable_node.hpp"
#include "evtree/exenv/context_objects/user_data.hpp"
#include "evtree/exenv/context_objects/inner_navigation.hpp"
#include "evtree/exenv/context_objects/builtins/lambda.hpp"
#include "evtree/nodes/callable.hpp"
#include "parser/operators/common.hpp"
#include "parser/grammar/tmpls.hpp"
#include "parser/parse.hpp"

using namespace std::literals;
using namespace absd::literals;
namespace tdata = boost::unit_test::data;
namespace east = cppjinja::east;
namespace ast = cppjinja::ast;
namespace evtn = cppjinja::evtnodes;

using evar_name = cppjinja::east::var_name;
using mocks::mock_exenv_fixture; // qtcreator cannot parse test with namespace
using co_nav_imp = cppjinja::evt::context_objects::navigation_imp;
using co_nav_tmpl = cppjinja::evt::context_objects::navigation_tmpl;
using co_nav_stmpl = cppjinja::evt::context_objects::navigation_single_tmpl;

struct mock_impls_fixture {
	cppjinja::evt::callstack_impl calls;
	cppjinja::evt::context_impl ctx;
};

struct mock_env_fixture
{
	mocks::data_provider data;
	cppjinja::evtree compiled;
};

struct impl_exenv_fixture : mock_env_fixture
{
	cppjinja::evt::exenv_impl env;
	cppjinja::evt::context_impl& ctx;
	cppjinja::evt::callstack_impl& calls;
	impl_exenv_fixture()
	    : env(&data, &compiled)
	    , ctx(static_cast<cppjinja::evt::context_impl&>(env.ctx()))
	    , calls(static_cast<cppjinja::evt::callstack_impl&>(env.calls()))
	{}
};

BOOST_AUTO_TEST_SUITE(phase_evaluate)

BOOST_AUTO_TEST_SUITE(context_object)
using cppjinja::east::var_name;
using cppjinja::evt::context_objects::tree;
BOOST_AUTO_TEST_SUITE(obj_tree)
std::string prt(const absd::data& d)
{
	std::stringstream out;
	out << d;
	return out.str();
}
BOOST_AUTO_TEST_CASE(find)
{
	tree obj;
	auto child1 = std::make_shared<tree>();
	auto child2 = std::make_shared<tree>();
	obj.add("child1"_s, child1);
	BOOST_TEST(obj.find("no") == nullptr);
	BOOST_TEST(obj.find("child1") == child1);
	obj.add("child2"_s, child2);
	BOOST_TEST(obj.find("no") == nullptr);
	BOOST_TEST(obj.find("child2") == child2);
}
BOOST_AUTO_TEST_CASE(solve)
{
	using covalue = cppjinja::evt::context_objects::value;

	tree obj;
	BOOST_TEST(prt(obj.solve()) == "{}");
	obj.add("c1", std::make_shared<tree>());
	BOOST_TEST(prt(obj.solve()) == "{\"c1\":{}}");
	obj.add("c2", std::make_shared<tree>());
	BOOST_TEST(prt(obj.solve()) == "{\"c1\":{},\"c2\":{}}");
	obj.find("c2")->add("c3", std::make_shared<tree>());
	obj.find("c2")->add("c4", std::make_shared<covalue>("ok"_ad));
	BOOST_TEST(prt(obj.solve()) == "{\"c1\":{},\"c2\":{\"c3\":{},\"c4\":\"ok\"}}");
}
BOOST_AUTO_TEST_CASE(cannot_call)
{
	tree obj;
	BOOST_CHECK_THROW(obj.call({}), std::exception);
}
BOOST_AUTO_TEST_SUITE_END() // tree
BOOST_AUTO_TEST_SUITE(obj_value)
using cppjinja::evt::context_objects::value;
BOOST_AUTO_TEST_CASE(no_tree_methods_and_no_call)
{
	value val("ok"_ad);
	BOOST_CHECK_THROW(val.add("a",std::make_shared<tree>()), std::exception);
	BOOST_CHECK_THROW(val.find("a"), std::exception);
	BOOST_CHECK_THROW(val.call({}), std::exception);
}
BOOST_AUTO_TEST_CASE(solve)
{
	BOOST_TEST(value("ok"_ad).solve() == "ok"_s);
	BOOST_TEST(value(4_ad).solve() == 4);
}
BOOST_AUTO_TEST_CASE(items)
{
	using namespace absd::literals;
	auto obj = std::make_shared<absd::simple_data_holder>();
	obj->put("a", "a_val"_sd);
	obj->put("b", "b_val"_sd);
	obj->put("c", "c_val"_sd);

	auto val_simple = std::make_shared<value>(absd::data{obj});
	BOOST_TEST(val_simple->find("a")->solve() == "a_val"s);
	BOOST_TEST(val_simple->find("items")->call({})->solve() == absd::data{obj});

	obj->put("items", "surprise"_sd);
	value val_items(absd::data{obj});
	BOOST_TEST(val_items.find("items")->solve() == "surprise"s);
}
BOOST_AUTO_TEST_SUITE_END() // value
BOOST_AUTO_TEST_SUITE(callable_node)
BOOST_AUTO_TEST_CASE(no_tree_methods_and_no_solve)
{
	mocks::exenv env;
	mocks::callable_node node;
	cppjinja::evt::context_objects::callable_node val(&env, &node);
	BOOST_CHECK_THROW(val.add("a",std::make_shared<tree>()), std::exception);
	BOOST_CHECK_THROW(val.find("a"), std::exception);
	BOOST_CHECK_THROW(val.solve(), std::exception);
}
BOOST_AUTO_TEST_CASE(is_it)
{
	mocks::exenv env;
	mocks::callable_node node;
	cppjinja::evt::context_objects::callable_node val(&env, &node);
	BOOST_TEST( val.is_it(&node) == true );
}
BOOST_FIXTURE_TEST_CASE(call, mock_exenv_fixture)
{
	mocks::callable_node node;
	cppjinja::evt::context_objects::callable_node val(&env, &node);
	mock::sequence seq;
	cppjinja::evt::context_object::function_parameter p1{"a"_s, std::make_shared<mocks::context_object>()};
	MOCK_EXPECT(calls.push).once().in(seq).calls([&node,&p1](auto*n, auto params){
		BOOST_TEST(n == &node);
		auto sp1 = params.find("a");
		BOOST_TEST(sp1 == p1.value);
	});
	MOCK_EXPECT(node.evaluate)
	        .once()
	        .in(seq)
	        .with([this](auto& e){return &e==&env;})
	        .returns("ok"_ad);
	MOCK_EXPECT(calls.pop).once().in(seq);
	MOCK_EXPECT(node.solved_params)
	        .at_least(1).returns(std::pmr::vector<cppjinja::east::function_parameter>{});
	BOOST_TEST(val.call({p1})->solve() == "ok"_s);
}
BOOST_AUTO_TEST_SUITE_END() // callable_node
BOOST_AUTO_TEST_SUITE(callable_params)
BOOST_AUTO_TEST_CASE(cannot_solve_call)
{
	cppjinja::evt::context_objects::callable_params obj({}, {});
	BOOST_CHECK_THROW(obj.call({}), std::exception);
	BOOST_CHECK_THROW(obj.solve(), std::exception);
}
BOOST_AUTO_TEST_CASE(add)
{
	cppjinja::evt::context_objects::callable_params obj({}, {});
	BOOST_CHECK_THROW(obj.add("a", nullptr), std::exception);
	auto par = std::make_shared<mocks::context_object>();
	BOOST_TEST(obj.find("a") == nullptr);
	obj.add("a", par);
	BOOST_TEST(obj.find("a") == par);
}
BOOST_AUTO_TEST_CASE(add_placed)
{
	auto param = std::make_shared<mocks::context_object>();
	cppjinja::evt::context_objects::callable_params obj1({}, {});
	BOOST_CHECK_THROW(obj1.add_placed(0, param), std::exception);

	using cppjinja::east::function_parameter;
	std::pmr::vector<function_parameter> expected;
	expected.emplace_back(function_parameter{"name", "orig"_ad});
	cppjinja::evt::context_objects::callable_params obj2(expected, {});
	obj2.add_placed(0, param);
	BOOST_TEST(obj2.find("name") == param);
}
BOOST_AUTO_TEST_CASE(params_objects)
{
	using cppjinja::east::function_parameter;
	function_parameter p1{"p1", std::nullopt};
	function_parameter p2{"p2", 52_ad};
	function_parameter p4{"p4", 54_ad};
	function_parameter p5{"p5", 55_ad};

	using cfnc_param = cppjinja::evt::context_object::function_parameter;
	cfnc_param c1{std::nullopt, std::make_shared<mocks::context_object>()};
	cfnc_param c2{"p2"_s, std::make_shared<mocks::context_object>()};
	cfnc_param c3{"p3"_s, std::make_shared<mocks::context_object>()};
	cfnc_param c4{std::nullopt, std::make_shared<mocks::context_object>()};

	cppjinja::evt::context_objects::callable_params obj({p1, p2, p4, p5}, {c1, c2, c4, c3});

	BOOST_TEST(obj.find("p1") == c1.value);
	BOOST_TEST(obj.find("p2") == c2.value);
	BOOST_TEST(obj.find("p3") == c3.value);
	BOOST_TEST(obj.find("p4") == c4.value);
	BOOST_TEST(obj.find("p5")->solve() == 55);
	BOOST_TEST(obj.find("P1") == nullptr);
}
BOOST_AUTO_TEST_SUITE_END() // callable_params
BOOST_AUTO_TEST_SUITE(queue)
BOOST_AUTO_TEST_CASE(from_queue)
{
	mocks::context_object obj1, obj2, obj3;
	cppjinja::evt::context_objects::queue q1({&obj1, &obj2});
	cppjinja::evt::context_objects::queue q2({&obj3});
	cppjinja::evt::context_objects::queue q({q1, q2});

	MOCK_EXPECT(obj1.find).once().with("a").returns(nullptr);
	MOCK_EXPECT(obj2.find).once().with("a").returns(nullptr);
	MOCK_EXPECT(obj3.find).once().with("a").returns(nullptr);
	BOOST_TEST(q.find("a") == nullptr);

	auto obj4 = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(obj1.find).once().with("a").returns(nullptr);
	MOCK_EXPECT(obj2.find).once().with("a").returns(nullptr);
	MOCK_EXPECT(obj3.find).once().with("a").returns(obj4);
	BOOST_TEST(q.find("a") == obj4);
}
BOOST_AUTO_TEST_CASE(find)
{
	mocks::context_object obj1, obj2;
	auto obj3 = std::make_shared<mocks::context_object>();
	cppjinja::evt::context_objects::queue q({&obj1, &obj2});

	BOOST_CHECK_THROW(q.solve(), std::exception);
	BOOST_CHECK_THROW(q.call({}), std::exception);

	MOCK_EXPECT(obj1.find).once().with("a").returns(nullptr);
	MOCK_EXPECT(obj2.find).once().with("a").returns(nullptr);
	BOOST_TEST(q.find("a") == nullptr);

	auto obj4 = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(obj1.find).once().with("a").returns(nullptr);
	MOCK_EXPECT(obj2.find).once().with("a").returns(obj4);
	BOOST_TEST(q.find("a") == obj4);

	using cppjinja::evt::context_object;
	using cppjinja::evt::context_objects::queue;
	MOCK_EXPECT(obj1.find).once().with("a").returns(nullptr);
	MOCK_EXPECT(obj2.find).once().with("a").returns(obj4);
	queue other1 = queue::clist{&obj1, &obj2};
	cppjinja::evt::context_objects::queue other = std::move(other1);
	BOOST_TEST(other.find("a") == obj4);
}
BOOST_AUTO_TEST_CASE(add)
{
	using cppjinja::evt::context_objects::queue;
	queue q;
	auto obj1 = std::make_shared<mocks::context_object>();
	BOOST_CHECK_THROW(q.add("", nullptr), std::exception);
	BOOST_CHECK_THROW(q.add("a", obj1), std::exception);

	auto obj2 = std::make_shared<mocks::context_object>();
	q = queue::clist{obj1.get()};
	BOOST_CHECK_THROW(q.add("a", obj1), std::exception);

	q = {obj1.get(), obj2.get()};
	MOCK_EXPECT(obj1->add).once().with("a", obj1);
	q.add("a", obj1);
}
BOOST_AUTO_TEST_SUITE_END() // queue
BOOST_AUTO_TEST_SUITE(user_data)
BOOST_AUTO_TEST_CASE(cannot_add)
{
	mocks::data_provider prov;
	cppjinja::evt::context_objects::user_data obj(&prov);
	BOOST_CHECK_THROW(obj.add("", nullptr), std::exception);
}
BOOST_AUTO_TEST_CASE(find_is_link)
{
	mocks::data_provider prov;
	cppjinja::evt::context_objects::user_data obj(&prov);
	auto obj_a = obj.find("a"_s);
	BOOST_REQUIRE(obj_a);
	MOCK_EXPECT(prov.value_var_name).with(var_name{"a"_s}).returns("ok"_ad);
	BOOST_TEST(obj_a->solve() == "ok"_s);

	auto obj_b = obj.find("b"_s);
	MOCK_EXPECT(prov.value_function_call).calls([](auto call){
		BOOST_TEST(call.ref.size()==1);
		BOOST_TEST(call.ref.at(0) == "b");
		BOOST_TEST(call.params.size() == 1);
		BOOST_TEST(*call.params.at(0).name == "p1");
		BOOST_TEST(*call.params.at(0).val == "p1_val");
		return "ok"_ad;
	} );
	auto p1_val = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(p1_val->solve).returns("p1_val"_ad);
	cppjinja::evt::context_object::function_parameter p1{"p1", p1_val};
	BOOST_TEST(obj_b->call({p1})->solve() == "ok"_s);
}
BOOST_AUTO_TEST_CASE(name_combination)
{
	mocks::data_provider prov;
	cppjinja::evt::context_objects::user_data obj(&prov);
	auto obj_a = obj.find("a"_s);
	BOOST_REQUIRE(obj_a);
	MOCK_EXPECT(prov.value_var_name).with(var_name{"a"_s}).returns("ok"_ad);
	BOOST_TEST(obj_a->solve() == "ok"_s);

	auto obj_ab = obj_a->find("b");
	MOCK_EXPECT(prov.value_var_name).with(var_name{"a"_s, "b"_s}).returns("ok2"_ad);
	BOOST_TEST(obj_ab->solve() == "ok2"_s);
}
BOOST_AUTO_TEST_SUITE_END() // user_data
BOOST_AUTO_TEST_SUITE(inner_navigation)
BOOST_AUTO_TEST_CASE(cannot_create_without_env)
{
	BOOST_CHECK_THROW(co_nav_imp{nullptr}, std::runtime_error);
	BOOST_CHECK_THROW(co_nav_tmpl{nullptr}, std::runtime_error);
}
BOOST_AUTO_TEST_CASE(cannot_add)
{
	mocks::exenv env;
	BOOST_CHECK_THROW(co_nav_imp(&env).add("a", nullptr), std::logic_error );
	BOOST_CHECK_THROW(co_nav_tmpl(&env).add("a", nullptr), std::logic_error );
}
BOOST_FIXTURE_TEST_CASE(find_in_imports, mock_exenv_fixture)
{
	co_nav_imp in(&env);
	cppjinja::evtree tmpl_info;

	ast::tmpl tast;
	tast.name.name = "tn";
	tast.file_name = "fn";
	tast.file_imports.emplace_back(ast::op_import{0,0,"fn", "as", "tn",{},{}});

	tmpl_info.add_tmpl(tast);
	const evtn::tmpl* tmpl = tmpl_info.search_tmpl("tn");

	auto b_block = std::make_shared<mocks::callable_node>();

	MOCK_EXPECT(env.current_tmpl).returns(tmpl);
	MOCK_EXPECT(env.tmpl).at_least(1).returns(std::ref(tmpl_info));
	expect_roots({b_block.get()});
	expect_solved_params(*b_block, {});
	expect_call(b_block.get());
	MOCK_EXPECT(b_block->name).returns("b");
	MOCK_EXPECT(b_block->evaluate).returns("test"_ad);

	auto btmpl = in.find("as");
	BOOST_REQUIRE(btmpl != nullptr);
	auto ab = btmpl->find("b");
	BOOST_REQUIRE(ab != nullptr);
	BOOST_TEST( in.find("self") != nullptr );

	BOOST_TEST( ab->call({})->solve() == "test"_ad );
}
BOOST_FIXTURE_TEST_CASE(find_in_roots, mock_exenv_fixture)
{
	ast::tmpl tast;
	tast.name.name = "name";
	tast.file_imports.emplace_back(ast::op_import{0,0,"fn", "as","tn",{},{}});

	co_nav_tmpl in(&env);
	cppjinja::evtree tmpl_info;
	tmpl_info.add_tmpl(tast);
	const evtn::tmpl* tmpl = tmpl_info.search_tmpl("name");

	auto a_block = std::make_shared<mocks::callable_node>();
	auto b_block = std::make_shared<mocks::callable_node>();

	expect_call(b_block.get());
	expect_solved_params(*b_block, {});
	expect_roots(tmpl, {a_block.get(), b_block.get()});
	MOCK_EXPECT(a_block->name).returns("a");
	MOCK_EXPECT(b_block->name).returns("b");
	MOCK_EXPECT(env.current_tmpl).returns(tmpl);
	MOCK_EXPECT(b_block->evaluate).once().returns("test"_ad);

	auto b = in.find("b");
	BOOST_TEST(b->call({})->solve() == "test"_ad);
}
BOOST_FIXTURE_TEST_CASE(find_in_single_tmpl, mock_exenv_fixture)
{
	evtn::tmpl tmpl (ast::tmpl{});
	co_nav_stmpl in(&env, &tmpl);

	auto a_block = std::make_shared<mocks::callable_node>();
	auto b_block = std::make_shared<mocks::callable_node>();
	expect_call(b_block.get());
	expect_solved_params(*b_block, {});
	expect_roots(&tmpl, {a_block.get(), b_block.get()});
	MOCK_EXPECT(a_block->name).returns("a");
	MOCK_EXPECT(b_block->name).returns("b");
	MOCK_EXPECT(b_block->evaluate).once().returns("test"_ad);
	MOCK_EXPECT(env.current_tmpl).returns(&tmpl);

	auto b = in.find("b");
	BOOST_TEST(b->call({})->solve() == "test"_ad);
}
BOOST_AUTO_TEST_SUITE_END() // inner_navigation
BOOST_AUTO_TEST_SUITE(lambda)
BOOST_AUTO_TEST_CASE(ret_type_conv)
{
	using cppjinja::evt::context_objects::function_adapter;
	function_adapter adapt([]()->std::pmr::string{ return "ok"; });
	BOOST_TEST( adapt({})->solve() == "ok" );

	function_adapter adapt2([]()->absd::data{ return "data"_sd; });
	BOOST_TEST( adapt2({})->solve() == "data" );
}
BOOST_AUTO_TEST_CASE(args_conv)
{
	using cppjinja::evt::context_objects::function_adapter;
	function_adapter adapt([](std::int64_t a, absd::data b)->std::pmr::string{
		return (std::to_string(a) + std::to_string((std::int64_t)b)).c_str();
	});
	std::pmr::vector<east::function_parameter> params;
	params.emplace_back(east::function_parameter{{}, absd::make_simple(1)});
	params.emplace_back(east::function_parameter{{}, absd::make_simple(2)});
	BOOST_TEST( adapt(params)->solve() == "12" );
}
BOOST_AUTO_TEST_SUITE_END() // lambda
BOOST_AUTO_TEST_SUITE_END() // context_object

BOOST_AUTO_TEST_SUITE(context)
BOOST_FIXTURE_TEST_CASE(current_tmpl, impl_exenv_fixture)
{
	using namespace cppjinja::ast;

	auto data = "<%template a%><%block a%>a<%endblock%><%endtemplate%><%template b%>b<%endtemplate%>"sv;
	auto parsed = cppjinja::text::parse(cppjinja::text::file, data);
	for(auto& t:parsed.tmpls) compiled.add_tmpl(t);
	BOOST_TEST( compiled.list().size() == 2 );
	const cppjinja::evtnodes::tmpl* tmpl_a = compiled.search_tmpl("a");
	const cppjinja::evtnodes::tmpl* tmpl_b = compiled.search_tmpl("b");
	BOOST_REQUIRE( tmpl_a );
	BOOST_REQUIRE( tmpl_b );
	BOOST_TEST_REQUIRE(tmpl_a->ast_name() == "a");
	BOOST_TEST_REQUIRE(tmpl_b->ast_name() == "b");

	BOOST_CHECK_THROW(ctx.current_tmpl(), std::runtime_error);
	ctx.push_shadow(tmpl_a);
	BOOST_TEST(ctx.current_tmpl()->ast_name() == "a");
	ctx.push_shadow(tmpl_b);
	BOOST_TEST(ctx.current_tmpl()->ast_name() == "b");
	ctx.pop(tmpl_b);
	BOOST_TEST(ctx.current_tmpl()->ast_name() == "a");
}
BOOST_FIXTURE_TEST_CASE(current_node, mock_impls_fixture)
{
	mocks::node fnode1, fnode2;

	BOOST_CHECK_THROW(ctx.nth_node_on_stack(0), std::exception);
	BOOST_CHECK_THROW(ctx.current_node(&fnode1), std::exception);

	ctx.push_shadow(&fnode1);
	ctx.current_node(&fnode1);
	BOOST_TEST(ctx.nth_node_on_stack(0) == &fnode1);

	ctx.current_node(&fnode2);
	BOOST_TEST(ctx.nth_node_on_stack(0) == &fnode2);
	BOOST_TEST(ctx.nth_node_on_stack(1) == &fnode1);

	BOOST_CHECK_THROW(ctx.nth_node_on_stack(2), std::exception);
}
BOOST_FIXTURE_TEST_CASE(stack, mock_impls_fixture)
{
	mocks::node fnode1, fnode2, fnode3;
	BOOST_CHECK_THROW(ctx.maker(), std::exception);
	BOOST_CHECK_THROW(ctx.pop(&fnode1), std::exception);
	BOOST_CHECK_THROW(ctx.push(&fnode1), std::exception);

	ctx.push_shadow(&fnode1);
	BOOST_TEST(ctx.maker() == &fnode1);
	BOOST_CHECK_THROW(ctx.pop(&fnode2), std::exception);

	ctx.push(&fnode2);
	BOOST_TEST(ctx.maker() == &fnode1);

	ctx.push_shadow(&fnode3);
	BOOST_TEST(ctx.maker() == &fnode3);

	BOOST_CHECK_NO_THROW(ctx.pop(&fnode3));
	BOOST_TEST(ctx.maker() == &fnode1);

	BOOST_CHECK_NO_THROW(ctx.pop(&fnode2));
	BOOST_CHECK_NO_THROW(ctx.pop(&fnode1));
}
BOOST_FIXTURE_TEST_CASE(cur_namespace, mock_impls_fixture)
{
	using cppjinja::evt::context_objects::value;
	BOOST_CHECK_THROW(ctx.cur_namespace(), std::exception);

	mocks::node maker, ctx1, ctx2;
	auto obj_maker = std::make_shared<value>("maker"_ad);
	auto obj_ctx1 = std::make_shared<value>("ctx1"_ad);
	auto obj2_ctx1 = std::make_shared<value>("ctx1_2"_ad);
	auto obj_ctx2 = std::make_shared<value>("ctx2"_ad);

	ctx.push_shadow(&maker);
	BOOST_CHECK_NO_THROW(ctx.cur_namespace());
	ctx.cur_namespace().add("a", obj_maker);
	ctx.push(&ctx1);
	ctx.cur_namespace().add("b", obj_ctx1);
	ctx.cur_namespace().add("c", obj2_ctx1);
	BOOST_TEST(ctx.cur_namespace().find("c"_s) == obj2_ctx1);
	ctx.push(&ctx2);
	ctx.cur_namespace().add("c", obj_ctx2);
	BOOST_TEST(ctx.cur_namespace().find("a"_s) == obj_maker);
	BOOST_TEST(ctx.cur_namespace().find("b"_s) == obj_ctx1);
	BOOST_TEST(ctx.cur_namespace().find("c"_s) == obj_ctx2);

	ctx.pop(&ctx2);
	BOOST_TEST(ctx.cur_namespace().find("a"_s) == obj_maker);
	BOOST_TEST(ctx.cur_namespace().find("b"_s) == obj_ctx1);
	BOOST_TEST(ctx.cur_namespace().find("c"_s) == obj2_ctx1);

	ctx.pop(&ctx1);
	BOOST_TEST(ctx.cur_namespace().find("a"_s) == obj_maker);
	BOOST_TEST(ctx.cur_namespace().find("b"_s) == nullptr);
	BOOST_TEST(ctx.cur_namespace().find("c"_s) == nullptr);
}
BOOST_FIXTURE_TEST_CASE(out, mock_impls_fixture)
{
	mocks::node node1, node2;
	BOOST_CHECK_THROW(ctx.out(), std::exception);
	BOOST_CHECK_THROW(ctx.result(), std::exception);
	ctx.push_shadow(&node1);
	BOOST_REQUIRE_NO_THROW(ctx.out());
	std::ostream* out1 = &ctx.out();
	BOOST_TEST(ctx.result() == "");
	ctx.out() << "test";
	BOOST_TEST(ctx.result() == "test");
	ctx.push(&node2);
	BOOST_TEST(ctx.result() == "test");
	BOOST_TEST(&ctx.out() == out1);
	ctx.pop(&node2);
	BOOST_TEST(&ctx.out() == out1);
	BOOST_TEST(ctx.result() == "test");
	ctx.out() << "test";
	BOOST_TEST(ctx.result() == "testtest");
}
BOOST_AUTO_TEST_SUITE_END() // context

BOOST_AUTO_TEST_SUITE(callstack)
BOOST_FIXTURE_TEST_CASE(cannot_call_nullptr, mock_impls_fixture)
{
	using namespace cppjinja::evt::context_objects;
	BOOST_CHECK_THROW(calls.push(nullptr, callable_params({},{})), std::exception);
}
BOOST_FIXTURE_TEST_CASE(push_pop, mock_impls_fixture)
{
	using namespace cppjinja::east;
	mocks::callable_node calling1, calling2;

	BOOST_CHECK_THROW(calls.current_params(&calling1), std::exception);
	calls.push(&calling1, cppjinja::evt::context_objects::callable_params({}, {}));
	BOOST_TEST(calls.current_params(&calling1).find("a") == nullptr);
	BOOST_CHECK_THROW(calls.current_params(&calling2), std::exception);
	calls.pop();
	BOOST_CHECK_THROW(calls.current_params(&calling1), std::exception);
}
BOOST_AUTO_TEST_SUITE_END() // callstack

BOOST_AUTO_TEST_SUITE(exenv)
BOOST_AUTO_TEST_CASE(rinfo_ops)
{
	std::stringstream out;
	cppjinja::evt::render_info ri{false, true};
	cppjinja::evt::render_info ri2{false, true};
	out << ri;
	BOOST_TEST(out.str() == "{0,1}");
	BOOST_TEST(ri == ri2);
}
BOOST_FIXTURE_TEST_CASE(environment, impl_exenv_fixture)
{
	BOOST_TEST(env.data() == &data);
	BOOST_TEST(&env.tmpl() == &compiled);
	BOOST_CHECK_NO_THROW(env.ctx());
	BOOST_CHECK_NO_THROW(env.calls());
}
BOOST_FIXTURE_TEST_CASE(render_shift, impl_exenv_fixture)
{
	mocks::node maker;
	ctx.push_shadow(&maker);
	env.out() << "ok\nok"s;
	BOOST_TEST(env.result() == "ok\nok"s);
	env.render_format().shift_tab(1);
	BOOST_TEST(env.result() == "ok\n\tok"s);
	env.render_format().shift_tab(1);
	BOOST_TEST(env.result() == "ok\n\t\tok"s);
	env.render_format().shift_tab(-2);
	BOOST_TEST(env.result() == "ok\nok"s);
}
BOOST_FIXTURE_TEST_CASE(rinfo, impl_exenv_fixture)
{
	using cppjinja::evt::render_info;
	render_info ri{true, false};
	BOOST_CHECK(!env.rinfo().has_value());
	BOOST_CHECK_NO_THROW(env.rinfo(ri));
	std::optional<render_info> val_after_set = env.rinfo();
	BOOST_REQUIRE(val_after_set.has_value());
	bool ltrim = val_after_set->trim_left;
	bool rtrim = val_after_set->trim_right;
	BOOST_TEST(ltrim == true);
	BOOST_TEST(rtrim == false);
}
BOOST_FIXTURE_TEST_CASE(current_node, impl_exenv_fixture)
{
	mocks::node fnode1, fnode2;
	ctx.push_shadow(&fnode2);
	BOOST_CHECK_NO_THROW( env.current_node(&fnode1) );
	BOOST_TEST(ctx.nth_node_on_stack(0) == &fnode1);
}
BOOST_FIXTURE_TEST_CASE(current_tmpl, impl_exenv_fixture)
{
	ast::tmpl asttmpl;
	asttmpl.name.name = "test";
	cppjinja::evtnodes::tmpl tmpl(asttmpl);
	mocks::node fnode1, fnode2;
	ctx.push_shadow(&tmpl);
	BOOST_TEST(env.current_tmpl() == &tmpl);
}
BOOST_FIXTURE_TEST_CASE(children, impl_exenv_fixture)
{
	using namespace cppjinja::ast;
	auto data = "<%block a%>a<%endblock%>"sv;
	auto parsed = cppjinja::text::parse(cppjinja::text::tmpl, data);
	compiled.add_tmpl(parsed);
	const cppjinja::evtnodes::tmpl* tmpl = compiled.search_tmpl("");
	BOOST_TEST( env.children(nullptr).empty() );
	BOOST_TEST( env.children(tmpl).size() == 1 );
	BOOST_TEST( env.roots(tmpl) == compiled.roots(tmpl) );
}
BOOST_FIXTURE_TEST_CASE(result, impl_exenv_fixture)
{
	mocks::node ctxmaker;
	ctx.push_shadow(&ctxmaker);
	BOOST_TEST(&env.out() == &ctx.out());
	BOOST_TEST(env.result() == "");
	ctx.out() << "test";
	BOOST_TEST(env.result() == "test");
}
BOOST_FIXTURE_TEST_CASE(bintins, impl_exenv_fixture)
{
	evtn::tmpl tmpl(ast::tmpl{});
	ctx.push_shadow(&tmpl);

	mocks::callable_node node;
	ctx.push_shadow(&node);
	calls.push(&node, cppjinja::evt::context_objects::callable_params({},{}));

	BOOST_CHECK(dynamic_cast<cppjinja::evt::context_objects::jinja_namespace*>(
	                env.all_ctx().find("namespace").get()));
}
BOOST_FIXTURE_TEST_CASE(globals, impl_exenv_fixture)
{
	BOOST_CHECK_NO_THROW(env.globals());
	BOOST_TEST(&env.globals() == &env.globals());
}
BOOST_FIXTURE_TEST_CASE(locals, impl_exenv_fixture)
{
	BOOST_CHECK_THROW(env.locals(), std::exception);
	mocks::node maker;
	ctx.push_shadow(&maker);
	BOOST_CHECK_NO_THROW(env.locals());
}
BOOST_FIXTURE_TEST_CASE(params, impl_exenv_fixture)
{
	mocks::callable_node calling1;
	auto p1_val = std::make_shared<mocks::context_object>();
	cppjinja::evt::context_objects::callable_params params(
	{}, {cppjinja::evt::context_object::function_parameter{"p1"_s, p1_val}});
	ctx.push_shadow(&calling1);
	calls.push(&calling1, params);
	BOOST_TEST(env.params().find("p1"_s) == p1_val);
}
BOOST_FIXTURE_TEST_CASE(user_data, impl_exenv_fixture)
{
	MOCK_EXPECT(data.value_var_name).with(evar_name{"a"_s}).returns(42_ad);
	BOOST_TEST(env.user_data().find("a")->solve() == 42);
}
BOOST_FIXTURE_TEST_CASE(all_ctx, impl_exenv_fixture)
{
	evtn::tmpl tmpl(ast::tmpl{});
	ctx.push_shadow(&tmpl);

	mocks::callable_node maker;
	ctx.push_shadow(&maker);
	calls.push(&maker, cppjinja::evt::context_objects::callable_params({},{}));
	MOCK_EXPECT(data.value_var_name).with(evar_name{"a"_s}).returns(42_ad);
	BOOST_TEST(env.all_ctx().find("a")->solve() == 42);
}
BOOST_AUTO_TEST_SUITE_END() // exenv

BOOST_AUTO_TEST_SUITE(raii)
BOOST_FIXTURE_TEST_CASE(push_ctx, mock_exenv_fixture)
{
	mocks::node maker;
	{
		mock::sequence seq;
		MOCK_EXPECT(ctx.push).once().in(seq).with(&maker);
		MOCK_EXPECT(ctx.pop).once().in(seq).with(&maker);
		cppjinja::evt::raii_push_ctx pusher(&maker, &ctx);
	}
	{
		mock::sequence seq;
		MOCK_EXPECT(ctx.push).once().in(seq).with(&maker);
		MOCK_EXPECT(ctx.pop).once().in(seq).with(&maker);
		cppjinja::evt::raii_push_ctx pusher(&maker, &ctx);
		cppjinja::evt::raii_push_ctx pusher2(std::move(pusher));
	}
}
BOOST_FIXTURE_TEST_CASE(push_shadow_ctx, mock_exenv_fixture)
{
	mocks::node maker;
	{
		mock::sequence seq;
		MOCK_EXPECT(ctx.push_shadow).once().in(seq).with(&maker);
		MOCK_EXPECT(ctx.pop).once().in(seq).with(&maker);
		cppjinja::evt::raii_push_shadow_ctx pusher(&maker, &ctx);
	}
	{
		mock::sequence seq;
		MOCK_EXPECT(ctx.push_shadow).once().in(seq).with(&maker);
		MOCK_EXPECT(ctx.pop).once().in(seq).with(&maker);
		cppjinja::evt::raii_push_shadow_ctx pusher(&maker, &ctx);
		cppjinja::evt::raii_push_shadow_ctx pusher2(std::move(pusher));
	}
}
BOOST_AUTO_TEST_CASE(result_formatter)
{
	using namespace cppjinja::evt;
	cppjinja::evt::result_formatter fmt;
	BOOST_TEST(fmt("\na"_s) == "\na"_s);
	{
		raii_result_format raii(&fmt, 2, -2);
		BOOST_TEST(fmt("\na"_s) == "\n\t\ta"_s);
	}
	BOOST_TEST(fmt("\na"_s) == "\na"_s);
}
BOOST_AUTO_TEST_SUITE_END() // raii

BOOST_AUTO_TEST_SUITE(result_formatter)
using cppjinja::evt::result_formatter;
BOOST_AUTO_TEST_CASE(single_shift)
{
	result_formatter fmt;
	BOOST_TEST(fmt("ok\nok"_s) == "ok\nok"_s);
	fmt.shift_tab(1);
	BOOST_TEST(fmt("ok"_s) == "ok"_s);
	BOOST_TEST(fmt("ok\nok"_s) == "ok\n\tok"_s);
	fmt.shift_tab(0);
	BOOST_TEST(fmt("ok\nok"_s) == "ok\n\tok"_s);
	fmt.shift_tab(-1);
	BOOST_TEST(fmt("ok\nok"_s) == "ok\nok"_s);
}
BOOST_AUTO_TEST_CASE(few_tabls)
{
	result_formatter fmt;
	BOOST_TEST(fmt("ok\nok"_s) == "ok\nok"_s);
	fmt.shift_tab(3);
	BOOST_TEST(fmt("ok"_s) == "ok"_s);
	BOOST_TEST(fmt("ok\nok"_s) == "ok\n\t\t\tok"_s);
	BOOST_CHECK_NO_THROW(fmt.shift_tab(-2));
	BOOST_TEST(fmt("ok\nok"_s) == "ok\n\tok"_s);
	BOOST_CHECK_NO_THROW(fmt.shift_tab(-1));
	BOOST_CHECK_THROW(fmt.shift_tab(-1), std::exception);
}
BOOST_AUTO_TEST_CASE(cannot_shift_under_zero)
{
	result_formatter fmt;
	BOOST_CHECK_THROW(fmt.shift_tab(-1), std::exception);
	BOOST_CHECK_NO_THROW(fmt.shift_tab(1));
	BOOST_CHECK_THROW(fmt.shift_tab(-2), std::exception);
	BOOST_CHECK_NO_THROW(fmt.shift_tab(0));
}
BOOST_AUTO_TEST_CASE(cannot_shift_too_much)
{
	result_formatter fmt;
	BOOST_CHECK_THROW(fmt.shift_tab(101), std::exception);
	BOOST_TEST(fmt("\na") == "\na");
}
BOOST_AUTO_TEST_SUITE_END() // result_formatter

BOOST_AUTO_TEST_SUITE_END() // phase_evaluate
