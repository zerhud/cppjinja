/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <ostream>
#include "ast/expr.hpp"


namespace cppjinja::ast::expr_ops {

template<typename T> inline bool operator == (const x3::forward_ast<T>& left, const x3::forward_ast<T>& right) { return left.get() == right.get(); }
template<typename T> inline bool operator == (const x3::forward_ast<T>& left, const T& right) { return left.get() == right; }
template<typename T> inline bool operator == (const T& left, const x3::forward_ast<T>& right) { return left == right.get(); }
template<typename T> inline std::ostream& operator << (std::ostream& out, const x3::forward_ast<T>& obj) { return out << obj.get(); }

template<typename Left, typename Right>
inline bool operator == (const x3::forward_ast<Left>& left, const Right& right){
	if constexpr (std::is_same_v<Left, Right>) return left.get() == expr{right};
	return false;
}

bool operator == (const expr& left, const expr& right);
bool operator == (const term& left, const term& right);
bool operator == (const list& left, const list& right);
bool operator == (const tuple& left, const tuple& right);
bool operator == (const dict& left, const dict& right);
bool operator == (const lvalue& left, const lvalue& right);
bool operator == (const any_assign& left, const any_assign& right);
bool operator == (const math& left, const math& right);
bool operator == (const concat& left, const concat& right);
bool operator == (const in_check& left, const in_check& right);
bool operator == (const cmp_check& left, const cmp_check& right);
bool operator == (const logic_check& left, const logic_check& right);
bool operator == (const negate& left, const negate& right);
bool operator == (const fnc_call& left, const fnc_call& right);
bool operator == (const filter_call& left, const filter_call& right);
bool operator == (const filter& left, const filter& right);
bool operator == (const single_var_name& left, const single_var_name& right);
bool operator == (const point& left, const point& right);
bool operator == (const op_if& left, const op_if& right);

std::ostream& operator << (std::ostream& out, math_op op);
std::ostream& operator << (std::ostream& out, logic_op op);
std::ostream& operator << (std::ostream& out, cmp_op op);
std::ostream& operator << (std::ostream& out, const expr& op);
std::ostream& operator << (std::ostream& out, const term& right);
std::ostream& operator << (std::ostream& out, const list& right);
std::ostream& operator << (std::ostream& out, const tuple& right);
std::ostream& operator << (std::ostream& out, const dict& right);
std::ostream& operator << (std::ostream& out, const lvalue& right);
std::ostream& operator << (std::ostream& out, const any_assign& right);
std::ostream& operator << (std::ostream& out, const math& right);
std::ostream& operator << (std::ostream& out, const concat& right);
std::ostream& operator << (std::ostream& out, const in_check& right);
std::ostream& operator << (std::ostream& out, const cmp_check& right);
std::ostream& operator << (std::ostream& out, const logic_check& right);
std::ostream& operator << (std::ostream& out, const negate& right);
std::ostream& operator << (std::ostream& out, const fnc_call& right);
std::ostream& operator << (std::ostream& out, const filter_call& right);
std::ostream& operator << (std::ostream& out, const filter& right);
std::ostream& operator << (std::ostream& out, const single_var_name& right);
std::ostream& operator << (std::ostream& out, const point& right);
std::ostream& operator << (std::ostream& out, const op_if& right);

} // namespace cppjinja::ast::expr_ops
