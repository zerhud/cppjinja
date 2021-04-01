/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <type_traits>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include "common.hpp"

namespace cppjinja::text {

	namespace x3 = boost::spirit::x3;


	template<typename EnvTag, typename Parser, typename Env>
	auto make_grammar(const Parser& parser, const Env& env)
	{
		return boost::spirit::x3::with<
		        EnvTag,std::reference_wrapper<const Env>
		        >(std::cref(env))[parser];
	}

	using iterator_type = boost::spirit::istream_iterator;
	using context_type = x3::context<
	    parser_env_tag,std::reference_wrapper<const parser_env>,
	    x3::phrase_parse_context<decltype(x3::space)>::type
	>;

	template<typename Id, typename Attribute, typename Iterator>
	Attribute parse(
	        boost::spirit::x3::rule<Id, Attribute> rule,
	        Iterator begin, Iterator end,
	        const parser_env* env=parser_env::default_env())
	{
		Attribute result;
		bool success = boost::spirit::x3::phrase_parse(
		            begin, end,
		            make_grammar<parser_env_tag>(rule, *env),
		            boost::spirit::x3::space,
		            result);

		if(!success) throw std::runtime_error("cannot parse");

		return result;
	}

	template<typename Id, typename Attribute>
	Attribute parse(
	        boost::spirit::x3::rule<Id, Attribute> rule,
	        std::string_view data,
	        const parser_env* env=parser_env::default_env())
	{
		//auto end = boost::u8_to_u32_iterator(data.end());
		//auto begin = boost::u8_to_u32_iterator(data.begin());
		std::stringstream data_stream;
		data_stream << data << std::noskipws;
		boost::spirit::istream_iterator begin(data_stream), end;
		return parse(std::move(rule), begin, end, env);
	}

} // namespace cppjinja::text
