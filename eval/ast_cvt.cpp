/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

/*
#include "ast_cvt.hpp"

using namespace std::literals;

cppjinja::east::var_name cppjinja::details::east_cvt::cvt(const ast::var_name& name)
{
	return name;
}

cppjinja::east::string_t cppjinja::details::east_cvt::cvt(const ast::string_t& str)
{
	return str;
}

cppjinja::east::value_term cppjinja::details::east_cvt::cvt(const ast::value_term& val)
{
	using east::value_term;
	if(val.var.type() == typeid(double))
		return value_term(boost::get<double>(val.var));
	if(val.var.type() == typeid(ast::string_t))
		return value_term(cvt(boost::get<ast::string_t>(val.var)));
	assert(false);
	return value_term(""s);
}
*/
