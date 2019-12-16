/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "ast_cvt.hpp"

using namespace std::literals;

cppjinja::east::var_name cppjinja::details::east_cvt::cvt(const ast::var_name& name)
{
	return name;
}

cppjinja::east::function_call cppjinja::details::east_cvt::cvt(const ast::function_call& fnc)
{
	east::function_call ret;
	ret.ref = cvt(fnc.ref);
	for(auto& param:fnc.params)
	{
		east::function_parameter& p = ret.params.emplace_back();
		if(param.name) p.name = cvt(*param.name);
		p.val = cvt(param.value);
	}

	return ret;
}

cppjinja::east::function_call cppjinja::details::east_cvt::cvt(const ast::filter_call& filter)
{
	if(filter.var.type() == typeid(ast::function_call))
		return cvt(boost::get<ast::function_call>(filter));

	ast::function_call fnc;
	fnc.ref = boost::get<ast::var_name>(filter.var);
	return cvt(fnc);
}

cppjinja::east::string_t cppjinja::details::east_cvt::cvt(const ast::string_t& str)
{
	return str;
}

cppjinja::east::value_term cppjinja::details::east_cvt::cvt(const ast::value_term& val)
{
	if(val.var.type() == typeid(double)) return boost::get<double>(val.var);
	if(val.var.type() == typeid(ast::string_t)) return cvt(boost::get<ast::string_t>(val.var));
	assert(false);
	return ""s;
}
