/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "grammar.hpp"


cppjinja::b_block cppjinja::parse(std::string_view data)
{
	b_block ret;
	ret.name = std::string(data);
	return  ret;
}
