/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3.hpp>
#include "fusion/blocks.hpp"

namespace cppjinja::text {
	namespace x3 = boost::spirit::x3;

	struct block_if_class;
	struct block_raw_class;
	struct block_for_class;
	struct block_set_class;
	struct block_call_class;
	struct else_thread_class;
	struct block_macro_class;
	struct block_named_class;
	struct block_content_class;
	struct block_filtered_class;
	struct block_raw_text_class;
	struct call_parameter_class;
	struct block_free_text_class;
	struct macro_parameter_class;
	struct block_content_vec_class;

	const x3::rule<block_free_text_class, std::string> block_free_text = "block_free_text";

	const x3::rule<block_content_class, ast::block_content> block_content = "block_content";
	const x3::rule<block_content_vec_class, std::vector<ast::block_content>> block_content_vec = "block_content_vec";

	const x3::rule<block_raw_text_class, std::string> block_raw_text = "block_raw_text";
	const x3::rule<block_raw_class, ast::block_raw> block_raw = "block_raw";

	const x3::rule<block_if_class,  ast::block_if>  block_if  = "block_if";
	const x3::rule<block_for_class, ast::block_for> block_for = "block_for";
	const x3::rule<block_set_class, ast::block_set> block_set = "block_set";
	const x3::rule<else_thread_class, ast::else_thread>  else_thread  = "else_thread";

	const x3::rule<block_call_class, ast::block_call> block_call = "block_call";
	const x3::rule<block_macro_class, ast::block_macro> block_macro = "block_macro";
	const x3::rule<block_named_class, ast::block_named> block_named = "block_named";
	const x3::rule<block_filtered_class, ast::block_filtered> block_filtered = "block_filtered";
	const x3::rule<call_parameter_class, ast::macro_parameter> call_parameter = "call_parameter";
	const x3::rule<macro_parameter_class, ast::macro_parameter> macro_parameter = "macro_parameter";

	BOOST_SPIRIT_DECLARE(decltype(block_if))
	BOOST_SPIRIT_DECLARE(decltype(block_for))
	BOOST_SPIRIT_DECLARE(decltype(block_raw))
	BOOST_SPIRIT_DECLARE(decltype(block_set))
	BOOST_SPIRIT_DECLARE(decltype(block_call))
	BOOST_SPIRIT_DECLARE(decltype(else_thread))
	BOOST_SPIRIT_DECLARE(decltype(block_named))
	BOOST_SPIRIT_DECLARE(decltype(block_macro))
	BOOST_SPIRIT_DECLARE(decltype(block_content))
	BOOST_SPIRIT_DECLARE(decltype(block_filtered))
	BOOST_SPIRIT_DECLARE(decltype(block_raw_text))
	BOOST_SPIRIT_DECLARE(decltype(call_parameter))
	BOOST_SPIRIT_DECLARE(decltype(block_free_text))
	BOOST_SPIRIT_DECLARE(decltype(macro_parameter))
	BOOST_SPIRIT_DECLARE(decltype(block_content_vec))

} // namespace cppjinja::text

