/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <functional>

namespace cppjinja::evtnodes {

template<
	  typename E
	, typename Body
	, template<typename...> typename List
	, typename ... LArgs
	> // requires(List<E, LArgs...>{}[0])
std::enable_if_t<std::is_pointer_v<E>>
loop_by_three(const List<E, LArgs...>& list, const Body& body)
{
	for(std::size_t i=0;i<list.size();++i)
	{
		E prev = i==0?nullptr:list[i-1];
		E cur = list[i];
		E next = i+1==list.size()?nullptr:list[i+1];
		body(prev, cur, next);
	}
}

template<
	  typename E
	, typename Body
	, template<typename...> typename List
	, typename ... LArgs
	> // requires(List<E, LArgs...>{}[0])
std::enable_if_t<!std::is_pointer_v<E>>
loop_by_three(const List<E, LArgs...>& list, const Body& body)
{
	for(std::size_t i=0;i<list.size();++i)
	{
		E* prev = i==0?nullptr:&list[i-1];
		E* cur = &list[i];
		E* next = i+1==list.size()?nullptr:&list[i+1];
		body(prev, cur, next);
	}
}

} // namespace cppjinja::evtnodes
