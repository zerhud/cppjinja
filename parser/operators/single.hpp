/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast/single.hpp"

#include "common.hpp"
#include "opterm.hpp"

DEFINE_OPERATORS(cppjinja::ast::op_out,     left.value, right.value, left.open, right.open, left.close, right.close)
DEFINE_OPERATORS(cppjinja::ast::op_comment, left.value, right.value, left.open, right.open, left.close, right.close)
DEFINE_OPERATORS(cppjinja::ast::op_set,     left.value, right.value, left.open, right.open, left.close, right.close)
DEFINE_OPERATORS(cppjinja::ast::op_include, left.filename, right.filename, left.ignore_missing, right.ignore_missing, left.with_context, right.with_context)

DEFINE_OPERATORS(cppjinja::ast::filter_call, left.var, right.var)
