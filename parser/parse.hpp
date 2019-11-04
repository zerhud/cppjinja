/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <type_traits>
#include "common.hpp"

namespace cppjinja::text {

    namespace x3 = boost::spirit::x3;


	template<typename Parser, typename Env>
	auto make_grammar(const Parser& parser, Env&& env)
	{
		return boost::spirit::x3::with<Env,Env>(std::move(env))[parser];
	}

	using iterator_type = std::string_view::const_iterator;
	//using context_type = x3::phrase_parse_context<decltype(x3::space)>::type;
	using context_type = x3::context<parser_env,parser_env,x3::phrase_parse_context<decltype(x3::space)>::type>;

	template<typename Id, typename Attribute>
	Attribute parse(boost::spirit::x3::rule<Id, Attribute> rule, std::string_view data, parser_env&& env=parser_env{})
	{
		//auto end = boost::u8_to_u32_iterator(data.end());
		//auto begin = boost::u8_to_u32_iterator(data.begin());
		auto end = data.end();
		auto begin = data.begin();

		Attribute result;
		bool success = boost::spirit::x3::phrase_parse(begin, end, make_grammar(rule, std::move(env)), boost::spirit::x3::space, result);

		if(!success) throw std::runtime_error("cannot parse");

		return result;
	}

} // namespace cppjinja::text
