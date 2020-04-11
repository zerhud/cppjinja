/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "expr_filter.hpp"
#include "eval/ast_cvt.hpp"
#include "evtree/exenv.hpp"
#include "expr_solver.hpp"

using namespace cppjinja::details;

void cppjinja::evt::expr_filter::ilegal_operator() const
{
	throw std::logic_error("cannot filter by such value");
}

cppjinja::evt::expr_filter::expr_filter(exenv* ud, const east::value_term& b)
    : base(b)
    , user_data(ud)
{
	if(!user_data)
		throw std::runtime_error("cannot create filter without user data provider");
}

cppjinja::east::value_term cppjinja::evt::expr_filter::operator()(
        const cppjinja::ast::value_term& val)
{
	return boost::apply_visitor(*this, val.var);
}

cppjinja::east::value_term cppjinja::evt::expr_filter::operator()(
        const cppjinja::ast::var_name& obj)
{
	east::function_call call;
	call.ref = east_cvt::cvt(obj);
	return user_data->data()->filter(call, base);
}

cppjinja::east::value_term cppjinja::evt::expr_filter::operator()(
        const cppjinja::ast::function_call& obj)
{
	east::function_call call;
	call.ref = east_cvt::cvt(obj.ref);
	for(auto& p:obj.params)
	{
		east::function_parameter param;
		param.name = p.name;
		param.val = expr_solver(user_data)(p.value.get());
		call.params.emplace_back(std::move(param));
	}
	return user_data->data()->filter(call, base);
}

cppjinja::east::value_term
cppjinja::evt::expr_filter::operator()(const double& obj) const
{
	(void)obj;
	ilegal_operator();
}

cppjinja::east::value_term
cppjinja::evt::expr_filter::operator()(const ast::string_t& obj) const
{
	(void)obj;
	ilegal_operator();
}

cppjinja::east::value_term
cppjinja::evt::expr_filter::operator()(const ast::tuple_v& obj) const
{
	(void)obj;
	ilegal_operator();
}

cppjinja::east::value_term
cppjinja::evt::expr_filter::operator()(const ast::array_v& obj) const
{
	(void)obj;
	ilegal_operator();
}

cppjinja::east::value_term
cppjinja::evt::expr_filter::operator()(const ast::binary_op& obj) const
{
	(void)obj;
	ilegal_operator();
}
