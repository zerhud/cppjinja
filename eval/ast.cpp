/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "ast.hpp"

cppjinja::east::array_v::array_v(const cppjinja::east::array_v& other)
{
	for(auto& i:other.items)
		items.emplace_back(std::make_unique<value_term>(*i));
}

cppjinja::east::array_v& cppjinja::east::array_v::operator =(
        const cppjinja::east::array_v& other)
{
	items.clear();
	for(auto& i:other.items)
		items.emplace_back(std::make_unique<value_term>(*i));
	return *this;
}

cppjinja::east::map_v::map_v(const cppjinja::east::map_v& other)
{
	for(auto&&[k,v]:other.items)
		items[k] = std::make_unique<value_term>(*v);
}

cppjinja::east::map_v& cppjinja::east::map_v::operator =(
        const cppjinja::east::map_v& other)
{
	items.clear();
	for(auto&&[k,v]:other.items)
		items[k] = std::make_unique<value_term>(*v);
	return *this;
}
