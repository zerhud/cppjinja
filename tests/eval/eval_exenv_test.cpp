/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE evaluator op_out

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "mocks.hpp"
#include "evtree/node.hpp"
#include "evtree/exenv.hpp"
#include "evtree/evtree.hpp"
#include "evtree/exenv/expr_solver.hpp"
#include "evtree/exenv/expr_filter.hpp"
#include "parser/operators/common.hpp"

using namespace std::literals;
namespace tdata = boost::unit_test::data;

using cppjinja::ast::comparator;
using cppjinja::ast::value_term;
using cppjinja::evt::expr_filter;
using east_value_term = cppjinja::east::value_term;

struct mock_env_fixture
{
	mocks::data_provider data;
	cppjinja::evtree compiled;
	std::stringstream out;
};

struct mock_exenv_fixture : mock_env_fixture
{
	cppjinja::evt::exenv env;
	cppjinja::evt::context& ctx;
	cppjinja::evt::callstack& calls;
	mock_exenv_fixture()
	    : env(&data, &compiled, out)
	    , ctx(env.ctx())
	    , calls(env.calls())
	{}
};

struct mock_solver_fixture : mock_exenv_fixture
{
	cppjinja::evt::expr_solver solver;
	mock_solver_fixture() : solver(&env) {}
};


BOOST_AUTO_TEST_SUITE(exenv)
BOOST_FIXTURE_TEST_CASE(environment, mock_exenv_fixture)
{
	BOOST_TEST(&env.out() == &out);
	BOOST_TEST(env.data() == &data);
	BOOST_TEST(&env.tmpl() == &compiled);
	BOOST_CHECK_NO_THROW(env.ctx());
	BOOST_CHECK_NO_THROW(env.calls());
}

BOOST_AUTO_TEST_SUITE(context)
BOOST_FIXTURE_TEST_CASE(current_node, mock_exenv_fixture)
{
	mocks::node fnode1, fnode2;

	BOOST_CHECK_THROW(ctx.current_node(), std::exception);
	BOOST_CHECK_THROW(ctx.current_node(&fnode1), std::exception);

	ctx.push(&fnode1);
	ctx.current_node(&fnode1);
	BOOST_TEST(ctx.current_node() == &fnode1);

	ctx.current_node(&fnode2);
	BOOST_TEST(ctx.current_node() == &fnode2);
	BOOST_TEST(ctx.current_node(1) == &fnode1);

	BOOST_CHECK_THROW(ctx.current_node(2), std::exception);
}
BOOST_FIXTURE_TEST_CASE(stack, mock_exenv_fixture)
{
	mocks::node fnode1, fnode2;
	BOOST_CHECK_THROW(ctx.maker(), std::exception);
	BOOST_CHECK_THROW(ctx.pop(&fnode1), std::exception);

	ctx.push(&fnode1);
	BOOST_TEST(ctx.maker() == &fnode1);
	BOOST_CHECK_THROW(ctx.pop(&fnode2), std::exception);
}
BOOST_AUTO_TEST_CASE(injection)
{
	cppjinja::evt::context ctx;
	mocks::node node;
	mocks::callable_node calling, wrong_calling;

	BOOST_CHECK_THROW(ctx.inject(nullptr), std::exception);
	BOOST_CHECK_THROW(ctx.takeout(nullptr), std::exception);
	BOOST_CHECK_THROW(ctx.injections(), std::exception);
	ctx.push(&node);

	BOOST_CHECK_THROW(ctx.inject(nullptr), std::exception);
	BOOST_TEST(ctx.injections().size() == 0);
	BOOST_CHECK_NO_THROW(ctx.inject(&calling));
	BOOST_TEST_REQUIRE(ctx.injections().size() == 1);
	BOOST_TEST(ctx.injections()[0] == &calling);

	BOOST_CHECK_THROW(ctx.takeout(&wrong_calling), std::exception);
	BOOST_TEST_REQUIRE(ctx.injections().size() == 1);
	BOOST_CHECK_NO_THROW(ctx.takeout(&calling));
	BOOST_TEST_REQUIRE(ctx.injections().size() == 0);

	ctx.pop(&node);
	BOOST_CHECK_THROW(ctx.injections(), std::exception);
}

BOOST_AUTO_TEST_SUITE(solve_name)
BOOST_FIXTURE_TEST_CASE(var_not_setted, mock_exenv_fixture)
{
	cppjinja::ast::var_name vn{ "a" };
	BOOST_CHECK_THROW(ctx.solve_var(vn), std::exception);

	mocks::node fnode1;
	ctx.push(&fnode1);
	BOOST_TEST(!ctx.solve_var(vn).has_value());
}
BOOST_FIXTURE_TEST_CASE(setted_variable, mock_exenv_fixture)
{
	cppjinja::ast::op_set data;
	data.name = "a";
	data.value = 42;
	cppjinja::evtnodes::op_set snode(std::move(data));
	mocks::node fnode1;

	ctx.push(&fnode1);
	ctx.current_node(&snode);

	cppjinja::ast::var_name vn{ "a" };
	BOOST_TEST(ctx.solve_var(vn) == cppjinja::ast::value_term{42});
}
BOOST_AUTO_TEST_SUITE_END() // solve_name

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
BOOST_FIXTURE_TEST_CASE(functions, mock_solver_fixture)
{
	cppjinja::ast::var_name fnc_name{"a"};
	cppjinja::ast::function_call_parameter param1(value_term{1});
	value_term call{cppjinja::ast::function_call{fnc_name, {param1}}};
	MOCK_EXPECT(data.value_function_call).once().returns(east_value_term{42});
	BOOST_TEST(solver(call) == east_value_term{42});
}
BOOST_DATA_TEST_CASE_F(mock_solver_fixture, binary_ops
          , tdata::make( value_term{1}, 2, "str"s )
          ^ tdata::make( value_term{1}, 3, "str"s )
          ^ tdata::make( comparator::eq, comparator::less, comparator::eq )
          ^ tdata::make( cppjinja::east::value_term{1}, 1, 1 )
          , left, right, op, result )
{
	using cppjinja::ast::var_name;
	using cppjinja::ast::binary_op;
	binary_op bop{left, op, right};
	BOOST_TEST(solver(value_term{bop}) == result);
	MOCK_EXPECT(data.value_function_call).once().returns(solver(left));
	bop.left = value_term{cppjinja::ast::function_call{var_name{"a"}, {}}};
	BOOST_TEST(solver(value_term{bop}) == result);
}
BOOST_FIXTURE_TEST_CASE(by_name_from_data, mock_solver_fixture)
{
	mocks::node caller;
	mocks::callable_node ctx_maker;
	ctx.push(&ctx_maker);
	calls.push(&caller, &ctx_maker);
	east_value_term result{42};
	cppjinja::ast::var_name vn{ "a" };
	MOCK_EXPECT(data.value_var_name).once().returns(result);
	MOCK_EXPECT(ctx_maker.param).once().returns(std::nullopt);
	BOOST_TEST(solver({value_term{vn}}) == result);
}
BOOST_FIXTURE_TEST_CASE(by_name_from_setted, mock_solver_fixture)
{
	east_value_term result{42};
	cppjinja::ast::var_name vn{ "a" };
	mocks::node caller, some_cnt;
	mocks::callable_node ctx_maker;

	cppjinja::ast::op_set setted_cnt;
	setted_cnt.value = 42;
	setted_cnt.name = vn[0];
	cppjinja::evtnodes::op_set var_setter(setted_cnt);

	calls.push(&caller, &ctx_maker);
	ctx.push(&ctx_maker);
	ctx.current_node(&var_setter);
	ctx.current_node(&some_cnt);

	MOCK_EXPECT(ctx_maker.param).once().returns(std::nullopt);
	BOOST_TEST(solver(value_term{vn}) == result);
}
BOOST_FIXTURE_TEST_CASE(in_params, mock_solver_fixture)
{
	mocks::node caller;
	mocks::callable_node maker;
	MOCK_EXPECT(maker.param).once().returns(value_term{42});

	ctx.push(&maker);
	calls.push(&caller, &maker);
	cppjinja::ast::var_name vn{ "a" };
	BOOST_TEST(solver(value_term{vn}) == east_value_term{42});
}
BOOST_FIXTURE_TEST_CASE(two_in_stack_one_in_ctx, mock_solver_fixture)
{
	mocks::node caller1, caller2;
	mocks::callable_node maker, called;
	ctx.push(&maker);
	calls.push(&caller1, &maker);
	calls.push(&caller2, &called);
	mock::sequence seq;
	MOCK_EXPECT(called.param).once().in(seq).returns(std::nullopt);
	MOCK_EXPECT(maker.param).once().in(seq).returns(value_term{42});
	cppjinja::ast::var_name vn{ "a" };
	BOOST_TEST(solver(value_term{vn}) == east_value_term{42});
}
BOOST_FIXTURE_TEST_CASE(no_calling_in_ctx, mock_solver_fixture)
{
	mocks::node caller;
	mocks::callable_node calling, maker;
	ctx.push(&maker);
	calls.push(&caller, &calling);
	MOCK_EXPECT(calling.param).once().returns(std::nullopt);
	BOOST_CHECK_THROW(solver(cppjinja::ast::var_name{"a"}), std::exception);
}
BOOST_AUTO_TEST_CASE(cannot_create_without_context)
{
	BOOST_CHECK_THROW(cppjinja::evt::expr_solver(nullptr), std::exception);
}
BOOST_AUTO_TEST_SUITE_END() // solver

BOOST_AUTO_TEST_SUITE(filter)
BOOST_FIXTURE_TEST_CASE(by_var, mock_exenv_fixture)
{
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
	MOCK_EXPECT(data.filter).once().calls(check);
	BOOST_TEST(expr_filter(&env, 2)(vn) == east_value_term{101});
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
BOOST_AUTO_TEST_SUITE_END() // filter

BOOST_AUTO_TEST_SUITE(callstack)
BOOST_FIXTURE_TEST_CASE(cannot_push_nullptr, mock_exenv_fixture)
{
	mocks::node caller;
	mocks::callable_node calling;
	BOOST_CHECK_THROW(calls.push(nullptr, nullptr), std::exception);
	BOOST_CHECK_THROW(calls.push(&caller, nullptr), std::exception);
	BOOST_CHECK_THROW(calls.push(nullptr, &calling), std::exception);
}
BOOST_FIXTURE_TEST_CASE(push_pop_caller, mock_exenv_fixture)
{
	mocks::node caller, caller2;
	mocks::callable_node calling, calling2;
	BOOST_CHECK_THROW(calls.caller(), std::exception);
	BOOST_CHECK_THROW(calls.calling_stack(), std::exception);
	BOOST_CHECK_THROW(calls.pop(&caller), std::exception);

	BOOST_CHECK_NO_THROW(calls.push(&caller, &calling));
	BOOST_TEST(calls.caller() == &caller);
	BOOST_TEST_REQUIRE(calls.calling_stack().size()==1);
	BOOST_TEST(calls.calling_stack()[0] == &calling);

	BOOST_CHECK_NO_THROW(calls.push(&caller2, &calling2));
	BOOST_TEST(calls.caller() == &caller2);
	BOOST_TEST(calls.calling_stack()[0] == &calling2);
	BOOST_CHECK_THROW(calls.pop(&caller), std::exception);
	BOOST_CHECK_NO_THROW(calls.pop(&caller2));
	BOOST_TEST(calls.caller() == &caller);
	BOOST_TEST(calls.calling_stack()[0] == &calling);
}
BOOST_FIXTURE_TEST_CASE(call_params, mock_exenv_fixture)
{
	mocks::node caller, caller2;
	mocks::callable_node calling;
	BOOST_CHECK_THROW(calls.call_params(), std::exception);
	BOOST_CHECK_NO_THROW(calls.push(&caller, &calling));
	BOOST_CHECK(calls.call_params().empty());

	cppjinja::ast::function_call_parameter p1;
	BOOST_CHECK_NO_THROW(calls.call_params({p1}));
	BOOST_TEST_REQUIRE(calls.call_params().size() == 1);
	BOOST_TEST(calls.call_params()[0] == p1);

	BOOST_CHECK_NO_THROW(calls.push(&caller2, &calling));
	BOOST_TEST(calls.calling_stack().size() == 2);
	BOOST_CHECK(calls.call_params().empty());
	BOOST_CHECK_NO_THROW(calls.pop(&caller2));
	BOOST_TEST_REQUIRE(calls.call_params().size() == 1);
	BOOST_TEST(calls.call_params()[0] == p1);
}
BOOST_FIXTURE_TEST_CASE(cannot_add_params_to_empty, mock_exenv_fixture)
{
	cppjinja::ast::function_call_parameter p1;
	BOOST_CHECK_THROW(calls.call_params({p1}), std::exception);
}
BOOST_AUTO_TEST_SUITE_END() // callstack

BOOST_AUTO_TEST_SUITE(raii)
BOOST_FIXTURE_TEST_CASE(push_ctx, mock_exenv_fixture)
{
	mocks::node maker;
	BOOST_CHECK_THROW(ctx.maker(), std::exception);
	{
		cppjinja::evt::raii_push_ctx pusher(&maker, &ctx);
		BOOST_TEST(ctx.maker() == &maker);
	}
	BOOST_CHECK_THROW(ctx.maker(), std::exception);
}
BOOST_FIXTURE_TEST_CASE(inject_to_ctx, mock_exenv_fixture)
{
	mocks::node maker;
	mocks::callable_node in;
	ctx.push(&maker);
	BOOST_TEST(ctx.injections().size() == 0);
	{
		cppjinja::evt::raii_inject injection(&in, &ctx);
		BOOST_TEST(ctx.injections().size() == 1);
	}
	BOOST_TEST(ctx.injections().size() == 0);
}
BOOST_FIXTURE_TEST_CASE(push_callstack, mock_exenv_fixture)
{
	mocks::node caller;
	mocks::callable_node calling;
	BOOST_CHECK_THROW(calls.caller(), std::exception);
	{
		cppjinja::evt::raii_push_callstack pusher(&caller, &calling, &calls);
		BOOST_TEST(calls.caller() == &caller);
		BOOST_TEST(calls.calling_stack()[0] == &calling);
	}
	BOOST_CHECK_THROW(calls.caller(), std::exception);
}
BOOST_AUTO_TEST_SUITE_END() // raii

BOOST_AUTO_TEST_SUITE_END() // exenv
