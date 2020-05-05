/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <ostream>
#include <boost/spirit/home/x3/support/ast/variant.hpp>

#include "ast/common.hpp"

#define VARIANT_OPERATORS(sname, ...) \
    namespace std { std::ostream& operator << (std::ostream& out, const sname& obj); } \

#define DEFINE_OPERATORS(sname, ...) \
    namespace cppjinja::ast { \
    inline bool operator == (const sname& left, const sname& right) { return cppjinja::lex_eq(__VA_ARGS__); } \
    inline bool operator != (const sname& left, const sname& right) { return !(left==right); } \
    inline bool operator == (const x3::forward_ast<sname>& left, const x3::forward_ast<sname>& right) { return left.get() == right.get(); } \
    inline bool operator != (const x3::forward_ast<sname>& left, const x3::forward_ast<sname>& right) { return !(left==right); } \
    }\
    namespace std { std::ostream& operator << (std::ostream& out, const sname& obj); }\


namespace cppjinja::ast
{
} // namespace cppjinja::ast

namespace cppjinja {

constexpr bool lex_eq() { return true; }

template<typename Left, typename Right, typename... Args>
constexpr int lex_eq(const Left& left, const Right& right, const Args&... args)
{
	static_assert((sizeof...(args)%2)==0, "cannot compare the odd number of arguments");
	if(left == right) return lex_eq(args...);
	return false;
}

namespace ast {
template<typename T>
bool operator == (const boost::spirit::x3::forward_ast<T>& left, const boost::spirit::x3::forward_ast<T>& right)
{
	return left.get() == right.get();
}

template<typename ... Args>
bool operator == (const boost::spirit::x3::variant<Args...>& left, const boost::spirit::x3::variant<Args...>& right)
{
	return left.var == right.var;
}

template<typename Vec, typename Val, typename... Vals>
void empback(Vec& vec, Val&& val, Vals&&... vals)
{
	vec.emplace_back(value_term{std::forward<Val>(val)});
	if constexpr (sizeof...(Vals)!=0) empback(vec, std::forward<Vals>(vals)...);
}

template<typename... Args>
inline array_v make_array(Args&&... args)
{
	array_v ret;
	empback(ret.fields, std::forward<Args>(args)...);
	return ret;
}

template<typename... Args>
inline tuple_v make_tuple(Args&&... args)
{
	tuple_v ret;
	empback(ret.fields, std::forward<Args>(args)...);
	return ret;
}
} // namespace ast
} // namespace cppjinja

DEFINE_OPERATORS(cppjinja::ast::var_name, left, right)
DEFINE_OPERATORS(cppjinja::ast::function_call_parameter, left.name, right.name, left.value, right.value)
DEFINE_OPERATORS(cppjinja::ast::function_call, left.ref, right.ref, left.params, right.params)
DEFINE_OPERATORS(cppjinja::ast::array_call, left.name, right.name, left.call, right.call)
DEFINE_OPERATORS(cppjinja::ast::array_calls, left, right)
DEFINE_OPERATORS(cppjinja::ast::binary_op, left.left, right.left, left.right, right.right)
DEFINE_OPERATORS(cppjinja::ast::value_term, left.var, right.var)
DEFINE_OPERATORS(cppjinja::ast::array_v, left.fields, right.fields)
DEFINE_OPERATORS(cppjinja::ast::tuple_v, left.fields, right.fields)
namespace std { std::ostream& operator << (std::ostream& out, const cppjinja::ast::comparator& obj); }
namespace std { std::ostream& operator << (std::ostream& out, const boost::spirit::x3::forward_ast<cppjinja::ast::value_term>& obj); }
namespace cppjinja::ast {
bool operator == (const var_name& left, const x3::variant<var_name,array_calls>& right);
bool operator == (const x3::variant<var_name,array_calls>& left, const var_name& right);
} // namespace cppjinja::ast
