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
		BOOST_TEST(env.params().front()->solve(var_name{"p"}) == value_term{42});
		BOOST_TEST(env.params().back()->solve(var_name{"p"}).has_value() == false);
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
	ctx.inject_obj("n", std::make_unique<mocks::ctx_object>());
	BOOST_CHECK_NO_THROW(ctx.cur_namespace().rem("n"));
}

BOOST_AUTO_TEST_SUITE(solve_name)
BOOST_FIXTURE_TEST_CASE(inject_object, mock_impls_fixture)
{
	mocks::node node;
	auto make_obj = [](){return std::make_unique<mocks::ctx_object>();};
	BOOST_CHECK_THROW(ctx.inject_obj("n", make_obj()), std::exception);
	BOOST_CHECK_THROW(ctx.takeout_obj("n"), std::exception);
	ctx.push(&node);
	BOOST_CHECK_THROW(ctx.takeout_obj(""), std::exception);
	BOOST_CHECK_NO_THROW(ctx.inject_obj("n", make_obj()));
	BOOST_CHECK_NO_THROW(ctx.takeout_obj("n"));
	BOOST_CHECK_THROW(ctx.takeout_obj("n"), std::exception);
}
BOOST_FIXTURE_TEST_CASE(inject_object_solves, mock_impls_fixture)
{
	using namespace cppjinja::ast;
	mocks::node node;
	auto obj = std::make_unique<mocks::ctx_object>();
	MOCK_EXPECT(obj->solve).exactly(2).returns(value_term{"ok"s});
	ctx.push(&node);
	BOOST_CHECK_NO_THROW(ctx.inject_obj("n", std::move(obj)));
	BOOST_TEST(ctx.cur_namespace().solve(var_name{"n"s}) == value_term{"ok"});
	BOOST_TEST(ctx.cur_namespace().solve(var_name{"n"s, "a"s}) == value_term{"ok"});
}
BOOST_FIXTURE_TEST_CASE(inject_object_calls, mock_impls_fixture)
{
	using namespace cppjinja::ast;
	mocks::node node;
	ctx.push(&node);
	auto obj1 = std::make_unique<mocks::ctx_object>();
	auto obj2 = std::make_unique<mocks::ctx_object>();
	MOCK_EXPECT(obj1->call).once().returns(value_term{"ok1"s});
	MOCK_EXPECT(obj2->call).once().returns(value_term{"ok2"s});
	BOOST_CHECK_NO_THROW(ctx.inject_obj("n", std::move(obj1)));
	BOOST_CHECK_NO_THROW(ctx.inject_obj("self", std::move(obj2)));
	function_call call;
	call.ref = {"n"s};
	BOOST_TEST(ctx.cur_namespace().call(call) == value_term{"ok1"});
	call.ref = {"self"s};
	BOOST_TEST(ctx.cur_namespace().call(call) == value_term{"ok2"});
}
BOOST_AUTO_TEST_SUITE_END() // solve_name

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
		BOOST_TEST(udfc.params[0].val == east_value_term{42});
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

BOOST_AUTO_TEST_SUITE(callstack)
BOOST_FIXTURE_TEST_CASE(cannot_call_nullptr, mock_impls_fixture)
{
	mocks::exenv env;
	mocks::callable_node calling;
	BOOST_CHECK_THROW(calls.call(nullptr, nullptr, {}), std::exception);
	BOOST_CHECK_THROW(calls.call(&env, nullptr, {}), std::exception);
	BOOST_CHECK_THROW(calls.call(nullptr, &calling, {}), std::exception);
}
BOOST_FIXTURE_TEST_CASE(no_callparams_in_empty, mock_impls_fixture)
{
	BOOST_CHECK_THROW(calls.call_params(), std::exception);
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
	[this,&env](cppjinja::evt::exenv& ienv){
		BOOST_TEST(&ienv == &env);
		auto& params = calls.params();
		BOOST_TEST(params.solve(var_name{"p1"}) == value_term{41});
		BOOST_TEST(params.solve(var_name{"p2"}) == value_term{42});
		BOOST_TEST(params.solve(var_name{"p3"}) == value_term{43});
		BOOST_TEST(params.solve(var_name{"p4"}) == value_term{44});
		return "ok"s;
	});
	function_call_parameter p1(value_term{41});
	function_call_parameter p2("p2", value_term{42});
	function_call_parameter p3("p3", value_term{43});
	BOOST_TEST(calls.call(&env, &calling, {p1,p2,p3}) == "ok");
	BOOST_CHECK_THROW(calls.calling_stack(), std::exception);
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
		BOOST_TEST(calls.param_stack(&calling1)[1]->solve(var_name{"p"}).has_value() == false);
		BOOST_TEST(calls.param_stack(&calling1)[0]->solve(var_name{"p"}) == value_term{42});
		BOOST_CHECK_THROW(calls.param_stack(nullptr), std::exception);
		return "c2";
	});
	BOOST_TEST(calls.param_stack(nullptr).size() == 0);
	BOOST_TEST(calls.param_stack(&calling1).size() == 0);
	calls.call(&env, &calling1, {});
}
BOOST_AUTO_TEST_SUITE_END() // callstack

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
BOOST_FIXTURE_TEST_CASE(inject_obj, mock_exenv_fixture)
{
	mock::sequence seq;
	MOCK_EXPECT(ctx.inject_obj).once().in(seq);
	MOCK_EXPECT(ctx.takeout_obj).once().in(seq).with("n");
	MOCK_EXPECT(ctx.inject_obj).once().in(seq);
	MOCK_EXPECT(ctx.takeout_obj).once().in(seq).with("n");

	cppjinja::evt::raii_inject_obj(
	            "n"s, std::make_unique<mocks::ctx_object>(), &ctx);

	cppjinja::evt::raii_inject_obj r1(
	            "n"s, std::make_unique<mocks::ctx_object>(), &ctx);
	cppjinja::evt::raii_inject_obj r2(std::move(r1));
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
	BOOST_TEST(ns.call(call).has_value() == false);
	call.ref.clear();

	MOCK_EXPECT(obj->call).once().calls([](function_call c){
		BOOST_TEST(c.ref.empty() == true);
		return value_term{"ok"s};
	});
	call.ref.emplace_back("a");
	BOOST_TEST(ns.call(call) == value_term{"ok"});

	MOCK_EXPECT(obj->call).once().calls([](function_call c){
		BOOST_TEST(c.ref.size() == 1);
		BOOST_TEST(c.ref[0] == "b");
		return value_term{"ok"s};
	});
	call.ref.emplace_back("b");
	BOOST_TEST(ns.call(call) == value_term{"ok"});
}
BOOST_AUTO_TEST_CASE(solve)
{
	obj_holder ns;
	auto obj_ = std::make_unique<mocks::ctx_object>();
	mocks::ctx_object* obj = obj_.get();
	ns.add("a", std::move(obj_));
	BOOST_TEST(ns.solve(var_name{"b"s}).has_value() == false);

	MOCK_EXPECT(obj->solve).once().calls([](var_name n){
		BOOST_TEST(n.size() == 0);
		return value_term{"ok"s};
	});
	BOOST_TEST(ns.solve(var_name{"a"}) == value_term{"ok"s});

	MOCK_EXPECT(obj->solve).once().calls([](var_name n){
		BOOST_TEST(n.size() == 1);
		BOOST_TEST(n[0] == "b"s);
		return value_term{"ok"s};
	});
	BOOST_TEST(ns.solve(var_name{"a", "b"}) == value_term{"ok"s});
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
