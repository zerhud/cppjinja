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
#include "parser/operators/common.hpp"

using namespace std::literals;
namespace tdata = boost::unit_test::data;

using cppjinja::ast::comparator;
using cppjinja::ast::value_term;
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
	cppjinja::evt::context_new ctx;
	mock_exenv_fixture() : env(&data, &compiled, out) {}
};


BOOST_AUTO_TEST_SUITE(exenv)
BOOST_FIXTURE_TEST_CASE(environment, mock_exenv_fixture)
{
	BOOST_TEST(&env.out() == &out);
	BOOST_TEST(env.data() == &data);
	BOOST_TEST(&env.tmpl() == &compiled);
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
	cppjinja::evt::context_new ctx;
	mocks::node fnode1, fnode2;
	BOOST_CHECK_THROW(ctx.maker(), std::exception);
	BOOST_CHECK_THROW(ctx.pop(&fnode1), std::exception);

	ctx.push(&fnode1);
	BOOST_TEST(ctx.maker() == &fnode1);
	BOOST_CHECK_THROW(ctx.pop(&fnode2), std::exception);
}
BOOST_AUTO_TEST_CASE(injection)
{
	cppjinja::evt::context_new ctx;
	mocks::node fnode1, fnode2, fnode3;

	BOOST_CHECK_THROW(ctx.inject(nullptr), std::exception);
	BOOST_CHECK_THROW(ctx.takeout(nullptr), std::exception);
	ctx.push(&fnode1);

	BOOST_CHECK_THROW(ctx.inject(nullptr), std::exception);
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
BOOST_FIXTURE_TEST_CASE(in_params, mock_exenv_fixture, * boost::unit_test::disabled())
{
	cppjinja::ast::var_name vn{ "a" };
	cppjinja::ast::value_term val{42};

	mocks::callable_node maker;
	MOCK_EXPECT(maker.param).once().returns(val);

	ctx.push(&maker);
	BOOST_TEST(ctx.solve_var(vn) == val);
}
BOOST_AUTO_TEST_SUITE_END() // solve_name
BOOST_AUTO_TEST_SUITE_END() // context

BOOST_AUTO_TEST_SUITE(solve_value)
BOOST_FIXTURE_TEST_CASE(simples, mock_exenv_fixture)
{
	BOOST_TEST(env.solve_value(value_term{42}) == east_value_term{42});
	BOOST_TEST(env.solve_value(value_term{"a"}) == east_value_term{"a"});

	cppjinja::ast::tuple_v tuple_test_v;
	tuple_test_v.fields.push_back(value_term{42});
	tuple_test_v.fields.push_back(value_term{"a"});
	value_term value_test{std::move(tuple_test_v)};
	cppjinja::east::array_v tuple_right_v;
	tuple_right_v.items.push_back(std::make_unique<east_value_term>(42));
	tuple_right_v.items.push_back(std::make_unique<east_value_term>("a"));
	east_value_term right_array{std::move(tuple_right_v)};
	BOOST_TEST(env.solve_value(value_test) == right_array);

	cppjinja::ast::array_v array_test_v;
	array_test_v.fields.push_back(value_term{42});
	array_test_v.fields.push_back(value_term{"a"});
	value_test = value_term{std::move(array_test_v)};
	BOOST_TEST(env.solve_value(value_test) == right_array);
}
BOOST_FIXTURE_TEST_CASE(functions, mock_exenv_fixture)
{
	cppjinja::ast::var_name fnc_name{"a"};
	cppjinja::ast::function_call_parameter param1(value_term{1});
	value_term call{cppjinja::ast::function_call{fnc_name, {param1}}};
	MOCK_EXPECT(data.value_function_call).once().returns(east_value_term{42});
	BOOST_TEST(env.solve_value(call) == east_value_term{42});
}
BOOST_DATA_TEST_CASE_F(mock_exenv_fixture, binary_ops
          , tdata::make( value_term{1}, 2, "str"s )
          ^ tdata::make( value_term{1}, 3, "str"s )
          ^ tdata::make( comparator::eq, comparator::less, comparator::eq )
          ^ tdata::make( cppjinja::east::value_term{1}, 1, 1 )
          , left, right, op, result )
{
	using cppjinja::ast::var_name;
	using cppjinja::ast::binary_op;
	binary_op bop{left, op, right};
	BOOST_TEST(env.solve_value(value_term{bop}) == result);
	MOCK_EXPECT(data.value_function_call).once().returns(env.solve_value(left));
	bop.left = value_term{cppjinja::ast::function_call{var_name{"a"}, {}}};
	BOOST_TEST(env.solve_value(value_term{bop}) == result);
}
BOOST_AUTO_TEST_SUITE_END() // solve_value
BOOST_AUTO_TEST_SUITE_END() // exenv
