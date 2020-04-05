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

using namespace std::literals;
namespace tdata = boost::unit_test::data;

struct mock_env_fixture
{
	std::unique_ptr<mocks::data_provider> data =
	        std::make_unique<mocks::data_provider>();
	cppjinja::evtree compiled;
	std::stringstream out;
};

struct mock_exenv_fixture : mock_env_fixture
{
	cppjinja::evt::exenv env;
	mock_exenv_fixture() : env(data.get(), &compiled, out) {}
};


BOOST_AUTO_TEST_SUITE(exenv)
BOOST_FIXTURE_TEST_CASE(environment, mock_env_fixture)
{
	cppjinja::evt::exenv env(data.get(), &compiled, out);
	BOOST_TEST(&env.tmpl() == &compiled);
	BOOST_TEST(env.data() == data.get());
	BOOST_TEST(&env.out() == &out);
}
BOOST_AUTO_TEST_SUITE(context)
BOOST_FIXTURE_TEST_CASE(current_node, mock_exenv_fixture)
{
	cppjinja::evt::context_new ctx;
	BOOST_CHECK_THROW(ctx.current_node(), std::exception);

	mocks::node fnode1, fnode2;
	ctx.current_node(&fnode1);
	BOOST_TEST(ctx.current_node() == &fnode1);

	ctx.current_node(&fnode2);
	BOOST_TEST(ctx.current_node() == &fnode2);
	BOOST_TEST(ctx.current_node(1) == &fnode1);
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
BOOST_AUTO_TEST_SUITE(solve_value)
BOOST_AUTO_TEST_CASE(simples)
{
	using cppjinja::ast::value_term;
	using east_value_term = cppjinja::east::value_term;
	cppjinja::evt::context_new ctx;
	BOOST_TEST(ctx.solve_value(value_term{42}) == east_value_term{42});
	BOOST_TEST(ctx.solve_value(value_term{"a"}) == east_value_term{"a"});

	cppjinja::ast::tuple_v tuple_test_v;
	tuple_test_v.fields.push_back(value_term{42});
	tuple_test_v.fields.push_back(value_term{"a"});
	value_term value_test{std::move(tuple_test_v)};
	cppjinja::east::array_v tuple_right_v;
	tuple_right_v.items.push_back(std::make_unique<east_value_term>(42));
	tuple_right_v.items.push_back(std::make_unique<east_value_term>("a"));
	east_value_term right_array{std::move(tuple_right_v)};
	BOOST_TEST(ctx.solve_value(value_test) == right_array);

	cppjinja::ast::array_v array_test_v;
	array_test_v.fields.push_back(value_term{42});
	array_test_v.fields.push_back(value_term{"a"});
	value_test = value_term{std::move(array_test_v)};
	BOOST_TEST(ctx.solve_value(value_test) == right_array);
}
BOOST_AUTO_TEST_CASE(variable_name)
{
	;
}
BOOST_AUTO_TEST_SUITE_END() // solve_value
BOOST_AUTO_TEST_SUITE_END() // context
BOOST_AUTO_TEST_SUITE_END() // exenv
