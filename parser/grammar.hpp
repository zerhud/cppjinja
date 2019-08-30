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

struct parser_data {
	struct {
		std::string b= "<=";
		std::string e= "=>";
	} output;
	struct {
		std::string b= "<%";
		std::string e= "%>";
	} term;
};

b_block<std::string> parse(std::string_view data, parser_data env={});
b_block<std::string> parse(std::wstring_view data, parser_data env={});
b_block<std::wstring> wparse(std::wstring_view data, parser_data env={});

} // namespace cppjinja
