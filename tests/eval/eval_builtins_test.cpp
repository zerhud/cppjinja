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
namespace tdata = boost::unit_test::data;
using var_name = cppjinja::east::var_name;
using fnc_param = cppjinja::evt::context_object::function_parameter;
using mocks::mock_exenv_fixture;

BOOST_AUTO_TEST_SUITE(phase_evaluate)
BOOST_AUTO_TEST_SUITE(context_object)
BOOST_AUTO_TEST_SUITE(builtins)
using function_parameter = cppjinja::evt::context_object::function_parameter;
BOOST_AUTO_TEST_CASE(context)
{
	using namespace cppjinja::evt::context_objects;
	cppjinja::evt::context_objects::builtins stdlib;

	BOOST_CHECK(dynamic_cast<join*>(stdlib.find(var_name{"join"s}).get()));
	BOOST_CHECK(dynamic_cast<jinja_namespace*>(stdlib.find(var_name{"namespace"s}).get()));
	BOOST_CHECK(dynamic_cast<builtins_tests::callable*>(stdlib.find(var_name{"$tests"s, "callable"s}).get()));
	BOOST_CHECK(dynamic_cast<builtins_tests::defined*>(stdlib.find(var_name{"$tests"s, "defined"s}).get()));
	BOOST_CHECK(dynamic_cast<builtins_tests::undefined*>(stdlib.find(var_name{"$tests"s, "undefined"s}).get()));
	BOOST_CHECK(dynamic_cast<builtins_tests::sameas*>(stdlib.find(var_name{"$tests"s, "sameas"s}).get()));
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
	cppjinja::evt::context_objects::join j;
	auto sep = std::make_shared<cppjinja::evt::context_objects::value>(","s,1);
	auto o1 = std::make_shared<cppjinja::evt::context_objects::value>("1"s,1);
	auto o2 = std::make_shared<cppjinja::evt::context_objects::value>("2"s,1);
	std::vector<function_parameter> params = {
	     function_parameter{"$"s, sep}
	    ,function_parameter{std::nullopt, o1}
	};

	auto c1 = j.call(params);
	BOOST_TEST(c1->jval() == "1");

	params.push_back(function_parameter{std::nullopt, o2});
	auto c2 = j.call(params);
	BOOST_TEST(c2->jval() == "1,2");
}
BOOST_AUTO_TEST_CASE(lambda)
{
	using cppjinja::evt::context_objects::lambda_function;
	auto obj = std::make_shared<mocks::context_object>();
	lambda_function::fnc_type empty_fnc;
	BOOST_TEST((lambda_function(empty_fnc)).call({})->jval() == nullptr);
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
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca}})->jval() == false);
	auto caobj = std::make_shared<cppjinja::evt::context_objects::callable_node>(&env, &canode);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, caobj}})->jval() == true);
}
BOOST_FIXTURE_TEST_CASE(defined, mock_exenv_fixture)
{
	cppjinja::evt::context_objects::builtins_tests::defined obj;
	auto nca = std::make_shared<mocks::context_object>();
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca}})->jval() == true);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nullptr}})->jval() == false);
}
BOOST_FIXTURE_TEST_CASE(undefined, mock_exenv_fixture)
{
	cppjinja::evt::context_objects::builtins_tests::undefined obj;
	auto nca = std::make_shared<mocks::context_object>();
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca}})->jval() == false);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nullptr}})->jval() == true);
}
BOOST_FIXTURE_TEST_CASE(sameas, mock_exenv_fixture)
{
	cppjinja::evt::context_objects::builtins_tests::sameas obj;
	auto nca1 = std::make_shared<mocks::context_object>();
	auto nca2 = std::make_shared<mocks::context_object>();
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca1}, fnc_param{std::nullopt, nca2}})->jval() == false);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca1}, fnc_param{std::nullopt, nca1}})->jval() == true);
}
BOOST_FIXTURE_TEST_CASE(upper, mock_exenv_fixture)
{
	cppjinja::evt::context_objects::builtins_tests::upper obj;
	auto nca_up = std::make_shared<mocks::context_object>();
	auto nca_low = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(nca_up->jval).returns("UP"s);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca_up}})->jval() == true);
	MOCK_EXPECT(nca_low->jval).returns("low"s);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca_low}})->jval() == false);
}
BOOST_FIXTURE_TEST_CASE(lower, mock_exenv_fixture)
{
	cppjinja::evt::context_objects::builtins_tests::lower obj;
	auto nca_up = std::make_shared<mocks::context_object>();
	auto nca_low = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(nca_up->jval).returns("UP"s);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca_up}})->jval() == false);
	MOCK_EXPECT(nca_low->jval).returns("low"s);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca_low}})->jval() == true);
}
BOOST_FIXTURE_TEST_CASE(iterable, mock_exenv_fixture)
{
	using cppjinja::evt::context_objects::tree;
	cppjinja::evt::context_objects::builtins_tests::iterable obj;
	auto nca_ar = std::make_shared<mocks::context_object>();
	auto nca_obj = std::make_shared<mocks::context_object>();
	auto nca_str = std::make_shared<mocks::context_object>();
	MOCK_EXPECT(nca_ar->jval).returns(cppjinja::json::array());
	MOCK_EXPECT(nca_obj->jval).returns(cppjinja::json::object());
	MOCK_EXPECT(nca_str->jval).returns("test"s);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, std::make_shared<tree>()}})->jval() == true);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca_ar}})->jval() == true);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca_obj}})->jval() == true);
	BOOST_TEST(obj.call({fnc_param{std::nullopt, nca_str}})->jval() == false);
}
BOOST_AUTO_TEST_SUITE_END() // tests
BOOST_AUTO_TEST_SUITE_END() // builtins
BOOST_AUTO_TEST_SUITE_END() // context_object
BOOST_AUTO_TEST_SUITE_END() // phase_evaluate
