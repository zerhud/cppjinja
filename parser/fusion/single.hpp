/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast/single.hpp"
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT( cppjinja::ast::op_out, open, value, close)
BOOST_FUSION_ADAPT_STRUCT( cppjinja::ast::op_comment, open, value, close)
BOOST_FUSION_ADAPT_STRUCT(cppjinja::ast::op_set, open, value, close)
BOOST_FUSION_ADAPT_STRUCT(
		cppjinja::ast::op_include,
		open, filename, ignore_missing, with_context, close)
