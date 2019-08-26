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


namespace cppjinja {

using var_name = std::vector<std::string>;

struct fnc_call {
	std::string fnc;
	std::vector<std::variant<std::string,var_name,boost::recursive_wrapper<fnc_call>>> params;
};

template<typename String>
struct st_out {
	std::variant<String,var_name,fnc_call> ref;
	std::variant<String,fnc_call> filters;
};

struct st_for {

};

struct st_if {

};

struct st_set {

};

struct st_call {

};

template<typename String>
using block_content = std::variant<String, st_out<String>, st_for, st_if, st_set, st_call>;

template<typename String>
struct b_block {
	std::string name;
	std::vector<block_content<String>> cnt;
};

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
