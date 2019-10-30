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
#include "debug_operators.hpp"

#define FILE_INLINING

namespace x3 = boost::spirit::x3;

namespace cppjinja {

namespace grammar::utf8 {
using gram_traits = traits::unicode_utf8;
#include "grammar.inl"
} // namespace grammar::utf8
namespace grammar::wide {
using gram_traits = traits::unicode_wide;
#include "grammar.inl"
} // namespace grammar::wide

template<typename Parser, typename Env>
auto make_grammar(const Parser& parser, Env&& env)
{
	return x3::with<Env,Env>(std::move(env))[parser];
}

template<typename Iter>
struct iter_timpl { 
	using iterator_category	= std::bidirectional_iterator_tag;
	using value_type = typename Iter::value_type;
	using difference_type = std::ptrdiff_t;
	using pointer = typename Iter::value_type*;
	using reference = typename Iter::value_type&;

	Iter iter;

	void applay() const {}

	typename Iter::value_type operator * () noexcept { return *iter; }
	typename Iter::value_type operator * () const noexcept { return *iter; }

	iter_timpl& operator ++ () noexcept { ++iter; return *this; }
	iter_timpl& operator -- () noexcept { ++iter; return *this; }
	iter_timpl& operator ++ (int) noexcept { iter++; return *this; }
	iter_timpl& operator -- (int) noexcept { iter++; return *this; }
};

template<typename Iter>
bool operator == (const iter_timpl<Iter>& left, const iter_timpl<Iter>& right) { return left.iter == right.iter; }
template<typename Iter>
bool operator != (const iter_timpl<Iter>& left, const iter_timpl<Iter>& right) { return left.iter != right.iter; }

/// cannot instaitiate x3::parse with template
/// calss as Iterator tempalte's parameter.
struct iter_simpl : iter_timpl<iter_stype> {};
struct iter_wimpl : iter_timpl<iter_wtype> {};

} // namespace cppjinja

cppjinja::s_block cppjinja::parsers(iter_stype begin, iter_stype end, parser_data env)
{
	s_block result;
	auto result_ref = std::ref(result);
	bool ok = x3::parse(
	              boost::u8_to_u32_iterator(iter_simpl{std::move(begin)}),
		      boost::u8_to_u32_iterator(iter_simpl{std::move(end)}),

	              make_grammar(grammar::utf8::block, std::move(env)),
	              result_ref
	              );
	if(!ok) throw std::runtime_error("cannot parse");
	return result;
}

cppjinja::s_block cppjinja::parserw(iter_wtype begin, iter_wtype end, parser_data env)
{
	s_block result;
	auto result_ref = std::ref(result);
	bool ok = x3::parse(
	              iter_wimpl{std::move(begin)}, iter_wimpl{std::move(end)},
	              make_grammar(grammar::utf8::block, std::move(env)),
	              result_ref
	              );
	if(!ok) throw std::runtime_error("cannot parse");
	return result;
}

cppjinja::s_block cppjinja::parse(std::string_view data, parser_data env)
{
	s_block result;
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

cppjinja::s_block cppjinja::parse(std::wstring_view data, parser_data env)
{
	s_block result;
	auto result_ref = std::ref(result);
	bool ok = x3::parse(
	              data.begin(), data.end(),
	              make_grammar(grammar::utf8::block, std::move(env)),
	              result_ref
	              );
	if(!ok) throw std::runtime_error("cannot parse");
	return result;
}

cppjinja::w_block cppjinja::wparse(std::wstring_view data, parser_data env)
{

	w_block result;
	auto result_ref = std::ref(result);
	bool ok = x3::parse(
	              data.begin(), data.end(),
	              make_grammar(grammar::wide::block, std::move(env)),
	              result_ref
	              );
	if(!ok) throw std::runtime_error("cannot parse");
	return result;
}
