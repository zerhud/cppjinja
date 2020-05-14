/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
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
#include "evtree/exenv/expr_solver.hpp"
#include "evtree/exenv/expr_filter.hpp"
#include "evtree/exenv/context_objects/tree.hpp"
#include "evtree/exenv/context_objects/value.hpp"
#include "evtree/exenv/context_objects/callable_node.hpp"
#include "evtree/exenv/context_objects/user_data.hpp"
#include "evtree/exenv/context_objects/delay_call.hpp"
#include "evtree/exenv/context_objects/builtins.hpp"
#include "evtree/nodes/callable.hpp"
#include "parser/operators/common.hpp"
#include "parser/grammar/tmpls.hpp"
#include "parser/parse.hpp"

using namespace std::literals;
namespace tdata = boost::unit_test::data;

using cppjinja::ast::var_name;
using cppjinja::ast::comparator;
using cppjinja::ast::value_term;
using cppjinja::evt::expr_filter;
using east_value_term = cppjinja::east::value_term;
using evalue_term = cppjinja::east::value_term;
using evar_name = cppjinja::east::var_name;
using mocks::mock_exenv_fixture; // qtcreator cannot parse test with namespace

template<typename A>
A make_container(const std::vector<value_term> vals)
{
	std::vector<cppjinja::ast::forward_ast<value_term>> favals;
	for(auto& v:vals) favals.emplace_back(v);
	return A{{1,1}, favals};
}

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

struct mock_solver_fixture : mocks::mock_exenv_fixture
{
	cppjinja::evt::expr_solver solver;
	mock_solver_fixture() : solver(&env) {}
};

BOOST_AUTO_TEST_SUITE(phase_evaluate)

BOOST_AUTO_TEST_SUITE(context_object)
using cppjinja::east::var_name;
using cppjinja::evt::context_objects::tree;
BOOST_AUTO_TEST_SUITE(obj_tree)
BOOST_AUTO_TEST_CASE(find)
{
	tree obj;
	auto child1 = std::make_shared<tree>();
	auto child2 = std::make_shared<tree>();
	obj.add("child1"s, child1);
	BOOST_TEST(obj.find(var_name{"no"}) == nullptr);
	BOOST_TEST(obj.find(var_name{"child1"}) == child1);
	obj.add("child2"s, child2);
	BOOST_TEST(obj.find(var_name{"no"}) == nullptr);
	BOOST_TEST(obj.find(var_name{"child2"}) == child2);
}
BOOST_AUTO_TEST_CASE(find_multilevel)
{
	tree obj;
	auto child1 = std::make_shared<tree>();
	auto child2 = std::make_shared<tree>();
	obj.add("child1"s, child1);
	child1->add("child2", child2);
	BOOST_TEST(obj.find(var_name{"child1", "child2"}) == child2);
}
BOOST_AUTO_TEST_CASE(solve)
{
	tree obj;
	BOOST_TEST(obj.solve() == east_value_term{"{}"s});
	obj.add("c1", std::make_shared<tree>());
	BOOST_TEST(obj.solve() == east_value_term{"{\"c1\":{}}"s});
	obj.add("c2", std::make_shared<tree>());
	BOOST_TEST(obj.solve() == east_value_term{"{\"c1\":{},\"c2\":{}}"s});
	obj.find(var_name{"c2"})->add("c3", std::make_shared<tree>());
	obj.find(var_name{"c2"})->add("c4", std::make_shared<cppjinja::evt::context_objects::value>("ok"s));
	BOOST_TEST(obj.solve() == east_value_term{"{\"c1\":{},\"c2\":{\"c3\":{},\"c4\":\"ok\"}}"s});
}
BOOST_AUTO_TEST_CASE(cannot_call)
{
	tree obj;
	BOOST_CHECK_THROW(obj.call({}), std::exception);
}
BOOST_AUTO_TEST_SUITE_END() // tree
BOOST_AUTO_TEST_SUITE(obj_value)
BOOST_AUTO_TEST_CASE(no_tree_methods_and_no_call)
{
	cppjinja::evt::context_objects::value val("ok");
	BOOST_CHECK_THROW(val.add("a",std::make_shared<tree>()), std::exception);
	BOOST_CHECK_THROW(val.find(var_name{"a"}), std::exception);
	BOOST_CHECK_THROW(val.call({}), std::exception);
}
BOOST_AUTO_TEST_CASE(solve)
{
	using cppjinja::evt::context_objects::value;
	BOOST_TEST(value("ok"s).solve() == east_value_term{"ok"s});
	BOOST_TEST(value(east_value_term{4}).solve() == east_value_term{4});
}
BOOST_AUTO_TEST_SUITE_END() // value
BOOST_AUTO_TEST_SUITE(callable_node)
BOOST_AUTO_TEST_CASE(no_tree_methods_and_no_solve)
{
	mocks::exenv env;
	mocks::callable_node node;
	cppjinja::evt::context_objects::callable_node val(&env, &node);
	BOOST_CHECK_THROW(val.add("a",std::make_shared<tree>()), std::exception);
	BOOST_CHECK_THROW(val.find(var_name{"a"}), std::exception);
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
	cppjinja::east::function_parameter p1{"a"s, "ok"s};
	MOCK_EXPECT(calls.push).once().in(seq).calls([&node,&p1](auto*n, auto params){
		BOOST_TEST(n == &node);
		auto sp1 = params.find(var_name{"a"});
		BOOST_TEST(sp1->jval() == *p1.jval);
	});
	MOCK_EXPECT(node.evaluate)
	        .once()
	        .in(seq)
	        .with([this](auto& e){return &e==&env;})
	        .returns("ok"s);
	MOCK_EXPECT(calls.pop).once().in(seq);
	MOCK_EXPECT(node.solved_params).at_least(1).returns(std::vector<cppjinja::east::function_parameter>{});
	BOOST_TEST(val.call({p1})->solve() == east_value_term{"ok"s});
}
BOOST_AUTO_TEST_SUITE_END() // callable_node
BOOST_AUTO_TEST_SUITE(callable_params)
BOOST_AUTO_TEST_CASE(cannot_add_solve_call)
{
	cppjinja::evt::context_objects::callable_params obj({}, {});
	BOOST_CHECK_THROW(obj.add("a", nullptr), std::exception);
	BOOST_CHECK_THROW(obj.call({}), std::exception);
	BOOST_CHECK_THROW(obj.solve(), std::exception);
}
BOOST_AUTO_TEST_CASE(params)
{
	using cppjinja::east::function_parameter;
	function_parameter c1{std::nullopt, 41};
	function_parameter c2{"p2"s, 42};
	function_parameter c3{"p3"s, 43};
	function_parameter c4{std::nullopt, 44};

	function_parameter p1{"p1", std::nullopt};
	function_parameter p2{"p2", 52};
	function_parameter p4{"p4", 54};
	function_parameter p5{"p5", 55};

	cppjinja::evt::context_objects::callable_params obj({p1, p2, p4, p5}, {c1, c2, c4, c3});
	BOOST_TEST(obj.find(var_name{"p1"})->jval() == 41);
	BOOST_TEST(obj.find(var_name{"p2"})->jval() == 42);
	BOOST_TEST(obj.find(var_name{"p3"})->jval() == 43);
	BOOST_TEST(obj.find(var_name{"p4"})->jval() == 44);
	BOOST_TEST(obj.find(var_name{"p5"})->jval() == 55);
	BOOST_TEST(obj.find(var_name{"P1"}) == nullptr);
}
BOOST_AUTO_TEST_SUITE_END() // callable_params
BOOST_AUTO_TEST_SUITE(queue)
BOOST_AUTO_TEST_CASE(from_queue)
{
	mocks::context_object obj1, obj2, obj3;
	cppjinja::evt::context_objects::queue q1({&obj1, &obj2});
	cppjinja::evt::context_objects::queue q2({&obj3});
	cppjinja::evt::context_objects::queue q({q1, q2});

	MOCK_EXPECT(obj1.find).once().with(var_name{"a"}).returns(nullptr);
	MOCK_EXPECT(obj2.find).once().with(var_name{"a"}).returns(nullptr);
	MOCK_EXPECT(obj3.find).once().with(var_name{"a"}).returns(nullptr);
	BOOST_TEST(q.find(var_name{"a"}) == nullptr);

	auto obj4 = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(obj1.find).once().with(var_name{"a"}).returns(nullptr);
	MOCK_EXPECT(obj2.find).once().with(var_name{"a"}).returns(nullptr);
	MOCK_EXPECT(obj3.find).once().with(var_name{"a"}).returns(obj4);
	BOOST_TEST(q.find(var_name{"a"}) == obj4);
}
BOOST_AUTO_TEST_CASE(find)
{
	mocks::context_object obj1, obj2;
	auto obj3 = std::make_shared<mocks::context_object>();
	cppjinja::evt::context_objects::queue q({&obj1, &obj2});

	BOOST_CHECK_THROW(q.solve(), std::exception);
	BOOST_CHECK_THROW(q.call({}), std::exception);

	MOCK_EXPECT(obj1.find).once().with(var_name{"a"}).returns(nullptr);
	MOCK_EXPECT(obj2.find).once().with(var_name{"a"}).returns(nullptr);
	BOOST_TEST(q.find(var_name{"a"}) == nullptr);

	auto obj4 = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(obj1.find).once().with(var_name{"a"}).returns(nullptr);
	MOCK_EXPECT(obj2.find).once().with(var_name{"a"}).returns(obj4);
	BOOST_TEST(q.find(var_name{"a"}) == obj4);

	using cppjinja::evt::context_object;
	MOCK_EXPECT(obj1.find).once().with(var_name{"a"}).returns(nullptr);
	MOCK_EXPECT(obj2.find).once().with(var_name{"a"}).returns(obj4);
	cppjinja::evt::context_objects::queue other1 =
	{(const context_object*)&obj1,(const context_object*)&obj2};
	cppjinja::evt::context_objects::queue other = std::move(other1);
	BOOST_TEST(other.find(var_name{"a"}) == obj4);
}
BOOST_AUTO_TEST_CASE(add)
{
	cppjinja::evt::context_objects::queue q;
	auto obj1 = std::make_shared<mocks::context_object>();
	BOOST_CHECK_THROW(q.add("", nullptr), std::exception);
	BOOST_CHECK_THROW(q.add("a", obj1), std::exception);

	auto obj2 = std::make_shared<mocks::context_object>();
	q = {(const cppjinja::evt::context_object*)obj1.get()};
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
	auto obj_a = obj.find(var_name{"a"s});
	BOOST_REQUIRE(obj_a);
	MOCK_EXPECT(prov.value_var_name).with(var_name{"a"s}).returns(east_value_term{"ok"s});
	BOOST_TEST(obj_a->jval() == "ok"s);

	auto obj_b = obj.find(var_name{"b"s});
	MOCK_EXPECT(prov.value_function_call).calls([](auto call){
		BOOST_TEST(call.ref.size()==1);
		BOOST_TEST(call.ref.at(0) == "b");
		BOOST_TEST(call.params.size() == 1);
		BOOST_TEST(*call.params.at(0).name == "p1"s);
		BOOST_TEST(*call.params.at(0).jval == "p1_val"s);
		return east_value_term{"ok"s};
	} );
	cppjinja::east::function_parameter p1{"p1"s, "p1_val"s};
	BOOST_TEST(obj_b->call({p1})->jval() == "ok"s);
}
BOOST_AUTO_TEST_CASE(name_combination)
{
	mocks::data_provider prov;
	cppjinja::evt::context_objects::user_data obj(&prov);
	auto obj_a = obj.find(var_name{"a"s});
	BOOST_REQUIRE(obj_a);
	MOCK_EXPECT(prov.value_var_name).with(var_name{"a"s}).returns(east_value_term{"ok"s});
	BOOST_TEST(obj_a->jval() == "ok"s);

	auto obj_ab = obj_a->find(var_name{"b"});
	MOCK_EXPECT(prov.value_var_name).with(var_name{"a"s, "b"s}).returns(east_value_term{"ok2"s});
	BOOST_TEST(obj_ab->jval() == "ok2"s);
}
BOOST_AUTO_TEST_SUITE_END() // user_data
BOOST_AUTO_TEST_SUITE(delay_call)
BOOST_AUTO_TEST_CASE(solve)
{
	auto obj = std::make_shared<mocks::context_object>();
	std::vector<cppjinja::east::function_parameter> params;
	params.emplace_back().jval = "pval"s;
	cppjinja::evt::context_objects::delay_call dc(obj, params);
	auto obj_val = std::make_shared<cppjinja::evt::context_objects::value>(evalue_term{"ok"s});
	MOCK_EXPECT(obj->call).once().with(params).returns(obj_val);
	BOOST_TEST(dc.solve() == evalue_term{"ok"s});
}
BOOST_AUTO_TEST_CASE(cannot_find_add)
{
	auto obj = std::make_shared<mocks::context_object>();
	std::vector<cppjinja::east::function_parameter> params;
	cppjinja::evt::context_objects::delay_call dc(obj, params);
	BOOST_CHECK_THROW(dc.find(evar_name{"a"}), std::exception);
	BOOST_CHECK_THROW(dc.add("a", obj), std::exception);
}
BOOST_AUTO_TEST_CASE(call)
{
	auto obj = std::make_shared<mocks::context_object>();
	std::vector<cppjinja::east::function_parameter> params;
	params.emplace_back().jval = "pval"s;
	params.emplace_back().name = "p3";
	params.back().jval = "bad"s;
	cppjinja::evt::context_objects::delay_call dc(obj, params);

	params.back().jval = "good"s;
	params.emplace_back().name = "p2";

	std::vector<cppjinja::east::function_parameter> call_params;
	call_params.emplace_back().name = "p2";
	call_params.emplace_back().name = "p3";
	call_params.back().jval = "good"s;

	auto obj_val = std::make_shared<cppjinja::evt::context_objects::value>(evalue_term{"ok"s});
	MOCK_EXPECT(obj->call).once().with(params).returns(obj_val);
	BOOST_TEST(dc.call(call_params)->solve() == evalue_term{"ok"s});
}
BOOST_AUTO_TEST_SUITE_END() // delay_call
BOOST_AUTO_TEST_SUITE(builtins)
using cppjinja::east::function_parameter;
BOOST_AUTO_TEST_CASE(context)
{
	using namespace cppjinja::evt::context_objects;
	cppjinja::evt::context_objects::builtins stdlib;

	auto ns = stdlib.find(var_name{"namespace"s});
	BOOST_CHECK(dynamic_cast<jinja_namespace*>(ns.get()));
}
BOOST_AUTO_TEST_CASE(jinja_namespace)
{
	cppjinja::evt::context_objects::jinja_namespace ns;
	BOOST_CHECK_THROW(ns.call({}), std::exception);
	BOOST_CHECK_THROW(ns.solve(), std::exception);
	BOOST_CHECK_THROW(ns.add("a", nullptr), std::exception);
	BOOST_CHECK_THROW(ns.find(evar_name{"a"}), std::exception);

	auto tree = ns.call({function_parameter{"a", "ok_a"}});
	BOOST_CHECK(tree);
	BOOST_TEST(tree->find(evar_name{"a"})->jval() =="ok_a"s);
}
BOOST_AUTO_TEST_SUITE_END() // delay_call
BOOST_AUTO_TEST_SUITE_END() // context_object

BOOST_AUTO_TEST_SUITE(context)
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
	auto obj_maker = std::make_shared<value>(evalue_term{"maker"});
	auto obj_ctx1 = std::make_shared<value>(evalue_term{"ctx1"});
	auto obj2_ctx1 = std::make_shared<value>(evalue_term{"ctx1_2"});
	auto obj_ctx2 = std::make_shared<value>(evalue_term{"ctx2"});

	ctx.push_shadow(&maker);
	BOOST_CHECK_NO_THROW(ctx.cur_namespace());
	ctx.cur_namespace().add("a", obj_maker);
	ctx.push(&ctx1);
	ctx.cur_namespace().add("b", obj_ctx1);
	ctx.cur_namespace().add("c", obj2_ctx1);
	BOOST_TEST(ctx.cur_namespace().find(evar_name{"c"s}) == obj2_ctx1);
	ctx.push(&ctx2);
	ctx.cur_namespace().add("c", obj_ctx2);
	BOOST_TEST(ctx.cur_namespace().find(evar_name{"a"s}) == obj_maker);
	BOOST_TEST(ctx.cur_namespace().find(evar_name{"b"s}) == obj_ctx1);
	BOOST_TEST(ctx.cur_namespace().find(evar_name{"c"s}) == obj_ctx2);

	ctx.pop(&ctx2);
	BOOST_TEST(ctx.cur_namespace().find(evar_name{"a"s}) == obj_maker);
	BOOST_TEST(ctx.cur_namespace().find(evar_name{"b"s}) == obj_ctx1);
	BOOST_TEST(ctx.cur_namespace().find(evar_name{"c"s}) == obj2_ctx1);

	ctx.pop(&ctx1);
	BOOST_TEST(ctx.cur_namespace().find(evar_name{"a"s}) == obj_maker);
	BOOST_TEST(ctx.cur_namespace().find(evar_name{"b"s}) == nullptr);
	BOOST_TEST(ctx.cur_namespace().find(evar_name{"c"s}) == nullptr);
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
	BOOST_TEST(calls.current_params(&calling1).find(var_name{"a"}) == nullptr);
	BOOST_CHECK_THROW(calls.current_params(&calling2), std::exception);
	calls.pop();
	BOOST_CHECK_THROW(calls.current_params(&calling1), std::exception);
}
BOOST_AUTO_TEST_SUITE_END() // callstack

BOOST_AUTO_TEST_SUITE(filter)
BOOST_FIXTURE_TEST_CASE(by_var, mock_exenv_fixture)
{
	using cppjinja::ast::filter_call;
	cppjinja::ast::var_name vn{"a"};
	auto check = [&vn](
	          const cppjinja::east::function_call& fc
	        , const east_value_term& base){
		BOOST_TEST_REQUIRE(fc.ref.size()==1);
		BOOST_TEST(fc.ref[0]==vn[0]);
		BOOST_TEST(base == east_value_term{2});
		BOOST_CHECK(fc.params.empty());
		return east_value_term{101};
	};
	MOCK_EXPECT(data.filter).exactly(2).calls(check);
	BOOST_TEST(expr_filter(&env, east_value_term{2})(vn) == east_value_term{101});
	BOOST_TEST(expr_filter(&env, east_value_term{2})(filter_call{vn}) == east_value_term{101});
}
BOOST_FIXTURE_TEST_CASE(by_fnc, mock_exenv_fixture)
{
	cppjinja::ast::function_call fc;
	fc.ref = var_name{"a"s};
	fc.params.emplace_back(value_term{42});
	auto check = [&fc](
	          const cppjinja::east::function_call& udfc
	        , const east_value_term& base){
		BOOST_TEST(udfc.ref==fc.ref);
		BOOST_TEST(base == east_value_term{2});
		BOOST_TEST(udfc.params.size() == fc.params.size());
		BOOST_TEST(udfc.params[0].name.has_value() == fc.params[0].name.has_value());
		BOOST_CHECK(udfc.params[0].jval.has_value());
		BOOST_TEST(*udfc.params[0].jval == 42.0);
		return east_value_term{101};
	};
	MOCK_EXPECT(data.filter).once().calls(check);
	BOOST_TEST(expr_filter(&env, east_value_term{2})(fc) == east_value_term{101});
	MOCK_EXPECT(data.filter).once().calls(check);
	BOOST_TEST(expr_filter(&env, east_value_term{2})(value_term{fc}) == east_value_term{101});
}
BOOST_FIXTURE_TEST_CASE(by_wrong, mock_exenv_fixture)
{
	using namespace cppjinja::ast;
	BOOST_CHECK_THROW(expr_filter(nullptr, east_value_term{42}), std::exception);
	BOOST_CHECK_THROW(expr_filter(&env, east_value_term{42})(2), std::exception);
	BOOST_CHECK_THROW(expr_filter(&env, east_value_term{42})("str"s), std::exception);
	BOOST_CHECK_THROW(expr_filter(&env, east_value_term{42})(tuple_v{}), std::exception);
	BOOST_CHECK_THROW(expr_filter(&env, east_value_term{42})(array_v{}), std::exception);
	BOOST_CHECK_THROW(expr_filter(&env, east_value_term{42})(binary_op{}), std::exception);
}
BOOST_FIXTURE_TEST_CASE(few_times, mock_exenv_fixture)
{
	using cppjinja::ast::var_name;
	using cppjinja::ast::function_call;
	int calls_num = 40;
	auto check = [&calls_num](
	          const cppjinja::east::function_call&
	        , const east_value_term& base){
		BOOST_TEST(base == east_value_term{calls_num});
		return east_value_term{calls_num + 1};
	};

	MOCK_EXPECT(data.filter).calls(check);
	expr_filter flt(&env, east_value_term{40});
	for(;calls_num < 45;++calls_num)
		BOOST_TEST(flt(var_name{"a"}) == east_value_term{calls_num+1});
	for(;calls_num < 50;++calls_num)
		BOOST_TEST(flt(function_call{}) == east_value_term{calls_num+1});
}
BOOST_FIXTURE_TEST_CASE(print, mock_exenv_fixture)
{
	std::stringstream out, out2;
	out << expr_filter(&env, east_value_term{40});
	out2 << east_value_term{40};
	BOOST_TEST(out.str() == out2.str());
}
BOOST_AUTO_TEST_SUITE_END() // filter

BOOST_AUTO_TEST_SUITE(solver)
BOOST_DATA_TEST_CASE_F(mock_solver_fixture, simples_num
       , tdata::random(-90000000,90000000)^tdata::xrange(3)
       , v, ind)
{
	BOOST_TEST(solver(value_term{v}) == east_value_term{(double)v});
	BOOST_TEST(solver(value_term{ind}) == east_value_term{(double)ind});
}
BOOST_FIXTURE_TEST_CASE(simples, mock_solver_fixture)
{
	BOOST_TEST(solver(value_term{42}) == east_value_term{(double)42});
	BOOST_TEST(solver(value_term{"a"}) == east_value_term{"a"});

	cppjinja::ast::tuple_v tuple_test_v;
	tuple_test_v.fields.push_back(value_term{42});
	tuple_test_v.fields.push_back(value_term{"a"});
	value_term value_test{std::move(tuple_test_v)};
	cppjinja::east::array_v tuple_right_v;
	tuple_right_v.items.push_back(std::make_unique<east_value_term>((double)42));
	tuple_right_v.items.push_back(std::make_unique<east_value_term>("a"));
	east_value_term right_array{std::move(tuple_right_v)};
	BOOST_TEST(solver(value_test) == right_array);

	cppjinja::ast::array_v array_test_v;
	array_test_v.fields.push_back(value_term{(double)42});
	array_test_v.fields.push_back(value_term{"a"});
	value_test = value_term{std::move(array_test_v)};
	BOOST_TEST(solver(value_test) == right_array);
}
BOOST_AUTO_TEST_SUITE(functions)
BOOST_FIXTURE_TEST_CASE(from_user_data, mock_solver_fixture)
{
	cppjinja::ast::var_name fnc_name{"a"};
	cppjinja::ast::function_call_parameter param1(value_term{1});
	value_term call{cppjinja::ast::function_call{fnc_name, {param1}}};

	expect_call(evar_name{"a"}, {cppjinja::east::function_parameter{std::nullopt, 1.0}}, evalue_term{(double)42});
	BOOST_TEST(solver(call) == east_value_term{(double)42});
}
BOOST_FIXTURE_TEST_CASE(solve_array_call, mock_solver_fixture)
{
	cppjinja::ast::function_call call{
		cppjinja::ast::array_calls{{var_name{"a"}, value_term{var_name{"b"s}}}},
		{}
	};
	expect_sovle(var_name{"b"}, evalue_term{40});
	expect_call(evar_name{"a","40"}, {}, evalue_term{42});
	BOOST_TEST(solver(call) == east_value_term{42});
}
BOOST_AUTO_TEST_SUITE_END() // functions
BOOST_DATA_TEST_CASE_F(mock_solver_fixture, binary_ops
          , tdata::make( value_term{1}, 2, "str"s )
          ^ tdata::make( value_term{1}, 3, "str"s )
          ^ tdata::make( comparator::eq, comparator::less, comparator::eq )
          ^ tdata::make( cppjinja::east::value_term{true}, true, true )
          , left, right, op, result )
{
	using cppjinja::ast::var_name;
	using cppjinja::ast::binary_op;
	binary_op bop{left, op, right};
	BOOST_TEST(solver(value_term{bop}) == result);

	expect_call(evar_name{"a"}, {}, solver(left));
	bop.left = value_term{cppjinja::ast::function_call{var_name{"a"}, {}}};
	BOOST_TEST(solver(value_term{bop}) == result);
}
BOOST_FIXTURE_TEST_CASE(by_name_from_local, mock_solver_fixture)
{
	cppjinja::ast::var_name vn{ "a" };
	expect_sovle(vn, evalue_term{42});
	BOOST_TEST(solver(value_term{vn}) == east_value_term{42});
}
BOOST_AUTO_TEST_CASE(cannot_create_without_context)
{
	BOOST_CHECK_THROW(cppjinja::evt::expr_solver(nullptr), std::exception);
}
BOOST_FIXTURE_TEST_CASE(var_name_array, mock_solver_fixture)
{
	using namespace cppjinja::ast;
	auto val = std::make_shared<cppjinja::evt::context_objects::value>(evalue_term{"ok"s});
	auto neested_var = std::make_shared<cppjinja::evt::context_objects::value>(evalue_term{"c"s});
	MOCK_EXPECT(all_ctx.find).with(evar_name{"a"s, "c"s}).returns(val);
	MOCK_EXPECT(all_ctx.find).with(evar_name{"b"s}).returns(neested_var);
	BOOST_TEST(solver(array_calls{{var_name{"a"s}, value_term{var_name{"b"}}}}) == val->solve());
}
BOOST_AUTO_TEST_SUITE_END() // solver

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
	BOOST_REQUIRE(val_after_set);
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
	mocks::callable_node node;
	ctx.push_shadow(&node);
	calls.push(&node, cppjinja::evt::context_objects::callable_params({},{}));
	BOOST_CHECK(dynamic_cast<cppjinja::evt::context_objects::jinja_namespace*>(
	                env.all_ctx().find(evar_name{"namespace"}).get()));
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
	cppjinja::evt::context_objects::callable_params params(
	{}, {cppjinja::east::function_parameter{"p1"s, "ok"s}});
	ctx.push_shadow(&calling1);
	calls.push(&calling1, params);
	BOOST_TEST(env.params().find(evar_name{"p1"s}) != nullptr);
}
BOOST_FIXTURE_TEST_CASE(user_data, impl_exenv_fixture)
{
	MOCK_EXPECT(data.value_var_name).with(evar_name{"a"s}).returns(evalue_term{42});
	BOOST_TEST(env.user_data().find(evar_name{"a"})->solve() == evalue_term{42});
}
BOOST_FIXTURE_TEST_CASE(all_ctx, impl_exenv_fixture)
{
	mocks::callable_node maker;
	ctx.push_shadow(&maker);
	calls.push(&maker, cppjinja::evt::context_objects::callable_params({},{}));
	MOCK_EXPECT(data.value_var_name).with(evar_name{"a"s}).returns(evalue_term{42});
	BOOST_TEST(env.all_ctx().find(evar_name{"a"})->solve() == evalue_term{42});
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
	BOOST_TEST(fmt("\na"s) == "\na"s);
	{
		raii_result_format raii(&fmt, 2, -2);
		BOOST_TEST(fmt("\na"s) == "\n\t\ta"s);
	}
	BOOST_TEST(fmt("\na"s) == "\na"s);
}
BOOST_AUTO_TEST_SUITE_END() // raii

BOOST_AUTO_TEST_SUITE(result_formatter)
using cppjinja::evt::result_formatter;
BOOST_AUTO_TEST_CASE(single_shift)
{
	result_formatter fmt;
	BOOST_TEST(fmt("ok\nok"s) == "ok\nok"s);
	fmt.shift_tab(1);
	BOOST_TEST(fmt("ok"s) == "ok"s);
	BOOST_TEST(fmt("ok\nok"s) == "ok\n\tok"s);
	fmt.shift_tab(0);
	BOOST_TEST(fmt("ok\nok"s) == "ok\n\tok"s);
	fmt.shift_tab(-1);
	BOOST_TEST(fmt("ok\nok"s) == "ok\nok"s);
}
BOOST_AUTO_TEST_CASE(few_tabls)
{
	result_formatter fmt;
	BOOST_TEST(fmt("ok\nok"s) == "ok\nok"s);
	fmt.shift_tab(3);
	BOOST_TEST(fmt("ok"s) == "ok"s);
	BOOST_TEST(fmt("ok\nok"s) == "ok\n\t\t\tok"s);
	BOOST_CHECK_NO_THROW(fmt.shift_tab(-2));
	BOOST_TEST(fmt("ok\nok"s) == "ok\n\tok"s);
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
