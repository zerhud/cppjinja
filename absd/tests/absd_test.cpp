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
#include <turtle/mock.hpp>

#include "abstract_data.hpp"

namespace abstract_data_mocks {

MOCK_BASE_CLASS(data_holder, abstract_data::data_holder)
{
	MOCK_METHOD(to_int, 0)
	MOCK_METHOD(to_double, 0)
	MOCK_METHOD(to_string, 0)
	MOCK_METHOD(to_bool, 0)
	MOCK_METHOD(by_key, 1)
	MOCK_METHOD(by_ind, 1)
};

struct fixture {
	std::shared_ptr<data_holder> prov = std::make_shared<data_holder>();
};

} // namespace abstract_data_mocks

BOOST_AUTO_TEST_SUITE(absd)
using abstract_data_mocks::fixture;
using abs_data = abstract_data::data;
BOOST_FIXTURE_TEST_CASE(to_int, fixture)
{
	abs_data dp(prov);
	MOCK_EXPECT(prov->to_int).once().returns(42);
	BOOST_TEST((std::int64_t)dp == 42);
	BOOST_TEST((std::int64_t)dp == 42, "result cached");
	BOOST_CHECK_THROW((bool)dp, std::bad_variant_access);
}
BOOST_FIXTURE_TEST_CASE(to_double, fixture)
{
	abs_data dp(prov);
	MOCK_EXPECT(prov->to_double).once().returns(3.14);
	BOOST_TEST((double)dp == 3.14);
	BOOST_TEST((double)dp == 3.14, "result cached");
	BOOST_CHECK_THROW((std::int64_t)dp, std::bad_variant_access);
}
BOOST_FIXTURE_TEST_CASE(to_string, fixture)
{
	abs_data dp(prov);
	MOCK_EXPECT(prov->to_string).once().returns("test");
	BOOST_TEST((std::pmr::string)dp == "test");
	BOOST_TEST((std::pmr::string)dp == "test", "result cached");
	BOOST_TEST((std::pmr::string)dp == dp.str());
	BOOST_CHECK_THROW((std::int64_t)dp, std::bad_variant_access);
}
BOOST_FIXTURE_TEST_CASE(to_bool, fixture)
{
	abs_data dp(prov);
	MOCK_EXPECT(prov->to_bool).once().returns(true);
	BOOST_TEST((bool)dp == true);
	BOOST_TEST((bool)dp == true, "result cached");
	BOOST_CHECK_THROW((std::int64_t)dp, std::bad_variant_access);
}
BOOST_FIXTURE_TEST_CASE(as_object, fixture)
{
	abs_data dp(prov);
	auto val = std::make_unique<abstract_data_mocks::data_holder>();
	MOCK_EXPECT(val->to_int).once().returns(42);
	MOCK_EXPECT(prov->by_key)
	        .once().with("key")
	        .calls([&val](auto){return std::move(val);});

	const abs_data& dval = dp["key"];
	BOOST_TEST((std::int64_t)dval == 42);
	BOOST_TEST(&dp["key"] == &dval);
}
BOOST_FIXTURE_TEST_CASE(as_array, fixture)
{
	abs_data dp(prov);
	auto val = std::make_unique<abstract_data_mocks::data_holder>();
	MOCK_EXPECT(val->to_int).once().returns(42);
	MOCK_EXPECT(prov->by_ind)
	        .once().with(13)
	        .calls([&val](auto){return std::move(val);});

	const abs_data& dval = dp[13];
	BOOST_TEST((std::int64_t)dval == 42);
	BOOST_TEST(&dp[13] == &dval);
}
BOOST_AUTO_TEST_SUITE_END() // absd
