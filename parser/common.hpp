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
#include <memory_resource>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/support/utf8.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>

namespace cppjinja {

struct parser_env_tag;
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

	static const parser_env* default_env() ;

	virtual std::string file_name() const =0 ;
	virtual void on_error() const =0 ;
	virtual std::ostream& elog() const =0 ;
	virtual std::string format_err_msg(const std::string& which_rule) const =0;
};

class throw_cerr_env : public parser_env {
	std::string cur_file;
public:
	throw_cerr_env(std::string_view file_name);
	std::string file_name() const override ;
	void on_error() const override ;
	std::ostream& elog() const override ;
	std::string format_err_msg(const std::string& which_rule) const override ;
};

namespace ast {

namespace x3 = boost::spirit::x3;
using string_t = std::string;

template<typename T>
using forward_ast = boost::spirit::x3::forward_ast<T>;

} // namespace ast

} // namespace cppjinja
