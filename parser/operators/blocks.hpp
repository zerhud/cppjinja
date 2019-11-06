/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast/blocks.hpp"

#include "common.hpp"
#include "opterm.hpp"

DEFINE_OPERATORS(cppjinja::ast::block_raw,     left.value, right.value,
                 left.left_open, right.left_open, left.left_close, right.left_close,
                 left.right_open, right.right_open, left.right_close, right.right_close)


