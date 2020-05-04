/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast.hpp"
#include "parser/ast/single.hpp"

namespace cppjinja::details {

struct east_cvt {
	static east::var_name cvt(const ast::var_name& name) ;
	static east::string_t cvt(const ast::string_t& str) ;
	static east::value_term cvt(const ast::value_term& val) ;
};

} // namespace cppjinja::details
