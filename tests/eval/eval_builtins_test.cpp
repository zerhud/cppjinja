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

#include "evtree/exenv/context_objects/builtins.hpp"
#include "evtree/exenv/context_objects/callable_node.hpp"
#include "evtree/exenv/context_objects/builtins/tests.hpp"
#include "evtree/exenv/context_objects/builtins/lambda.hpp"
#include "evtree/exenv/context_objects/tree.hpp"

using namespace std::literals;
using namespace absd::literals;
namespace tdata = boost::unit_test::data;
using var_name = cppjinja::east::var_name;
using fnc_param = cppjinja::evt::context_object::function_parameter;
using mocks::mock_exenv_fixture;
using cppjinja::evt::context_objects::make_val;

BOOST_AUTO_TEST_SUITE(phase_evaluate)
BOOST_AUTO_TEST_SUITE(context_object)
BOOST_AUTO_TEST_SUITE(builtins)
using function_parameter = cppjinja::evt::context_object::function_parameter;
BOOST_AUTO_TEST_CASE(context)
{
	using namespace cppjinja::evt::context_objects;
	cppjinja::evt::context_objects::builtins stdlib;

	BOOST_CHECK(dynamic_cast<join*>(stdlib.find(var_name{"join"_s}).get()));
	BOOST_CHECK(dynamic_cast<jinja_namespace*>(stdlib.find(var_name{"namespace"_s}).get()));
	BOOST_CHECK(dynamic_cast<builtins_tests::callable*>(stdlib.find(var_name{"$tests"_s, "callable"_s}).get()));
	BOOST_CHECK(dynamic_cast<builtins_tests::defined*>(stdlib.find(var_name{"$tests"_s, "defined"_s}).get()));
	BOOST_CHECK(dynamic_cast<builtins_tests::undefined*>(stdlib.find(var_name{"$tests"_s, "undefined"_s}).get()));
	BOOST_CHECK(dynamic_cast<builtins_tests::sameas*>(stdlib.find(var_name{"$tests"_s, "sameas"_s}).get()));

	BOOST_CHECK(stdlib.find(var_name{"upper"_s}).get());
}
BOOST_AUTO_TEST_CASE(upper)
{
	using param = cppjinja::evt::context_object::function_parameter;

	cppjinja::evt::context_objects::builtins stdlib;
	auto fnc = stdlib.find(var_name{"upper"});
	BOOST_REQUIRE(fnc!=nullptr);
	auto result = fnc->call({param{std::nullopt, make_val("text"_sd)}});
	BOOST_REQUIRE( result!=nullptr );
	BOOST_TEST( result->solve() == "TEXT" );
}
BOOST_AUTO_TEST_CASE(jinja_namespace)
{
	cppjinja::evt::context_objects::jinja_namespace ns;
	BOOST_CHECK_THROW(ns.call({}), std::exception);
	BOOST_CHECK_THROW(ns.solve(), std::exception);
	BOOST_CHECK_THROW(ns.add("a", nullptr), std::exception);
	BOOST_CHECK_THROW(ns.find(var_name{"a"}), std::exception);

	auto pval = std::make_shared<mocks::context_object>();
	auto tree = ns.call({function_parameter{"a", pval}});
	BOOST_CHECK(tree);
	BOOST_TEST(tree->find(var_name{"a"}) == pval);
}
BOOST_AUTO_TEST_CASE(join)
{
	using ctx_val = cppjinja::evt::context_objects::value;
	cppjinja::evt::context_objects::join j;
	auto sep = std::make_shared<ctx_val>(mock_exenv_fixture::create_absd_data(","));
	auto o1 = std::make_shared<ctx_val>(mock_exenv_fixture::create_absd_data("1"));
	auto o2 = std::make_shared<ctx_val>(mock_exenv_fixture::create_absd_data("2"));
	std::pmr::vector<function_parameter> params = {
	     function_parameter{"$"_s, sep}
	    ,function_parameter{std::nullopt, o1}
	};

	auto c1 = j.call(params);
	BOOST_TEST(c1->solve() == "1");

	params.push_back(function_parameter{std::nullopt, o2});
	auto c2 = j.call(params);
	BOOST_TEST(c2->solve() == "1,2");
}
BOOST_AUTO_TEST_CASE(lambda)
{
	using cppjinja::evt::context_objects::lambda_function;
	auto obj = std::make_shared<mocks::context_object>();
	lambda_function::fnc_type empty_fnc;
	BOOST_TEST((lambda_function(empty_fnc)).call({})->solve() == "");
	BOOST_TEST((lambda_function([obj](auto){return obj;})).call({}) == obj);
	BOOST_TEST((lambda_function([obj](auto p){
		BOOST_TEST(p.size()==2);
		return obj;
	})).call({function_parameter{}, function_parameter{}}) == obj);
}
BOOST_AUTO_TEST_SUITE(tests)
BOOST_FIXTURE_TEST_CASE(callable, mock_exenv_fixture)
{
	cppjinja::evt::context_objects::builtins_tests::callable obj;
	mocks::callable_node canode;
	auto nca = std::make_shared<mocks::context_object>();
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca}})->solve() == false);
	auto caobj = std::make_shared<cppjinja::evt::context_objects::callable_node>(&env, &canode);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, caobj}})->solve() == true);
}
BOOST_FIXTURE_TEST_CASE(defined, mock_exenv_fixture)
{
	cppjinja::evt::context_objects::builtins_tests::defined obj;
	auto nca = std::make_shared<mocks::context_object>();
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca}})->solve() == true);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nullptr}})->solve() == false);
}
BOOST_FIXTURE_TEST_CASE(undefined, mock_exenv_fixture)
{
	cppjinja::evt::context_objects::builtins_tests::undefined obj;
	auto nca = std::make_shared<mocks::context_object>();
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca}})->solve() == false);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nullptr}})->solve() == true);
}
BOOST_FIXTURE_TEST_CASE(sameas, mock_exenv_fixture)
{
	cppjinja::evt::context_objects::builtins_tests::sameas obj;
	auto nca1 = std::make_shared<mocks::context_object>();
	auto nca2 = std::make_shared<mocks::context_object>();
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca1}, fnc_param{std::nullopt, nca2}})->solve() == false);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca1}, fnc_param{std::nullopt, nca1}})->solve() == true);
}
BOOST_FIXTURE_TEST_CASE(upper, mock_exenv_fixture)
{
	cppjinja::evt::context_objects::builtins_tests::upper obj;
	auto nca_up = std::make_shared<mocks::context_object>();
	auto nca_low = std::make_shared<mocks::context_object>();
	expect_solve(*nca_up, "UP");
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca_up}})->solve() == true);
	expect_solve(*nca_low, "low");
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca_low}})->solve() == false);
}
BOOST_FIXTURE_TEST_CASE(lower, mock_exenv_fixture)
{
	cppjinja::evt::context_objects::builtins_tests::lower obj;
	auto nca_up = std::make_shared<mocks::context_object>();
	auto nca_low = std::make_shared<mocks::context_object>();
	expect_solve(*nca_up, "UP");
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca_up}})->solve() == false);
	expect_solve(*nca_low, "low");
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca_low}})->solve() == true);
}
BOOST_FIXTURE_TEST_CASE(iterable, mock_exenv_fixture)
{
	using cppjinja::evt::context_objects::tree;
	cppjinja::evt::context_objects::builtins_tests::iterable obj;
	auto nca_ar = std::make_shared<mocks::context_object>();
	auto nca_obj = std::make_shared<mocks::context_object>();
	auto nca_str = std::make_shared<mocks::context_object>();
	auto nca_int = std::make_shared<mocks::context_object>();
	auto v_ar = std::make_shared<absd::simple_data_holder>();
	v_ar->push_back() = 1;
	auto v_obj = std::make_shared<absd::simple_data_holder>();
	v_obj->put("key") = 1;
	MOCK_EXPECT(nca_ar->solve).returns(absd::data{v_ar});
	MOCK_EXPECT(nca_obj->solve).returns(absd::data{v_obj});
	expect_solve(*nca_str, "test");
	expect_solve(*nca_int, 42);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, std::make_shared<tree>()}})->solve() == true);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca_ar}})->solve() == true);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca_obj}})->solve() == true);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca_str}})->solve() == true);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca_int}})->solve() == false);
}
BOOST_AUTO_TEST_SUITE_END() // tests
BOOST_AUTO_TEST_SUITE_END() // builtins
BOOST_AUTO_TEST_SUITE_END() // context_object
BOOST_AUTO_TEST_SUITE_END() // phase_evaluate
