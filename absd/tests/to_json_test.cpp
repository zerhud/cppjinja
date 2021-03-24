/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of abstract_data.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE absd_to_json

#include <variant>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "mocks.hpp"

BOOST_AUTO_TEST_SUITE(absd)
using absd_mocks::fixture;
using abs_data = absd::data;
std::string prt(const data& d)
{
	std::stringstream out;
	out << d;
	return out.str();
}
BOOST_AUTO_TEST_SUITE(printing)
BOOST_FIXTURE_TEST_CASE(pod_int, fixture)
{
	expect_reflection(absd::data_type::integer);
	MOCK_EXPECT(prov->to_int).once().returns(42);
	abs_data d(prov);
	BOOST_TEST(prt(d) == "42");
	BOOST_TEST(prt(d) == "42");
}
BOOST_FIXTURE_TEST_CASE(pod_double, fixture)
{
	expect_reflection(absd::data_type::floating_point);
	MOCK_EXPECT(prov->to_double).once().returns(3.14);
	BOOST_TEST(prt(abs_data(prov)) == "3.14");
}
BOOST_FIXTURE_TEST_CASE(pod_string, fixture)
{
	expect_reflection(absd::data_type::string);
	MOCK_EXPECT(prov->to_string).once().returns("foo\"");
	abs_data d(prov);
	BOOST_TEST(prt(d) == "\"foo\\\"\"");
	BOOST_TEST(prt(d) == "\"foo\\\"\"");
}
BOOST_FIXTURE_TEST_CASE(pod_boolean, fixture)
{
	expect_reflection(absd::data_type::boolean);
	MOCK_EXPECT(prov->to_bool).once().returns(true);
	BOOST_TEST(prt(abs_data(prov)) == "true");
	MOCK_EXPECT(prov->to_bool).once().returns(false);
	abs_data d(prov);
	BOOST_TEST(prt(d) == "false");
	BOOST_TEST(prt(d) == "false");
}
BOOST_FIXTURE_TEST_CASE(object_empty, fixture)
{
	expect_reflection(absd::data_type::object, {});
	abs_data d(prov);
	BOOST_TEST(prt(d) == "{}");
	BOOST_TEST(prt(d) == "{}");
}
BOOST_FIXTURE_TEST_CASE(object_single, fixture)
{
	auto src_42 = std::make_shared<absd_mocks::data_holder>();
	expect_reflection(absd::data_type::object, {"a"});
	expect_reflection(absd::data_type::integer, {}, {}, src_42.get());
	MOCK_EXPECT(src_42->to_int).returns(42);
	MOCK_EXPECT(prov->by_key).with("a").calls([&src_42](auto){return src_42;});
	BOOST_TEST(prt(abs_data(prov)) == "{\"a\":42}");
}
BOOST_FIXTURE_TEST_CASE(object_two, fixture)
{
	auto src_42 = std::make_shared<absd_mocks::data_holder>();
	auto src_str = std::make_shared<absd_mocks::data_holder>();
	expect_reflection(absd::data_type::object, {"a","b"});
	expect_reflection(absd::data_type::integer, {}, {}, src_42.get());
	expect_reflection(absd::data_type::string, {}, {}, src_str.get());
	MOCK_EXPECT(src_42->to_int).returns(42);
	MOCK_EXPECT(src_str->to_string).returns("foo");
	MOCK_EXPECT(prov->by_key).with("a").calls([&src_42](auto){return src_42;});
	MOCK_EXPECT(prov->by_key).with("b").calls([&src_str](auto){return src_str;});
	abs_data d(prov);
	BOOST_TEST(prt(d) == "{\"a\":42,\"b\":\"foo\"}");
	BOOST_TEST(prt(d) == "{\"a\":42,\"b\":\"foo\"}");
}
BOOST_FIXTURE_TEST_CASE(object_three, fixture)
{
	auto src_42 = std::make_shared<absd_mocks::data_holder>();
	auto src_str = std::make_shared<absd_mocks::data_holder>();
	auto src_bool = std::make_shared<absd_mocks::data_holder>();
	expect_reflection(absd::data_type::object, {"a","b","c"});
	expect_reflection(absd::data_type::integer, {}, {}, src_42.get());
	expect_reflection(absd::data_type::string, {}, {}, src_str.get());
	expect_reflection(absd::data_type::boolean, {}, {}, src_bool.get());
	MOCK_EXPECT(src_42->to_int).returns(42);
	MOCK_EXPECT(src_str->to_string).returns("foo");
	MOCK_EXPECT(src_bool->to_bool).returns(true);
	MOCK_EXPECT(prov->by_key).with("a").calls([&src_42](auto){return src_42;});
	MOCK_EXPECT(prov->by_key).with("b").calls([&src_str](auto){return src_str;});
	MOCK_EXPECT(prov->by_key).with("c").calls([&src_bool](auto){return src_bool;});
	abs_data d(prov);
	BOOST_TEST(prt(d) == "{\"a\":42,\"b\":\"foo\",\"c\":true}");
	BOOST_TEST(prt(d) == "{\"a\":42,\"b\":\"foo\",\"c\":true}");
}
BOOST_FIXTURE_TEST_CASE(array_three, fixture)
{
	auto src_42 = std::make_shared<absd_mocks::data_holder>();
	auto src_str = std::make_shared<absd_mocks::data_holder>();
	auto src_bool = std::make_shared<absd_mocks::data_holder>();
	expect_reflection(absd::data_type::array, {}, 3);
	expect_reflection(absd::data_type::integer, {}, std::nullopt, src_42.get());
	expect_reflection(absd::data_type::string, {}, std::nullopt, src_str.get());
	expect_reflection(absd::data_type::boolean, {}, std::nullopt, src_bool.get());
	MOCK_EXPECT(src_42->to_int).returns(42);
	MOCK_EXPECT(src_str->to_string).returns("foo");
	MOCK_EXPECT(src_bool->to_bool).returns(true);
	MOCK_EXPECT(prov->by_ind).with(0).calls([&src_42](auto){return src_42;});
	MOCK_EXPECT(prov->by_ind).with(1).calls([&src_str](auto){return src_str;});
	MOCK_EXPECT(prov->by_ind).with(2).calls([&src_bool](auto){return src_bool;});
	abs_data d(prov);
	BOOST_TEST(prt(d) == "[42,\"foo\",true]");
	BOOST_TEST(prt(d) == "[42,\"foo\",true]");
}
BOOST_FIXTURE_TEST_CASE(array_two, fixture)
{
	auto src_42 = std::make_shared<absd_mocks::data_holder>();
	auto src_str = std::make_shared<absd_mocks::data_holder>();
	expect_reflection(absd::data_type::array, {}, 2);
	expect_reflection(absd::data_type::integer, {}, std::nullopt, src_42.get());
	expect_reflection(absd::data_type::string, {}, std::nullopt, src_str.get());
	MOCK_EXPECT(src_42->to_int).returns(42);
	MOCK_EXPECT(src_str->to_string).returns("foo");
	MOCK_EXPECT(prov->by_ind).with(0).calls([&src_42](auto){return src_42;});
	MOCK_EXPECT(prov->by_ind).with(1).calls([&src_str](auto){return src_str;});
	abs_data d(prov);
	BOOST_TEST(prt(d) == "[42,\"foo\"]");
	BOOST_TEST(prt(d) == "[42,\"foo\"]");
}
BOOST_FIXTURE_TEST_CASE(array_empty, fixture)
{
	expect_reflection(absd::data_type::array, {}, 0);
	abs_data d(prov);
	BOOST_TEST(prt(d) == "[]");
	BOOST_TEST(prt(d) == "[]");
}
BOOST_AUTO_TEST_SUITE_END() // printing
BOOST_AUTO_TEST_SUITE_END() // absd
