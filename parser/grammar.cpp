/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "grammar.hpp"
#include "traits.hpp"
#include "ast.hpp"

namespace x3 = boost::spirit::x3;

namespace cppjinja {

namespace grammar::utf8 {
using gram_traits = traits::unicode_utf8;
#include "grammar.ipp"
} // namespace grammar::utf8

} // namespace cppjinja

cppjinja::b_block<std::string> cppjinja::parse(std::string_view data)
{
	grammar::utf8::gram_traits::types::block_t result;
	bool ok = x3::parse(
	              boost::u8_to_u32_iterator(data.begin()),
	              boost::u8_to_u32_iterator(data.end()),
	              grammar::utf8::block,
	              result
	              );
	if(!ok) throw std::runtime_error("cannot parse");
	return result;
}

cppjinja::b_block<std::string> cppjinja::parse(std::wstring_view data)
{
	grammar::utf8::gram_traits::types::block_t result;
	bool ok = x3::parse(
	              data.begin(), data.end(),
	              grammar::utf8::block,
	              result
	              );
	if(!ok) throw std::runtime_error("cannot parse");
	return result;
}

