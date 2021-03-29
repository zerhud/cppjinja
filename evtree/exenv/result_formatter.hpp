/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <string>

namespace cppjinja::evt {

class result_formatter final {
	int bval=0;
public:
	void shift_tab(int val);
	std::pmr::string operator()(std::pmr::string src) const ;
};

} // namespace cppjinja::evt
