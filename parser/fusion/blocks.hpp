/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast/blocks.hpp"
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
		cppjinja::ast::block_raw,
		left_open, left_close,
		value,
		right_open, right_close)

BOOST_FUSION_ADAPT_STRUCT(
		cppjinja::ast::block_if,
		left_open, condition, left_close,
		content, content, elifs, else_block,
		right_open, right_close)

BOOST_FUSION_ADAPT_STRUCT(
		cppjinja::ast::block_elif,
		left_open, condition, left_close,
		content, content)

BOOST_FUSION_ADAPT_STRUCT(
		cppjinja::ast::else_thread,
		left_open, left_close, content, content)

BOOST_FUSION_ADAPT_STRUCT(
		cppjinja::ast::block_for,
		left_open,
		vars, value, condition, recursive,
		left_close,
		content, content, else_block,
		right_open, right_close)

BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::macro_parameter, name, value)
BOOST_FUSION_ADAPT_STRUCT(
		cppjinja::ast::block_macro,
		left_open, name, params, left_close,
		content, content,
		right_open, right_close)

BOOST_FUSION_ADAPT_STRUCT(
		cppjinja::ast::block_named,
		left_open, name, params, left_close,
		content, content,
		right_open, right_close)

BOOST_FUSION_ADAPT_STRUCT(
		cppjinja::ast::block_filtered,
		left_open, flt, left_close,
		content,
		right_open, right_close)

BOOST_FUSION_ADAPT_STRUCT(
		cppjinja::ast::block_set,
		left_open, name, filters, left_close,
		content, content,
		right_open, right_close)

BOOST_FUSION_ADAPT_STRUCT(
    cppjinja::ast::block_call,
    left_open, params, name, call_params, left_close,
    content, content,
    right_open, right_close)
