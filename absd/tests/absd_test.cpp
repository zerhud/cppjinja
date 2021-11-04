/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of abstract_data.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE abstract_data

#include <variant>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "mocks.hpp"

BOOST_AUTO_TEST_SUITE(absd)
using absd_mocks::fixture;
using abs_data = absd::data;
BOOST_FIXTURE_TEST_CASE(to_int, fixture)
{
	MOCK_EXPECT(prov->to_int).once().returns(42);
	BOOST_TEST((std::int64_t)dp() == 42);
	BOOST_TEST((std::int64_t)dp() == 42, "result cached");
	BOOST_CHECK_THROW((bool)dp(), std::bad_variant_access);
}
BOOST_FIXTURE_TEST_CASE(to_double, fixture)
{
	MOCK_EXPECT(prov->to_double).once().returns(3.14);
	BOOST_TEST((double)dp() == 3.14);
	BOOST_TEST((double)dp() == 3.14, "result cached");
	BOOST_CHECK_THROW((std::int64_t)dp(), std::bad_variant_access);
}
BOOST_FIXTURE_TEST_CASE(to_string, fixture)
{
	MOCK_EXPECT(prov->to_string).once().returns("test");
	BOOST_TEST((std::pmr::string)dp() == "test");
	BOOST_TEST((std::pmr::string)dp() == "test", "result cached");
	BOOST_TEST((std::pmr::string)dp() == dp().str());
	BOOST_CHECK_THROW((std::int64_t)dp(), std::bad_variant_access);
}
BOOST_FIXTURE_TEST_CASE(to_bool, fixture)
{
	MOCK_EXPECT(prov->to_bool).once().returns(true);
	BOOST_TEST((bool)dp() == true);
	BOOST_TEST((bool)dp() == true, "result cached");
	BOOST_CHECK_THROW((std::int64_t)dp(), std::bad_variant_access);
}
BOOST_FIXTURE_TEST_CASE(as_object, fixture)
{
	auto val = make_holder();
	MOCK_EXPECT(val->to_int).once().returns(42);
	MOCK_EXPECT(prov->by_key)
	        .once().with("key")
	        .calls([val](auto){return val;});

	const abs_data& dval = dp()["key"];
	BOOST_TEST((std::int64_t)dval == 42);
	BOOST_TEST(&dp()["key"] == &dval);
}
BOOST_FIXTURE_TEST_CASE(as_object_fnc, fixture)
{
	auto val = make_holder();
	MOCK_EXPECT(val->to_int).returns(42);
	MOCK_EXPECT(prov->by_key)
	        .once().with("key")
	        .calls([val](auto){return val;});

	expect_reflection(absd::data_type::object, {"key"});
	auto map = dp().as_map();
	BOOST_TEST(map.size() == 1);
	for(auto& [k,v]:map) {
		BOOST_TEST(k=="key");
		BOOST_TEST(v == absd::data{val});
	}
}
BOOST_FIXTURE_TEST_CASE(as_array, fixture)
{
	auto val = make_holder();
	MOCK_EXPECT(val->to_int).once().returns(42);
	MOCK_EXPECT(prov->by_ind)
	        .once().with(13)
	        .calls([val](auto){return val;});

	const abs_data& dval = dp()[13];
	BOOST_TEST((std::int64_t)dval == 42);
	BOOST_TEST(&dp()[13] == &dval);
}
BOOST_FIXTURE_TEST_CASE(as_array_fnc, fixture)
{
	auto val = make_holder();
	MOCK_EXPECT(val->to_int).returns(42);
	MOCK_EXPECT(prov->by_ind)
	        .once().with(0)
	        .calls([val](auto){return val;});

	expect_reflection(absd::data_type::array, {}, 1);
	auto ar = dp().as_array();
	BOOST_TEST(ar.size() == 1);
	for(auto& v:ar) BOOST_TEST(v == absd::data{val});
}
BOOST_AUTO_TEST_SUITE(reflect)
BOOST_FIXTURE_TEST_CASE(empty, fixture)
{
	expect_reflection(absd::data_type::empty);
	BOOST_TEST(dp().is_string() == false);
	BOOST_TEST(dp().is_integer() == false);
	BOOST_TEST(dp().is_float() == false);
	BOOST_TEST(dp().is_boolean() == false);
	BOOST_TEST(dp().is_object() == false);
	BOOST_TEST(dp().is_array() == false);
	BOOST_TEST(dp().is_empty() == true);
}
BOOST_FIXTURE_TEST_CASE(string, fixture)
{
	expect_reflection(absd::data_type::string);
	BOOST_TEST(dp().is_string() == true);
	BOOST_TEST(dp().is_integer() == false);
	BOOST_TEST(dp().is_float() == false);
	BOOST_TEST(dp().is_boolean() == false);
	BOOST_TEST(dp().is_object() == false);
	BOOST_TEST(dp().is_array() == false);
	BOOST_TEST(dp().is_empty() == false);
}
BOOST_FIXTURE_TEST_CASE(integer, fixture)
{
	expect_reflection(absd::data_type::integer);
	BOOST_TEST(dp().is_string() == false);
	BOOST_TEST(dp().is_integer() == true);
	BOOST_TEST(dp().is_float() == false);
	BOOST_TEST(dp().is_boolean() == false);
	BOOST_TEST(dp().is_object() == false);
	BOOST_TEST(dp().is_array() == false);
	BOOST_TEST(dp().is_empty() == false);
}
BOOST_FIXTURE_TEST_CASE(floating_point, fixture)
{
	expect_reflection(absd::data_type::floating_point);
	BOOST_TEST(dp().is_string() == false);
	BOOST_TEST(dp().is_integer() == false);
	BOOST_TEST(dp().is_float() == true);
	BOOST_TEST(dp().is_boolean() == false);
	BOOST_TEST(dp().is_object() == false);
	BOOST_TEST(dp().is_array() == false);
	BOOST_TEST(dp().is_empty() == false);
}
BOOST_FIXTURE_TEST_CASE(boolean, fixture)
{
	expect_reflection(absd::data_type::boolean);
	BOOST_TEST(dp().is_string() == false);
	BOOST_TEST(dp().is_integer() == false);
	BOOST_TEST(dp().is_float() == false);
	BOOST_TEST(dp().is_boolean() == true);
	BOOST_TEST(dp().is_object() == false);
	BOOST_TEST(dp().is_array() == false);
	BOOST_TEST(dp().is_empty() == false);
}
BOOST_FIXTURE_TEST_CASE(object, fixture)
{
	expect_reflection(absd::data_type::object);
	BOOST_TEST(dp().is_string() == false);
	BOOST_TEST(dp().is_integer() == false);
	BOOST_TEST(dp().is_float() == false);
	BOOST_TEST(dp().is_boolean() == false);
	BOOST_TEST(dp().is_object() == true);
	BOOST_TEST(dp().is_array() == false);
	BOOST_TEST(dp().is_empty() == false);
}
BOOST_FIXTURE_TEST_CASE(array, fixture)
{
	expect_reflection(absd::data_type::array);
	BOOST_TEST(dp().is_string() == false);
	BOOST_TEST(dp().is_integer() == false);
	BOOST_TEST(dp().is_float() == false);
	BOOST_TEST(dp().is_boolean() == false);
	BOOST_TEST(dp().is_object() == false);
	BOOST_TEST(dp().is_array() == true);
	BOOST_TEST(dp().is_empty() == false);
}
BOOST_AUTO_TEST_SUITE_END() // reflect
BOOST_AUTO_TEST_SUITE(operators)
BOOST_FIXTURE_TEST_CASE(data_eq, fixture)
{
	BOOST_TEST(dp() == absd::data{prov});
}
BOOST_FIXTURE_TEST_CASE(data_eq_str, fixture)
{
	expect_reflection(absd::data_type::string);
	expect_reflection2(absd::data_type::string);
	MOCK_EXPECT(prov->to_string).once().returns("test");
	MOCK_EXPECT(prov2->to_string).once().returns("test");
	BOOST_TEST(dp() == dp2());
	BOOST_TEST(dp() == "test");
}
BOOST_FIXTURE_TEST_CASE(data_eq_int, fixture)
{
	expect_reflection(absd::data_type::integer);
	expect_reflection2(absd::data_type::integer);
	MOCK_EXPECT(prov->to_int).once().returns(42);
	MOCK_EXPECT(prov2->to_int).once().returns(42);
	BOOST_TEST(dp() == dp2());
	BOOST_TEST(dp() == 42);
}
BOOST_FIXTURE_TEST_CASE(data_eq_float, fixture)
{
	expect_reflection(absd::data_type::floating_point);
	expect_reflection2(absd::data_type::floating_point);
	MOCK_EXPECT(prov->to_double).once().returns(3.14);
	MOCK_EXPECT(prov2->to_double).once().returns(3.14);
	BOOST_TEST(dp() == dp2());
	BOOST_TEST(dp() == 3.14);
}
BOOST_FIXTURE_TEST_CASE(data_eq_bool, fixture)
{
	expect_reflection(absd::data_type::boolean);
	expect_reflection2(absd::data_type::boolean);
	MOCK_EXPECT(prov->to_bool).once().returns(true);
	MOCK_EXPECT(prov2->to_bool).once().returns(true);
	BOOST_TEST(dp() == dp2());
	BOOST_TEST(dp() == true);
}
BOOST_FIXTURE_TEST_CASE(data_eq_ar, fixture)
{
	auto val = make_holder();
	expect_reflection(absd::data_type::array, {}, 1);
	expect_reflection2(absd::data_type::array, {}, 1);
	expect_reflection(absd::data_type::integer,{},std::nullopt,val.get());
	MOCK_EXPECT(val->to_int).returns(42);
	MOCK_EXPECT(prov->by_ind).once().with(0)
	        .calls([&val](auto){return val;});
	MOCK_EXPECT(prov2->by_ind).once().with(0)
	        .calls([&val](auto){return val;});

	BOOST_TEST(dp() == dp2());
}
BOOST_FIXTURE_TEST_CASE(data_eq_map, fixture)
{
	auto val = make_holder();
	expect_reflection(absd::data_type::object, {"key"}, std::nullopt);
	expect_reflection2(absd::data_type::object, {"key"}, std::nullopt);
	expect_reflection(absd::data_type::integer,{},std::nullopt,val.get());
	MOCK_EXPECT(val->to_int).returns(42);
	MOCK_EXPECT(prov->by_key).once().with("key")
	        .calls([&val](auto){return val;});
	MOCK_EXPECT(prov2->by_key).once().with("key")
	        .calls([&val](auto){return val;});

	BOOST_TEST(dp() == dp2());
}
BOOST_AUTO_TEST_SUITE_END() // operators
BOOST_AUTO_TEST_SUITE_END() // absd

