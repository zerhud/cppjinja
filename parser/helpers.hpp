/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <ostream>
#include <type_traits>
#include <boost/tti/has_member_data.hpp>
#include "ast/blocks.hpp"

#define DEFINE_MEMBER_CHECKER(member, prefix, fieldtype) \
	template<typename T, typename V = bool> \
	struct has_ ## prefix ## _ ## member : std::false_type { }; \
	template<typename T> \
	struct has_ ## prefix ## _ ## member<T, \
	    typename std::enable_if_t< \
	        std::is_same_v<decltype(std::declval<T>().member), fieldtype>, bool> \
	    > : std::true_type { }; \
	template< typename T > \
	inline constexpr bool has_ ## prefix ## _ ## member ## _v = has_ ## prefix ## _ ## member<T>::value;

#define TRYCON(a, b, c) extern int kuku ## a ## b ## c ## _post;

namespace cppjinja {

DEFINE_MEMBER_CHECKER(name, str, std::string)
DEFINE_MEMBER_CHECKER(content, blockcontent, std::vector<ast::block_content>)
BOOST_TTI_HAS_MEMBER_DATA(content)


template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

} // namespace cppjinja

