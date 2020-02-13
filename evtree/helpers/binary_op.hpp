/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "eval/ast.hpp"
#include "parser/ast/common.hpp"

namespace cppjinja::evtnodes {

class binary_op_helper {
	const ast::value_term& left_;
	ast::comparator cmp_;
public:
	binary_op_helper(const ast::value_term& left, ast::comparator op);

	bool operator()(const bool& v) const ;
	bool operator()(const int& v) const =delete ;
	bool operator()(const double& v) const ;
	bool operator()(const east::string_t& v) const ;
	bool operator()(const east::array_v& v) const ;
	bool operator()(const east::map_v& v) const ;
};

} // namespace cppjinja::evtnodes
