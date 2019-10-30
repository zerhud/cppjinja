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

BOOST_FUSION_ADAPT_TPL_STRUCT(
		(String),
		(cppjinja::binary_op)(String),
		left, right)
  

BOOST_FUSION_ADAPT_TPL_STRUCT(
		(String),
		(cppjinja::fnc_call_parameter)(String),
		name, value)

BOOST_FUSION_ADAPT_TPL_STRUCT(
		(String),
		(cppjinja::fnc_call)(String),
		ref, params)

