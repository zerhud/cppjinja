/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast.hpp"

namespace cppjinja {

b_block<std::string> parse(std::string_view data);
b_block<std::wstring> parse(std::wstring_view data);

} // namespace cppjinja
