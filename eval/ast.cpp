/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

// need to include some file for work with streams
// if not crashes when print double to std::ostream&
#include <iomanip>
#include "ast.hpp"

bool cppjinja::east::operator ==(
          const cppjinja::east::function_parameter& left
        , const cppjinja::east::function_parameter& right)
{
	return left.name == right.name && left.val == right.val;
}

std::ostream& cppjinja::east::operator <<(std::ostream& out, const cppjinja::east::function_parameter& val)
{
	using namespace std::literals;
	out << (val.name.has_value() ? *val.name : "[]"s);
	if(val.val.has_value()) return out << '=' << *val.val;
	return out << '=' << "[]"sv;
}
