/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast.hpp"


#define DEFINE_OPERATORS_STRING(sname, ...) \
    template<typename String> \
    bool operator < (const sname<String>& left, const sname<String>& right) \
    { \
    	return lex_cmp(__VA_ARGS__) < 0; \
    } \
    template<typename String> \
    bool operator == (const sname<String>& left, const sname<String>& right) \
    { \
    	return lex_cmp(__VA_ARGS__) == 0; \
    } \
    template<typename String> \
    bool operator != (const sname<String>& left, const sname<String>& right) \
    { \
    	return !(left==right); \
    } \


#define DEFINE_OPERATORS(sname, ...) \
    inline bool operator < (const sname& left, const sname& right) { return lex_cmp(__VA_ARGS__) < 0; } \
    inline bool operator == (const sname& left, const sname& right) { return lex_cmp(__VA_ARGS__) == 0; } \
    inline bool operator != (const sname& left, const sname& right) { return !(left==right); } \
    std::ostream& operator << (std::ostream& out, const sname& obj); \

namespace cppjinja {

template<typename T>
bool operator < (const boost::recursive_wrapper<T>& left, const boost::recursive_wrapper<T>& right)
{
	return left.get() < right.get();
}

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
int lex_cmp(const Left& left, const Right& right, const Args&... args)
{
	static_assert((sizeof...(args)%2)==0, "cannot compare the odd number of arguments");
	if(left < right) return -1;
	if(right < left) return  1;
	return lex_cmp(args...);
}


DEFINE_OPERATORS(var_name, left, right)
DEFINE_OPERATORS(jtmpl, left.name, right.name, left.extends, right.extends)
std::ostream& operator << (std::ostream& out, comparator obj);

template<typename String>
std::ostream& operator << (std::ostream& out, const value_term<String>& obj)
{
	auto printer = overloaded{
		  [&out](const auto& i){out << i;}
		, [&out](const std::wstring&){ out << "[wstring content]";}
		, [&out](const boost::recursive_wrapper<fnc_call<String>>& i){out << i.get();}
	};
	std::visit(printer, obj);
	return out;
}

DEFINE_OPERATORS_STRING(fnc_call, left.ref, right.ref, left.params, right.params)
template<typename String>
std::ostream& operator << (std::ostream& out, const fnc_call<String>& obj)
{
	out << obj.ref << '(';
	for(auto& i:obj.params)  out << i << ',';
	return out << ')';
}


inline bool operator < (const st_raw&, const st_raw&) { return false; }
inline bool operator == (const st_raw&, const st_raw&) { return true; }
inline bool operator != (const st_raw& left, const st_raw& right) { return !(left==right); }
std::ostream& operator << (std::ostream& out, const st_raw& obj);

DEFINE_OPERATORS_STRING(st_out, left.ref, right.ref, left.params, right.params)
template<typename String>
std::ostream& operator << (std::ostream& out, const st_out<String>& obj)
{
	auto printer = overloaded{
		  [&out](const std::wstring&){out << "[wstring content]";}
		, [&out](const auto& i){out << i;}
	};
	std::visit(printer, obj.ref);
	for(auto& i:obj.params) {
		out << '|';
		std::visit(printer, i);
	}
	return out;
}



DEFINE_OPERATORS_STRING(st_for, left.params, right.params, left.ref, right.ref)
template<typename String>
std::ostream& operator << (std::ostream& out, const st_for<String>& obj)
{
	auto printer = overloaded{
		  [&out](const std::wstring&){out << "[wstring content]";}
		, [&out](const boost::recursive_wrapper<fnc_call<String>>& i){out << i.get();}
		, [&out](const auto& i){out << i;}
	};

	out << "for ";
	for(auto& f:obj.params) out << f << ", ";
	out << ": ";
	std::visit(printer, obj.ref);
	return out;
}

DEFINE_OPERATORS_STRING(extends_op, left.name, right.name)
template<typename String>
std::ostream& cppjinja::operator << (std::ostream& out, const extends_op<String>& obj)
{
	out << "extends " << obj.name;
	return out;
}

DEFINE_OPERATORS_STRING(set_op, left.names, right.names, left.value, right.value)
template<typename String>
std::ostream& operator << (std::ostream& out, const set_op<String>& obj)
{
	out << "set ";
	for(auto& n:obj.names) out << n << ", ";
	out << " = " << obj.value;
	return out;
}

DEFINE_OPERATORS_STRING(st_if, left.op, right.op, left.left, right.left, left.right, right.right)
template<typename String>
std::ostream& operator << (std::ostream& out, const st_if<String>& obj)
{
	out << "if " << obj.left << " " << obj.op << " " << obj.right;
	return out;
}

DEFINE_OPERATORS_STRING(st_comment, left.cnt, right.cnt)
template<typename String>
std::ostream& operator << (std::ostream& out, const st_comment<String>& obj)
{
	out << "comment: " << obj.cnt;
	return out;
}


DEFINE_OPERATORS_STRING(st_macro, left.name, right.name, left.params, right.params)
DEFINE_OPERATORS_STRING(macro_parameter, left.name, right.name, left.value, right.value)
template<typename String>
std::ostream& operator << (std::ostream& out, const macro_parameter<String>& obj)
{
	out << obj.name;
	if(obj.value) out << "=" << *obj.value;
	return out;
}
template<typename String>
std::ostream& operator << (std::ostream& out, const st_macro<String>& obj)
{
	out << "macro: " << obj.name << '(';
	for(auto& p:obj.params) out << p << ',';
	out << ')';
	return out;
}

DEFINE_OPERATORS_STRING(b_block, left.ref, right.ref, left.cnt, right.cnt)
template<typename String>
bool operator == (const boost::recursive_wrapper<b_block<String>>& left, const boost::recursive_wrapper<b_block<String>>& right)
{ return left.get() == right.get();}
template<typename String>
std::ostream& operator << (std::ostream& out, const boost::recursive_wrapper<b_block<String>>& obj)
{out << obj.get(); return out;}
template<typename String>
std::ostream& operator << (std::ostream& out, const b_block<String>& obj)
{
	auto printer = overloaded{
	  [&out](const std::string& c){out << c << ", ";}
	, [&out](const std::wstring&){out << "[string content]";}
	, [&out](const st_out<String>&){out << "[out operator]";}
	, [&out](const boost::recursive_wrapper<b_block<String>>& o){ out << o.get();}
	, [&out](const auto& c){out << c;}
	};

	out << "block: " ;
	if(obj.ref) std::visit(printer, *obj.ref);
	else out << "[no ref]";
	for(auto& i:obj.cnt) {
		out << std::endl << "\t" ;
		std::visit(printer, i);
	}

	return out;
}

} // namespace cppjinja
