/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "grammar/tmpls.hpp"
#include "grammar/opterm.hpp"
#include "grammar/single.hpp"
#include "grammar/blocks.hpp"

#include "common.hpp"

namespace cppjinja::text {
    auto const extend_st_def = omit[block_term_start] >> lit("extends") >> x3::attr(std::nullopt) >> quoted_string >> omit[block_term_end] ;
	auto const extend_st_ex_def = single_var_name >> lit("from") >> quoted_string;

	auto const file_def = +tmpl_ex | +tmpl_original;

	auto const tmpl_def = tmpl_ex | tmpl_original;
	auto const tmpl_original_def =
	           x3::attr(std::string(""))
	        >> *extend_st
	        >> +block_content
	        > x3::eoi
	        ;
	auto const tmpl_ex_def =
	           omit[block_term_start]
	        >> lit("template")
	        >> single_var_name
	        >> -(lit("extends") >> extend_st_ex % ',')
	        >> omit[block_term_end]
	        >> +block_content
	        >> omit[block_term_start]
	        >> lit("endtemplate")
	        >> omit[block_term_end]
	           ;

	class extend_st_class : x3::annotate_on_success {};
	class extend_st_ex_class : x3::annotate_on_success {};

	class tmpl_class : x3::annotate_on_success {} ;
	class tmpl_ex_class : x3::annotate_on_success {} ;
	class tmpl_original_class : x3::annotate_on_success {} ;

	class file_class : x3::annotate_on_success {} ;

	BOOST_SPIRIT_DEFINE( extend_st )
	BOOST_SPIRIT_DEFINE( extend_st_ex )

	BOOST_SPIRIT_DEFINE( tmpl )
	BOOST_SPIRIT_DEFINE( tmpl_ex )
	BOOST_SPIRIT_DEFINE( tmpl_original )

	BOOST_SPIRIT_DEFINE( file )
} // namespace cppjinja::text

