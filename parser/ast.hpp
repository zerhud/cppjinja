/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <string>
#include <variant>
#include <vector>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_wrapper.hpp>

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

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

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

using var_name = std::vector<std::string>;
DEFINE_OPERATORS(var_name, left, right)


struct fnc_call {
	std::string ref;
	std::vector<std::variant<std::string,var_name,boost::recursive_wrapper<fnc_call>>> params;
};
DEFINE_OPERATORS(fnc_call, left.ref, right.ref, left.params, right.params)

template<typename String>
struct st_out {
	std::variant<String,var_name,fnc_call> ref;
	std::vector<std::variant<var_name,fnc_call>> params;
};
DEFINE_OPERATORS_STRING(st_out, left.ref, right.ref, left.params, right.params)
template<typename String>
std::ostream& operator << (std::ostream& out, const st_out<String>& obj)
{
	auto printer = overloaded{
		[&out](const std::wstring&){out << "[wstring content]";},
		[&out](const auto& i){out << i;}
	};
	std::visit(printer, obj.ref);
	for(auto& i:obj.params) {
		out << '|';
		std::visit(printer, i);
	}
	return out;
}

struct st_for {

};

struct st_if {

};

struct st_set {

};

struct st_call {

};

template<typename String> struct b_block;

template<typename String>
using block_content = std::variant<String, st_out<String>, boost::recursive_wrapper<b_block<String>>>;//, st_for, st_if, st_set, st_call>;

template<typename String>
struct b_block {
	std::string name;
	std::vector<block_content<String>> cnt;
};
DEFINE_OPERATORS_STRING(b_block, left.name, right.name, left.cnt, right.cnt)
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
	};

	out << "block: " << obj.name;
	for(auto& i:obj.cnt) {
		out << std::endl << "\t" ;
		std::visit(printer, i);
	}

	return out;
}

using s_block = b_block<std::string>;
using w_block = b_block<std::wstring>;

struct b_macro {

};

struct b_filter {

};

class jtmpl final {
public:
	jtmpl();
};

} // namespace cppjinja

//BOOST_FUSION_ADAPT_STRUCT( cppjinja::fnc_call, fnc, params )
//BOOST_FUSION_ADAPT_STRUCT( cppjinja::st_out, ref, filters )
//BOOST_FUSION_ADAPT_STRUCT( cppjinja::s_block, name, cnt )
//BOOST_FUSION_ADAPT_STRUCT( cppjinja::w_block, name, cnt )
