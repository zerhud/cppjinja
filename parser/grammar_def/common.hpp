/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>
#include <boost/spirit/home/support/utf8.hpp>
#include "grammar/opterm.hpp"
#include "../common.hpp"

namespace cppjinja::text {

struct error_handler
{
	template <typename Iterator, typename Exception, typename Context>
	x3::error_handler_result on_error(
	        Iterator&, Iterator const&
	      , Exception const& x, Context const& context)
	{
		auto& error_handler = x3::get<x3::error_handler_tag>(context).get();
		const parser_env& env = x3::get<parser_env_tag>(context).get();
		error_handler(x.where(), env.format_err_msg(x.which()));
		env.on_error();
		return x3::error_handler_result::fail;
	}

	// bug workaround: sizeof of Id incomplete type (x3::on_success class),
	// just copy pase it's code here
	template <typename Iterator, typename Context, typename... Types>
	inline void on_success(Iterator const& first, Iterator const& last
	  , x3::variant<Types...>& ast, Context const& context)
	{
		ast.apply_visitor(x3::make_lambda_visitor<void>([&](auto& node)
		{
		this->on_success(first, last, node, context);
		}));
	}

	template <typename T, typename Iterator, typename Context>
	inline void on_success(Iterator const& first, Iterator const& last
	  , T& ast, Context const& context)
	{
		auto& error_handler = x3::get<x3::error_handler_tag>(context).get();
		error_handler.tag(ast, first, last);
	}
};

    //auto& char_ = x3::unicode::char_;
    //auto& space = x3::unicode::space;
    //using boost::spirit::x3::standard_wide::lit;


	extern decltype(x3::char_)& char_;
	extern decltype(x3::space)& space;
	using boost::spirit::x3::lit;
	using boost::spirit::x3::skip;
	using boost::spirit::x3::omit;
	using boost::spirit::x3::lexeme;

} // namespace cppjinja::text
