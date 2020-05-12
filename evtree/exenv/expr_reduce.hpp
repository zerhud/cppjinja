/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "eval/ast.hpp"
#include "parser/ast/expr.hpp"
#include "evtree/declarations.hpp"

namespace cppjinja::evt {

class expr_reduce final {
	const exenv* env;
public:
	expr_reduce(const exenv* e);

	east::map_v operator()(ast::expr_ops::dict v) const ;
};

} // namespace cppjinja::evt
