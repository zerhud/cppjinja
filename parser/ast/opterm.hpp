/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <optional>

#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

namespace cppjinja::ast {

namespace x3 = boost::spirit::x3;

struct op_term_start : public x3::position_tagged { bool trim=false; };
struct op_term_end : public x3::position_tagged { bool trim=false; };
struct block_term_start : public x3::position_tagged { bool trim=false; };
struct block_term_end : public x3::position_tagged { bool trim=false; };

} // namespace cppjinja::ast
