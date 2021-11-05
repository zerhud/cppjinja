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

#include "expr.hpp"

#define DEFINE_DEFINED_OPERATORS(sname, ...) \
	namespace cppjinja::ast { \
	inline bool operator == (const x3::forward_ast<sname>& left, const x3::forward_ast<sname>& right) { return left.get() == right.get(); } \
	inline bool operator != (const x3::forward_ast<sname>& left, const x3::forward_ast<sname>& right) { return !(left==right); } \
	}\
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

#ifndef __clang__
template<typename T, typename O>
concept left_shifted_type = requires(O& out, const T& obj)
{
    {out << obj} -> std::same_as<O&>;
};

template<typename T>
concept printable_type = left_shifted_type<T, std::ostream>;
#endif

#ifdef __clang__
template<typename T>
#else
template<printable_type T>
#endif
std::ostream& operator << (std::ostream& out, const std::vector<T>& vec)
{
    using namespace std::literals;
    for(const auto& v:vec) out << v << ',';
    out << ". "sv;
    return out;
}
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

} // namespace ast
} // namespace cppjinja
