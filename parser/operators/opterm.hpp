/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "common.hpp"
#include "ast/opterm.hpp"


DEFINE_OPERATORS(cppjinja::ast::op_term_start,      left.trim, right.trim)
DEFINE_OPERATORS(cppjinja::ast::op_term_end,        left.trim, right.trim)
DEFINE_OPERATORS(cppjinja::ast::block_term_start,   left.trim, right.trim)
DEFINE_OPERATORS(cppjinja::ast::block_term_end,     left.trim, right.trim)
DEFINE_OPERATORS(cppjinja::ast::comment_term_start, left.trim, right.trim)
DEFINE_OPERATORS(cppjinja::ast::comment_term_end,   left.trim, right.trim)
