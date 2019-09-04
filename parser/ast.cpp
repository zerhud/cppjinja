/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "ast.hpp"

std::ostream& cppjinja::operator << (std::ostream& out, const cppjinja::var_name& obj)
{
	for(auto& i:obj) out << i << '.';
	return out;
}


