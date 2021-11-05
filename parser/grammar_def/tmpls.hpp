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
	auto const extend_st_def =
		   omit[block_term_start]
		>> lit("extends")
		>> x3::attr(std::nullopt)
		>> filename
		>> omit[block_term_end] ;
	//NOTE: !!! single_var_name should be here
	auto const extend_st_ex_def = x3::attr(std::nullopt) >> -(lit("from") >> filename);

	auto const file_def = *op_include >> *op_import >> (+tmpl_ex | +tmpl_original);

	auto const tmpl_def = tmpl_ex | tmpl_original;
	auto const tmpl_original_def =
	           x3::attr(ast::expr_ops::single_var_name{})
	        >> *extend_st
	        >> block_content_vec
	        >> x3::eoi
	        ;
	auto const tmpl_ex_def =
	           omit[block_term_start]
	        >> lit("template")
	        >> expr_ops::single_var_name
	        >> -(lit("extends") >> extend_st_ex % ',')
	        >> omit[block_term_end]
	        >  block_content_vec
	        >  omit[block_term_start]
	        >  lit("endtemplate")
	        >  omit[block_term_end]
	           ;

	struct extend_st_class : error_handler, x3::annotate_on_success {};
	struct extend_st_ex_class : error_handler, x3::annotate_on_success {};

	struct tmpl_class : error_handler, x3::annotate_on_success {} ;
	struct tmpl_ex_class : error_handler, x3::annotate_on_success {} ;
	struct tmpl_original_class : error_handler, x3::annotate_on_success {} ;

	struct file_class : error_handler, x3::annotate_on_success {} ;

	BOOST_SPIRIT_DEFINE( extend_st )
	BOOST_SPIRIT_DEFINE( extend_st_ex )

	BOOST_SPIRIT_DEFINE( tmpl )
	BOOST_SPIRIT_DEFINE( tmpl_ex )
	BOOST_SPIRIT_DEFINE( tmpl_original )

	BOOST_SPIRIT_DEFINE( file )
} // namespace cppjinja::text

