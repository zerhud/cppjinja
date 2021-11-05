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

	struct op_out_class;
	struct op_comment_class;
	struct op_comment_value_class;
	struct op_set_class;
	struct op_include_class;
	struct op_import_class;
	struct filename_class;

	const x3::rule<op_out_class, ast::op_out> op_out = "op_out";
	const x3::rule<op_comment_value_class, ast::string_t> op_comment_value = "op_comment_value";
	const x3::rule<op_comment_class, ast::op_comment> op_comment = "op_comment";
	const x3::rule<op_set_class, ast::op_set> op_set = "op_set";
	const x3::rule<op_include_class, ast::op_include> op_include = "op_include";
	const x3::rule<op_import_class, ast::op_import> op_import = "op_import";
	const x3::rule<filename_class, ast::string_t> filename = "filename";

	BOOST_SPIRIT_DECLARE(decltype(op_out))
	BOOST_SPIRIT_DECLARE(decltype(op_comment_value))
	BOOST_SPIRIT_DECLARE(decltype(op_comment))
	BOOST_SPIRIT_DECLARE(decltype(op_set))
	BOOST_SPIRIT_DECLARE(decltype(op_include))
	BOOST_SPIRIT_DECLARE(decltype(op_import))
	BOOST_SPIRIT_DECLARE(decltype(filename))

} // namespace cppjinja::text

