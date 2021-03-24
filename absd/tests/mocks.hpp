/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of abstract_data.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <turtle/mock.hpp>
#include "abstract_data.hpp"

namespace absd_mocks {

MOCK_BASE_CLASS(data_holder, absd::data_holder)
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
	absd::data dp{prov};
};

} // namespace absd_mocks
