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
#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>
#include "common.hpp"

namespace cppjinja::text {

	namespace x3 = boost::spirit::x3;

	using iterator_type = boost::spirit::istream_iterator;
	using error_handler_type = x3::error_handler<iterator_type>;
	using context_type = x3::context<
	    x3::skipper_tag, x3::unused_skipper<decltype(x3::space)>,
	    x3::context<
	         parser_env_tag,std::reference_wrapper<const parser_env>,
	         x3::context<
	           x3::error_handler_tag, std::reference_wrapper<error_handler_type>,
	           x3::phrase_parse_context<decltype(x3::space)>::type
	         >
	    >
	>;

	using context_type_skipper =
	    x3::context<
	         parser_env_tag,std::reference_wrapper<const parser_env>,
	         x3::context<
	           x3::error_handler_tag, std::reference_wrapper<error_handler_type>,
	           x3::phrase_parse_context<decltype(x3::space)>::type
	         >
	>;

	template<typename EnvTag, typename Parser, typename Env, typename ErrHndl>
	auto make_grammar(const Parser& parser, const Env& env, ErrHndl& eh)
	{
		return x3::with<x3::error_handler_tag>(std::ref(eh))
		        [
		            boost::spirit::x3::with<EnvTag>(std::cref(env))
		            [
		                parser
		            ]
		        ];
	}

	template<typename Id, typename Attribute, typename Iterator>
	Attribute parse(
	        boost::spirit::x3::rule<Id, Attribute> rule,
	        Iterator begin, Iterator end,
	        const parser_env* env=parser_env::default_env())
	{
		Attribute result;
		x3::error_handler eh(begin,end, env->elog(), env->file_name());
		bool success = boost::spirit::x3::phrase_parse(
		            begin, end,
		            make_grammar<parser_env_tag>(rule, *env, eh),
		            boost::spirit::x3::space,
		            result);

		if(!success) env->on_error();

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
