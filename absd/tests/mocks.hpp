/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of abstract_data.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <turtle/mock.hpp>
#include "absd/abstract_data.hpp"

namespace absd_mocks {

MOCK_BASE_CLASS(data_holder, absd::data_holder)
{
	MOCK_METHOD(storage, 0)
	MOCK_METHOD(reflect, 0)
	MOCK_METHOD(to_int, 0)
	MOCK_METHOD(to_double, 0)
	MOCK_METHOD(to_string, 0)
	MOCK_METHOD(to_bool, 0)
	MOCK_METHOD(by_key, 1)
	MOCK_METHOD(by_ind, 1)
};

struct fixture {
	std::shared_ptr<data_holder> prov;
	std::optional<absd::data> _dp;

	std::shared_ptr<data_holder> prov2;
	std::optional<absd::data> _dp2;

	absd::data& dp() { assert(!!_dp); return *_dp; }
	absd::data& dp2() { assert(!!_dp2); return *_dp2; }

	fixture()
	    : prov(make_holder())
	    , prov2(make_holder())
	{
		_dp.emplace(prov);
		_dp2.emplace(prov2);
	}

	std::shared_ptr<data_holder> make_holder()
	{
		auto ret = std::make_shared<data_holder>();
		MOCK_EXPECT(ret->storage).returns(std::pmr::new_delete_resource());
		return ret;
	}

	void expect_reflection(
	        absd::data_type t,
	        std::pmr::vector<std::string_view> k={},
	        std::optional<std::uint64_t> i=std::nullopt,
	        data_holder* p = nullptr)
	{
		data_holder* s = p ? p : prov.get();
		MOCK_EXPECT(s->reflect).returns(
		            absd::reflection_info{
		                .type=t,
		                .size = i ? *i : 0,
		                .keys=std::move(k) });
	}

	void expect_reflection2(
	        absd::data_type t,
	        std::pmr::vector<std::string_view> k={},
	        std::optional<std::uint64_t> i=std::nullopt)
	{
		expect_reflection(t,std::move(k),i,prov2.get());
	}
};

} // namespace absd_mocks
