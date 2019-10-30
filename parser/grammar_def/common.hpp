/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

//#define BOOST_SPIRIT_X3_UNICODE

#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>
#include <boost/spirit/home/support/utf8.hpp>
#include "grammar/common.hpp"

namespace cppjinja::text {
	//auto& char_ = x3::unicode::char_;
	//auto& space = x3::unicode::space;
	//using boost::spirit::x3::standard_wide::lit;

	auto& char_ = x3::char_;
	auto& space = x3::space;
	using boost::spirit::x3::lit;

	auto const quoted_string_1_def = *(char_ >> !lit('\'') | lit("\\'") >> x3::attr('\'')) >> char_;
	auto const quoted_string_2_def = *(char_ >> !lit('"') | lit("\\\"") >> x3::attr('"')) >> char_;
	auto const quoted_string_def = lit("'") >> -quoted_string_1_def >> lit("'");

	auto const single_var_name_def = char_("A-Za-z_") >> *char_("0-9A-Za-z_");

	BOOST_SPIRIT_DEFINE( quoted_string )
	BOOST_SPIRIT_DEFINE( single_var_name )

	class quoted_string_class : x3::annotate_on_success {};
	class single_var_name_class : x3::annotate_on_success {};

} // namespace cppjinja::text
