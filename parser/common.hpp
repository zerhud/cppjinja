/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#define BOOST_SPIRIT_X3_UNICODE

#include <string>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/support/utf8.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>

namespace cppjinja {

struct parser_env {
	struct {
		std::string b= "<=";
		std::string e= "=>";
	} output;
	struct {
		std::string b= "<%";
		std::string e= "%>";
	} term;
	struct {
		std::string b="<#";
		std::string e="#>";
	} cmt;
};

namespace ast {

namespace x3 = boost::spirit::x3;
using string_t = std::string;

template<typename T>
using forward_ast = boost::spirit::x3::forward_ast<T>;

} // namespace ast

} // namespace cppjinja
