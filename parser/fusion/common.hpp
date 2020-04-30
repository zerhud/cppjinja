/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast/common.hpp"
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::array_v, fields)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::tuple_v, fields)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::array_call, name, call)

BOOST_FUSION_ADAPT_STRUCT(
		cppjinja::ast::binary_op,
		left, op, right)


BOOST_FUSION_ADAPT_STRUCT(
		cppjinja::ast::function_call_parameter,
		name, value)

BOOST_FUSION_ADAPT_STRUCT(
		cppjinja::ast::function_call,
		ref, params)

