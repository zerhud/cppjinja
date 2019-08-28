/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <type_traits>

#include "grammar.hpp"
#include "traits.hpp"
#include "ast.hpp"

namespace x3 = boost::spirit::x3;

namespace cppjinja {

struct grammar_result_tag {};

namespace grammar::utf8 {
using gram_traits = traits::unicode_utf8;
#include "grammar.ipp"
} // namespace grammar::utf8
namespace grammar::wide {
using gram_traits = traits::unicode_wide;
#include "grammar.ipp"
} // namespace grammar::wide

template<typename Parser, typename Env>
auto make_grammar(const Parser& parser, Env&& env)
{
	return x3::with<Env,Env>(std::move(env))[parser];
}

} // namespace cppjinja

cppjinja::b_block<std::string> cppjinja::parse(std::string_view data, parser_data env)
{
	using block_t = grammar::utf8::gram_traits::types::block_t;
	block_t result;
	auto result_ref = std::ref(result);
	bool ok = x3::parse(
	              boost::u8_to_u32_iterator(data.begin()),
	              boost::u8_to_u32_iterator(data.end()),
	              make_grammar(grammar::utf8::block, std::move(env)),
		      result_ref
	              );
	if(!ok) throw std::runtime_error("cannot parse");
	return result;
}

cppjinja::b_block<std::string> cppjinja::parse(std::wstring_view data, parser_data env)
{
	using block_t = grammar::utf8::gram_traits::types::block_t;
	block_t result;
	auto result_ref = std::ref(result);
	bool ok = x3::parse(
	              data.begin(), data.end(),
	              make_grammar(grammar::utf8::block, std::move(env)),
		      result_ref
	              );
	if(!ok) throw std::runtime_error("cannot parse");
	return result;
}

cppjinja::b_block<std::wstring> cppjinja::wparse(std::wstring_view data, parser_data env)
{

	using block_t = grammar::wide::gram_traits::types::block_t;
	block_t result;
	return result;
}
