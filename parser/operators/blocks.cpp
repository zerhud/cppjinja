/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "blocks.hpp"

std::ostream& std::operator << (std::ostream& out, const cppjinja::ast::block_raw& obj)
{
	return out << "raw: " << obj.value;
}


