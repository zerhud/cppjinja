/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3.hpp>
#include "fusion/single.hpp"

namespace cppjinja::text {
	namespace x3 = boost::spirit::x3;

	class op_out_class;
	class op_comment_class;
	class op_comment_value_class;
	class op_set_class;
	class op_include_class;
	class filter_call_class;
	class filename_class;

	const x3::rule<op_out_class, ast::op_out> op_out = "op_out";
	const x3::rule<op_comment_value_class, ast::string_t> op_comment_value = "op_comment_value";
	const x3::rule<op_comment_class, ast::op_comment> op_comment = "op_comment";
	const x3::rule<op_set_class, ast::op_set> op_set = "op_set";
	const x3::rule<filter_call_class, ast::filter_call> filter_call = "filter_call";
	const x3::rule<op_include_class, ast::op_include> op_include = "op_include";
	const x3::rule<filename_class, ast::string_t> filename = "filename";

	BOOST_SPIRIT_DECLARE(decltype(op_out))
	BOOST_SPIRIT_DECLARE(decltype(op_comment_value))
	BOOST_SPIRIT_DECLARE(decltype(op_comment))
	BOOST_SPIRIT_DECLARE(decltype(op_set))
	BOOST_SPIRIT_DECLARE(decltype(op_include))
	BOOST_SPIRIT_DECLARE(decltype(filter_call))
	BOOST_SPIRIT_DECLARE(decltype(filename))

} // namespace cppjinja::text

