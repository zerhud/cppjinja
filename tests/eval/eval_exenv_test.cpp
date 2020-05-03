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
#include "evtree/nodes/callable.hpp"
#include "parser/operators/common.hpp"
#include "parser/grammar/tmpls.hpp"
#include "parser/parse.hpp"

using namespace std::literals;
namespace tdata = boost::unit_test::data;

using cppjinja::ast::comparator;
using cppjinja::ast::value_term;
using cppjinja::evt::expr_filter;
using east_value_term = cppjinja::east::value_term;
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
	BOOST_TEST(value(4).solve() == east_value_term{4});
}
BOOST_AUTO_TEST_SUITE_END() // value
BOOST_AUTO_TEST_SUITE(callable_node)
BOOST_AUTO_TEST_CASE(no_tree_methods_and_no_solve)
{
	mocks::callable_node node;
	mocks::exenv env;
	cppjinja::evt::context_objects::callable_node val(&env, &node);
	BOOST_CHECK_THROW(val.add("a",std::make_shared<tree>()), std::exception);
	BOOST_CHECK_THROW(val.find(var_name{"a"}), std::exception);
	BOOST_CHECK_THROW(val.solve(), std::exception);
}
BOOST_FIXTURE_TEST_CASE(call, mock_exenv_fixture)
{
	mocks::callable_node node;
	cppjinja::evt::context_objects::callable_node val(&env, &node);
	mock::sequence seq;
	cppjinja::east::function_parameter p1{"a"s, east_value_term{"ok"s}};
	MOCK_EXPECT(calls.push).once().in(seq).calls([&node,&p1](auto*n, auto params){
		BOOST_TEST(n == &node);
		auto sp1 = params.find(var_name{"a"});
		BOOST_TEST(sp1->solve() == *p1.val);
	});
	MOCK_EXPECT(node.evaluate)
	        .once()
	        .in(seq)
	        .with([this](auto& e){return &e==&env;})
	        .returns("ok"s);
	MOCK_EXPECT(calls.pop).once().in(seq);
	MOCK_EXPECT(node.solved_params).at_least(1).returns(std::vector<cppjinja::east::function_parameter>{});
	BOOST_TEST(val.call({p1}) == east_value_term{"ok"s});
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
	function_parameter c1{std::nullopt, east_value_term{41}};
	function_parameter c2{"p2"s, east_value_term{42}};
	function_parameter c3{"p3"s, east_value_term{43}};
	function_parameter c4{std::nullopt, east_value_term{44}};

	function_parameter p1{"p1", std::nullopt};
	function_parameter p2{"p2", east_value_term{52}};
	function_parameter p4{"p4", east_value_term{54}};
	function_parameter p5{"p5", east_value_term{55}};

	cppjinja::evt::context_objects::callable_params obj({p1, p2, p4, p5}, {c1, c2, c4, c3});
	BOOST_TEST(obj.find(var_name{"p1"})->solve() == east_value_term{41});
	BOOST_TEST(obj.find(var_name{"p2"})->solve() == east_value_term{42});
	BOOST_TEST(obj.find(var_name{"p3"})->solve() == east_value_term{43});
	BOOST_TEST(obj.find(var_name{"p4"})->solve() == east_value_term{44});
	BOOST_TEST(obj.find(var_name{"p5"})->solve() == east_value_term{55});
	BOOST_TEST(obj.find(var_name{"P1"}) == nullptr);
}
BOOST_AUTO_TEST_SUITE_END() // callable_params
BOOST_AUTO_TEST_SUITE(queue)
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

	MOCK_EXPECT(obj1.find).once().with(var_name{"a"}).returns(nullptr);
	MOCK_EXPECT(obj2.find).once().with(var_name{"a"}).returns(nullptr);
	MOCK_EXPECT(obj3->find).once().with(var_name{"a"}).returns(nullptr);
	q.add("", obj3);
	BOOST_TEST(q.find(var_name{"a"}) == nullptr);

	auto obj4 = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(obj1.find).once().with(var_name{"a"}).returns(nullptr);
	MOCK_EXPECT(obj2.find).once().with(var_name{"a"}).returns(obj4);
	BOOST_TEST(q.find(var_name{"a"}) == obj4);
}
BOOST_AUTO_TEST_CASE(cannot_add_twice)
{
	cppjinja::evt::context_objects::queue q({});
	auto obj1 = std::make_shared<mocks::context_object>();
	BOOST_CHECK_THROW(q.add("a", obj1), std::exception);
	BOOST_CHECK_THROW(q.add("", nullptr), std::exception);
	q.add("", obj1);
	BOOST_CHECK_THROW(q.add("", obj1), std::exception);
}
BOOST_AUTO_TEST_SUITE_END() // queue
BOOST_AUTO_TEST_SUITE(user_data)
BOOST_AUTO_TEST_CASE(cannot_add)
{
	auto prov = std::make_shared<mocks::data_provider>();
	cppjinja::evt::context_objects::user_data obj(prov);
	BOOST_CHECK_THROW(obj.add("", nullptr), std::exception);
}
BOOST_AUTO_TEST_CASE(find_is_link)
{
	auto prov = std::make_shared<mocks::data_provider>();
	cppjinja::evt::context_objects::user_data obj(prov);
	auto obj_a = obj.find(var_name{"a"s});
	BOOST_REQUIRE(obj_a);
	MOCK_EXPECT(prov->value_var_name).with(var_name{"a"s}).returns(east_value_term{"ok"s});
	BOOST_TEST(obj_a->solve() == east_value_term{"ok"s});

	auto obj_b = obj.find(var_name{"b"s});
	MOCK_EXPECT(prov->value_function_call).calls([](auto call){
		BOOST_TEST(call.ref.size()==1);
		BOOST_TEST(call.ref.at(0) == "b");
		BOOST_TEST(call.params.size() == 1);
		BOOST_TEST(*call.params.at(0).name == "p1"s);
		BOOST_TEST(*call.params.at(0).val == east_value_term{"p1_val"s});
		return east_value_term{"ok"s};
	} );
	cppjinja::east::function_parameter p1;
	p1.name = "p1"s;
	p1.val = east_value_term{"p1_val"s};
	BOOST_TEST(obj_b->call({p1}) == east_value_term{"ok"s});
}
BOOST_AUTO_TEST_SUITE_END() // user_data
BOOST_AUTO_TEST_SUITE_END() // context_object

BOOST_AUTO_TEST_SUITE(context)
BOOST_FIXTURE_TEST_CASE(current_node, mock_impls_fixture)
{
	mocks::node fnode1, fnode2;

	BOOST_CHECK_THROW(ctx.nth_node_on_stack(0), std::exception);
	BOOST_CHECK_THROW(ctx.current_node(&fnode1), std::exception);

	ctx.push(&fnode1);
	ctx.current_node(&fnode1);
	BOOST_TEST(ctx.nth_node_on_stack(0) == &fnode1);

	ctx.current_node(&fnode2);
	BOOST_TEST(ctx.nth_node_on_stack(0) == &fnode2);
	BOOST_TEST(ctx.nth_node_on_stack(1) == &fnode1);

	BOOST_CHECK_THROW(ctx.nth_node_on_stack(2), std::exception);
}
BOOST_FIXTURE_TEST_CASE(stack, mock_impls_fixture)
{
	mocks::node fnode1, fnode2;
	BOOST_CHECK_THROW(ctx.maker(), std::exception);
	BOOST_CHECK_THROW(ctx.pop(&fnode1), std::exception);

	ctx.push(&fnode1);
	BOOST_TEST(ctx.maker() == &fnode1);
	BOOST_CHECK_THROW(ctx.pop(&fnode2), std::exception);

	ctx.push(&fnode2);
	BOOST_TEST(ctx.maker() == &fnode2);

	BOOST_CHECK_NO_THROW(ctx.pop(&fnode2));
	BOOST_TEST(ctx.maker() == &fnode1);
}
BOOST_FIXTURE_TEST_CASE(cur_namespace, mock_impls_fixture)
{
	BOOST_CHECK_THROW(ctx.cur_namespace(), std::exception);
	mocks::node maker;
	ctx.push(&maker);
	BOOST_CHECK_NO_THROW(ctx.cur_namespace());
}

BOOST_FIXTURE_TEST_CASE(out, mock_impls_fixture)
{
	mocks::node node1, node2;
	BOOST_CHECK_THROW(ctx.out(), std::exception);
	BOOST_CHECK_THROW(ctx.result(), std::exception);
	ctx.push(&node1);
	BOOST_REQUIRE_NO_THROW(ctx.out());
	std::ostream* out1 = &ctx.out();
	BOOST_TEST(ctx.result() == "");
	ctx.out() << "test";
	BOOST_TEST(ctx.result() == "test");
	ctx.push(&node2);
	BOOST_TEST(ctx.result() == "");
	BOOST_TEST(&ctx.out() != out1);
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
	mocks::exenv env;
	mocks::callable_node calling;
	BOOST_CHECK_THROW(calls.call(nullptr, nullptr, {}), std::exception);
	BOOST_CHECK_THROW(calls.call(&env, nullptr, {}), std::exception);
	BOOST_CHECK_THROW(calls.call(nullptr, &calling, {}), std::exception);
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
BOOST_FIXTURE_TEST_CASE(call_block, mock_impls_fixture)
{
	using namespace cppjinja::ast;

	mocks::exenv env;
	mocks::callable_node calling;
	MOCK_EXPECT(calling.params).returns(std::vector<macro_parameter>{
	                                       macro_parameter{"p1", std::nullopt},
	                                       macro_parameter{"p2", value_term{52}},
	                                       macro_parameter{"p4", value_term{44}}
	                                    });
	MOCK_EXPECT(calling.evaluate).once().calls(
	[this,&env,&calling](cppjinja::evt::exenv& ienv){
		BOOST_TEST(&ienv == &env);
		auto params = calls.param_stack(&calling);
		BOOST_TEST_REQUIRE(params.size() == 1);
		BOOST_TEST((*params[0])(var_name{"p1"}) == value_term{41});
		BOOST_TEST((*params[0])(var_name{"p2"}) == value_term{42});
		BOOST_TEST((*params[0])(var_name{"p3"}) == value_term{43});
		BOOST_TEST((*params[0])(var_name{"p4"}) == value_term{44});
		return "ok"s;
	});
	function_call_parameter p1(value_term{41});
	function_call_parameter p2("p2", value_term{42});
	function_call_parameter p3("p3", value_term{43});
	BOOST_TEST(calls.call(&env, &calling, {p1,p2,p3}) == "ok");
}
BOOST_FIXTURE_TEST_CASE(param_stack, mock_impls_fixture)
{
	using namespace cppjinja::ast;

	mocks::exenv env;
	mocks::callable_node calling1, calling2;
	MOCK_EXPECT(calling1.params).calls([]()->std::vector<macro_parameter>{return {};});
	MOCK_EXPECT(calling2.params).calls([]()->std::vector<macro_parameter>{
	                                       return {macro_parameter{"p",value_term{42}}};
	                                   });
	MOCK_EXPECT(calling1.evaluate).once().calls(
	[this,&calling1,&env,&calling2](cppjinja::evt::exenv&){
		BOOST_TEST(calls.param_stack(&calling1).size()==1);
		calls.call(&env, &calling2, {});
		BOOST_TEST(calls.param_stack(&calling1).size()==1);
		return "c1";
	});
	MOCK_EXPECT(calling2.evaluate).once().calls(
	[this,&calling1](cppjinja::evt::exenv&){
		BOOST_TEST(calls.param_stack(&calling1).size()==2);
		BOOST_TEST((*calls.param_stack(&calling1)[1])(var_name{"p"}).has_value() == false);
		BOOST_TEST((*calls.param_stack(&calling1)[0])(var_name{"p"}) == value_term{42});
		BOOST_CHECK_THROW(calls.param_stack(nullptr), std::exception);
		return "c2";
	});
	BOOST_TEST(calls.param_stack(nullptr).size() == 0);
	BOOST_TEST(calls.param_stack(&calling1).size() == 0);
	calls.call(&env, &calling1, {});
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
	BOOST_TEST(expr_filter(&env, 2)(vn) == east_value_term{101});
	BOOST_TEST(expr_filter(&env, 2)(filter_call{vn}) == east_value_term{101});
}
BOOST_FIXTURE_TEST_CASE(by_fnc, mock_exenv_fixture)
{
	cppjinja::ast::function_call fc;
	fc.ref.push_back("a");
	fc.params.emplace_back(value_term{42});
	auto check = [&fc](
	          const cppjinja::east::function_call& udfc
	        , const east_value_term& base){
		BOOST_TEST_REQUIRE(fc.ref.size()==1);
		BOOST_TEST(udfc.ref[0]==fc.ref[0]);
		BOOST_TEST(base == east_value_term{2});
		BOOST_TEST(udfc.params.size() == fc.params.size());
		BOOST_TEST(udfc.params[0].name.has_value() == fc.params[0].name.has_value());
		BOOST_CHECK(udfc.params[0].val.has_value());
		BOOST_TEST(*udfc.params[0].val == east_value_term{42});
		return east_value_term{101};
	};
	MOCK_EXPECT(data.filter).once().calls(check);
	BOOST_TEST(expr_filter(&env, 2)(fc) == east_value_term{101});
	MOCK_EXPECT(data.filter).once().calls(check);
	BOOST_TEST(expr_filter(&env, 2)(value_term{fc}) == east_value_term{101});
}
BOOST_FIXTURE_TEST_CASE(by_wrong, mock_exenv_fixture)
{
	using namespace cppjinja::ast;
	BOOST_CHECK_THROW(expr_filter(nullptr, 42), std::exception);
	BOOST_CHECK_THROW(expr_filter(&env, 42)(2), std::exception);
	BOOST_CHECK_THROW(expr_filter(&env, 42)("str"s), std::exception);
	BOOST_CHECK_THROW(expr_filter(&env, 42)(tuple_v{}), std::exception);
	BOOST_CHECK_THROW(expr_filter(&env, 42)(array_v{}), std::exception);
	BOOST_CHECK_THROW(expr_filter(&env, 42)(binary_op{}), std::exception);
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
	expr_filter flt(&env, 40);
	for(;calls_num < 45;++calls_num)
		BOOST_TEST(flt(var_name{"a"}) == east_value_term{calls_num+1});
	for(;calls_num < 50;++calls_num)
		BOOST_TEST(flt(function_call{}) == east_value_term{calls_num+1});
}
BOOST_FIXTURE_TEST_CASE(print, mock_exenv_fixture)
{
	std::stringstream out;
	out << expr_filter(&env, 40);
	BOOST_TEST(out.str() == "40");
}
BOOST_AUTO_TEST_SUITE_END() // filter

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
	ctx.push(&maker);
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
	ctx.push(&fnode2);
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
	ctx.push(&ctxmaker);
	BOOST_TEST(&env.out() == &ctx.out());
	BOOST_TEST(env.result() == "");
	ctx.out() << "test";
	BOOST_TEST(env.result() == "test");
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
	ctx.push(&maker);
	BOOST_CHECK_NO_THROW(env.locals());
}
BOOST_FIXTURE_TEST_CASE(params, impl_exenv_fixture)
{
	using namespace cppjinja::ast;
	mocks::callable_node calling1, calling2;
	MOCK_EXPECT(calling1.params).calls([]()->std::vector<macro_parameter>{return {};});
	MOCK_EXPECT(calling2.params).calls([]()->std::vector<macro_parameter>{
	                                       return {macro_parameter{"p",value_term{42}}};
	                                   });
	MOCK_EXPECT(calling2.evaluate).once().calls([this](cppjinja::evt::exenv&){
		BOOST_TEST(env.params().size() == 2);
		BOOST_TEST((*env.params().front())(var_name{"p"}) == value_term{42});
		BOOST_TEST((*env.params().back())(var_name{"p"}).has_value() == false);
		return "calling2";
	});
	MOCK_EXPECT(calling1.evaluate).once().calls([this,&calling2](cppjinja::evt::exenv&){
		BOOST_TEST(env.params().size() == 1);
		calls.call(&env, &calling2, {});
		BOOST_TEST(env.params().size() == 1);
		return "calling1";
	});
	ctx.push(&calling1);
	BOOST_TEST(env.params().size() == 0);
	calls.call(&env, &calling1, {});
	BOOST_TEST(env.params().size() == 0);
}

BOOST_AUTO_TEST_SUITE(solver)
BOOST_DATA_TEST_CASE_F(mock_solver_fixture, simples_num
       , tdata::random(-90000000.0,90000000.0)^tdata::xrange(93)
       , v, ind)
{
	BOOST_TEST(solver(value_term{v}) == east_value_term{v});
	BOOST_TEST(solver(value_term{ind}) == east_value_term{ind});
}
BOOST_FIXTURE_TEST_CASE(simples, mock_solver_fixture)
{
	BOOST_TEST(solver(value_term{42}) == east_value_term{42});
	BOOST_TEST(solver(value_term{"a"}) == east_value_term{"a"});

	cppjinja::ast::tuple_v tuple_test_v;
	tuple_test_v.fields.push_back(value_term{42});
	tuple_test_v.fields.push_back(value_term{"a"});
	value_term value_test{std::move(tuple_test_v)};
	cppjinja::east::array_v tuple_right_v;
	tuple_right_v.items.push_back(std::make_unique<east_value_term>(42));
	tuple_right_v.items.push_back(std::make_unique<east_value_term>("a"));
	east_value_term right_array{std::move(tuple_right_v)};
	BOOST_TEST(solver(value_test) == right_array);

	cppjinja::ast::array_v array_test_v;
	array_test_v.fields.push_back(value_term{42});
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

	expect_glp(1, 1, 1);
	MOCK_EXPECT(data.value_function_call).once().returns(east_value_term{42});
	BOOST_TEST(solver(call) == east_value_term{42});
}
BOOST_FIXTURE_TEST_CASE(call_other_block, mock_solver_fixture)
{
	cppjinja::ast::var_name fnc_name{"a"};
	value_term call{cppjinja::ast::function_call{fnc_name, {}}};
	locals.add("a", std::make_shared<cppjinja::evt::var_solver>(value_term{"result"}));
	expect_glp(0, 1, 1);
	BOOST_TEST(solver(call) == east_value_term{"result"});
}
BOOST_AUTO_TEST_SUITE_END() // functions
BOOST_DATA_TEST_CASE_F(mock_solver_fixture, binary_ops
          , tdata::make( value_term{1}, 2, "str"s )
          ^ tdata::make( value_term{1}, 3, "str"s )
          ^ tdata::make( comparator::eq, comparator::less, comparator::eq )
          ^ tdata::make( cppjinja::east::value_term{1}, 1, 1 )
          , left, right, op, result )
{
	using cppjinja::ast::var_name;
	using cppjinja::ast::binary_op;
	using cppjinja::evt::obj_holder;
	binary_op bop{left, op, right};
	expect_glp(1, 1, 1);
	BOOST_TEST(solver(value_term{bop}) == result);
	MOCK_EXPECT(data.value_function_call).once().returns(solver(left));
	bop.left = value_term{cppjinja::ast::function_call{var_name{"a"}, {}}};
	BOOST_TEST(solver(value_term{bop}) == result);
}
BOOST_FIXTURE_TEST_CASE(by_name_from_data, mock_solver_fixture)
{
	east_value_term result{42};
	cppjinja::ast::var_name vn{ "a" };
	expect_glp(1, 1, 1);
	MOCK_EXPECT(data.value_var_name).once().returns(result);
	BOOST_TEST(solver({value_term{vn}}) == result);
}
BOOST_FIXTURE_TEST_CASE(by_name_from_local, mock_solver_fixture)
{
	locals.add("a", std::make_shared<cppjinja::evt::var_solver>(value_term{42}));
	expect_glp(0, 1, 1);
	cppjinja::ast::var_name vn{ "a" };
	BOOST_TEST(solver(value_term{vn}) == east_value_term{42});
}
BOOST_FIXTURE_TEST_CASE(in_params, mock_solver_fixture)
{
	cppjinja::evt::obj_holder params1;
	params1.add("a", std::make_shared<cppjinja::evt::var_solver>(value_term{42}));
	params.emplace_back(&params1);
	expect_glp(0,0,1);

	cppjinja::ast::var_name vn{ "a" };
	BOOST_TEST(solver(value_term{vn}) == east_value_term{42});
}
BOOST_AUTO_TEST_CASE(cannot_create_without_context)
{
	BOOST_CHECK_THROW(cppjinja::evt::expr_solver(nullptr), std::exception);
}
BOOST_AUTO_TEST_SUITE_END() // solver

BOOST_AUTO_TEST_SUITE(raii)
BOOST_FIXTURE_TEST_CASE(push_ctx, impl_exenv_fixture)
{
	mocks::node maker;
	BOOST_CHECK_THROW(ctx.maker(), std::exception);
	{
		cppjinja::evt::raii_push_ctx pusher(&maker, &ctx);
		BOOST_TEST(ctx.maker() == &maker);
	}
	BOOST_CHECK_THROW(ctx.maker(), std::exception);
	{
		cppjinja::evt::raii_push_ctx pusher(&maker, &ctx);
		cppjinja::evt::raii_push_ctx pusher2(std::move(pusher));
		BOOST_TEST(ctx.maker() == &maker);
	}
	BOOST_CHECK_THROW(ctx.maker(), std::exception);
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

BOOST_AUTO_TEST_SUITE(ctx_objects)
BOOST_AUTO_TEST_SUITE(holder)
using cppjinja::evt::obj_holder;
using namespace cppjinja::ast;
BOOST_FIXTURE_TEST_CASE(add, mock_exenv_fixture)
{
	obj_holder ns;
	auto make_obj = [](){return std::make_unique<mocks::ctx_object>();};
	BOOST_CHECK_THROW(ns.rem("n"), std::exception);
	BOOST_CHECK_NO_THROW(ns.add("n", make_obj()));
	BOOST_CHECK_NO_THROW(ns.rem("n"));
	BOOST_CHECK_THROW(ns.rem("n"), std::exception);
}
BOOST_FIXTURE_TEST_CASE(find, mock_exenv_fixture)
{
	obj_holder ns;
	auto obj = std::make_shared<mocks::ctx_object>();
	ns.add("n", obj);
	BOOST_TEST(ns.find("n") == obj);
	BOOST_CHECK_THROW(ns.find("no"), std::exception);
}
BOOST_AUTO_TEST_CASE(call)
{
	obj_holder ns;
	auto obj_ = std::make_unique<mocks::ctx_object>();
	mocks::ctx_object* obj = obj_.get();
	ns.add("a", std::move(obj_));
	function_call call;
	call.ref.emplace_back("n");
	BOOST_TEST(ns(call).has_value() == false);
	call.ref.clear();

	MOCK_EXPECT(obj->call).once().calls([](function_call c){
		BOOST_TEST(c.ref.empty() == true);
		return value_term{"ok"s};
	});
	call.ref.emplace_back("a");
	BOOST_TEST(ns(call) == value_term{"ok"});

	MOCK_EXPECT(obj->call).once().calls([](function_call c){
		BOOST_TEST(c.ref.size() == 1);
		BOOST_TEST(c.ref[0] == "b");
		return value_term{"ok"s};
	});
	call.ref.emplace_back("b");
	BOOST_TEST(ns(call) == value_term{"ok"});
}
BOOST_AUTO_TEST_CASE(solve)
{
	obj_holder ns;
	auto obj_ = std::make_unique<mocks::ctx_object>();
	mocks::ctx_object* obj = obj_.get();
	ns.add("a", std::move(obj_));
	BOOST_TEST(ns(var_name{"b"s}).has_value() == false);

	MOCK_EXPECT(obj->solve).once().calls([](var_name n){
		BOOST_TEST(n.size() == 0);
		return value_term{"ok"s};
	});
	BOOST_TEST(ns(var_name{"a"}) == value_term{"ok"s});

	MOCK_EXPECT(obj->solve).once().calls([](var_name n){
		BOOST_TEST(n.size() == 1);
		BOOST_TEST(n[0] == "b"s);
		return value_term{"ok"s};
	});
	BOOST_TEST(ns(var_name{"a", "b"}) == value_term{"ok"s});
}
BOOST_AUTO_TEST_SUITE_END() // holder
BOOST_FIXTURE_TEST_CASE(delay_solver, mock_exenv_fixture)
{
	using namespace cppjinja::ast;
	value_term val{"ok"s};
	cppjinja::evt::delay_solver ds(&val);
	function_call fc;
	fc.ref.emplace_back("a"s);
	BOOST_CHECK_THROW(ds.call(fc), std::exception);
	BOOST_CHECK_THROW(ds.solve(var_name{"a"s}), std::exception);
	fc.ref.clear();
	BOOST_TEST(ds.call(fc) == value_term{"ok"});
	BOOST_TEST(ds.solve(var_name{}) == value_term{"ok"});
}
BOOST_FIXTURE_TEST_CASE(callable_solver, mock_exenv_fixture)
{
	using namespace cppjinja::ast;
	using cppjinja::evtnodes::callable_solver;
	mocks::callable_node node;
	callable_solver sl(&env, &node);
	BOOST_CHECK_THROW(sl.solve(var_name{"a"}), std::exception);
	function_call call;
	call.params.emplace_back("p1", value_term{"val1"});
	expect_call(&node, call.params);
	MOCK_EXPECT(node.evaluate).once().returns("ok");
	BOOST_TEST(sl.call(call) == value_term{"ok"});

	call.ref.emplace_back("a");
	BOOST_CHECK_THROW(sl.call(call), std::exception);
}
BOOST_FIXTURE_TEST_CASE(callable_multisolver, mock_exenv_fixture)
{
	using namespace cppjinja::ast;
	cppjinja::evtnodes::callable_multisolver sl(&env);
	mocks::callable_node block_a, block_b;

	function_call call;
	call.ref = {"a"s};
	BOOST_CHECK_THROW(sl.solve(var_name{"a"}), std::exception);
	BOOST_CHECK_THROW(sl.call(call), std::exception);

	sl.add("a", &block_a);
	sl.add("b", &block_b);
	sl.add("c", value_term{42});
	expect_call(&block_a, call.params);
	MOCK_EXPECT(block_a.evaluate).once().returns("block_a");
	BOOST_TEST(sl.call(call) == value_term{"block_a"});

	BOOST_CHECK_THROW(sl.solve(var_name{"no"}), std::exception);
	BOOST_CHECK_THROW(sl.solve(var_name{"c","c"}), std::exception);
	BOOST_TEST(sl.solve(var_name{"c"}) == value_term{42});

	call.ref = {"b"s};
	expect_call(&block_b, call.params);
	MOCK_EXPECT(block_b.evaluate).once().returns("block_b");
	BOOST_TEST(sl.call(call) == value_term{"block_b"});

	call.ref.emplace_back("b");
	BOOST_CHECK_THROW(sl.call(call), std::exception);
}
BOOST_AUTO_TEST_SUITE_END() // ctx_objects

BOOST_AUTO_TEST_SUITE_END() // exenv

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
