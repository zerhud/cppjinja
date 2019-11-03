/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <ostream>
#include "ast/common.hpp"

#define VARIANT_OPERATORS(sname, ...) \
    namespace std { std::ostream& operator << (std::ostream& out, const sname& obj); } \

#define DEFINE_OPERATORS(sname, ...) \
    namespace cppjinja::ast { \
    inline bool operator < (const sname& left, const sname& right) { return cppjinja::lex_cmp(__VA_ARGS__) < 0; } \
    inline bool operator == (const sname& left, const sname& right) { return cppjinja::lex_cmp(__VA_ARGS__) == 0; } \
    inline bool operator != (const sname& left, const sname& right) { return !(left==right); } \
    }\
    namespace std { std::ostream& operator << (std::ostream& out, const sname& obj); }\


namespace cppjinja::ast {
	template<typename T>
	bool operator < (const boost::spirit::x3::forward_ast<T>& left, const boost::spirit::x3::forward_ast<T>& right){ return left.get() < right.get(); }
	template<typename ... Args>
	bool operator < (const boost::spirit::x3::variant<Args...>& left, const boost::spirit::x3::variant<Args...>& right)
	{
		return left.var == right.var;
	}
} // namespace cppjinja::ast

namespace cppjinja {

constexpr int lex_cmp() { return 0; }
template<typename T>
int lex_cmp(const std::vector<T>& left, const std::vector<T>& right)
{
	if(left.size() < right.size()) return -1;
	if(right.size() < left.size()) return  1;
	for(std::size_t i=0;i<left.size();++i) {
		if(left[i] < right[i]) return -1;
		if(right[i] < left[i]) return  1;
	}
	return 0;
}

template<typename Left, typename Right, typename... Args>
constexpr int lex_cmp(const Left& left, const Right& right, const Args&... args)
{
	static_assert((sizeof...(args)%2)==0, "cannot compare the odd number of arguments");
	if(left < right) return -1;
	if(right < left) return  1;
	return lex_cmp(args...);
}

} // namespace cppjinja

DEFINE_OPERATORS(cppjinja::ast::var_name, left, right)
DEFINE_OPERATORS(cppjinja::ast::function_call_parameter, left.name, right.name, left.value, right.value)
DEFINE_OPERATORS(cppjinja::ast::function_call, left.ref, right.ref, left.params, right.params)
DEFINE_OPERATORS(cppjinja::ast::binary_op, left.left, right.left, left.right, right.right)
DEFINE_OPERATORS(cppjinja::ast::value_term, left.var, right.var)
namespace std { std::ostream& operator << (std::ostream& out, const cppjinja::ast::comparator& obj); }
namespace std { std::ostream& operator << (std::ostream& out, const boost::spirit::x3::forward_ast<cppjinja::ast::value_term>& obj); }
