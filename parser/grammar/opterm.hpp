/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3.hpp>
#include "fusion/opterm.hpp"

namespace cppjinja::text {

	namespace x3 = boost::spirit::x3;

	struct op_term_start_class;
	struct op_term_end_class;
	struct block_term_start_class;
	struct block_term_end_class;
	struct comment_term_start_class;
	struct comment_term_end_class;

	const x3::rule<op_term_start_class,      ast::op_term_start>      op_term_start      = "op_term_start";
	const x3::rule<op_term_end_class,        ast::op_term_end>        op_term_end        = "op_term_end";
	const x3::rule<block_term_start_class,   ast::block_term_start>   block_term_start   = "block_term_start";
	const x3::rule<block_term_end_class,     ast::block_term_end>     block_term_end     = "block_term_end";
	const x3::rule<comment_term_start_class, ast::comment_term_start> comment_term_start = "comment_term_start";
	const x3::rule<comment_term_end_class,   ast::comment_term_end>   comment_term_end   = "comment_term_end";

	BOOST_SPIRIT_DECLARE(decltype(op_term_start))
	BOOST_SPIRIT_DECLARE(decltype(op_term_end))
	BOOST_SPIRIT_DECLARE(decltype(block_term_start))
	BOOST_SPIRIT_DECLARE(decltype(block_term_end))
	BOOST_SPIRIT_DECLARE(decltype(comment_term_start))
	BOOST_SPIRIT_DECLARE(decltype(comment_term_end))

} // namespace cppjinja::text
