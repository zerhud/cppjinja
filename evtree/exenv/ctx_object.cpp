/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "ctx_object.hpp"
#include "expr_solver.hpp"

cppjinja::evt::delay_solver::delay_solver(
          cppjinja::evt::exenv* e
        , const ast::value_term* v)
    : env(e)
    , val(v)
{
}

cppjinja::ast::value_term
cppjinja::evt::delay_solver::call(cppjinja::ast::function_call fnc)
{
	if(!fnc.ref.empty())
		throw std::runtime_error("cannot solve named variable");
	return *val;
}

cppjinja::ast::value_term
cppjinja::evt::delay_solver::solve(cppjinja::ast::var_name var)
{
	if(!var.empty())
		throw std::runtime_error("cannot solve named variable");
	return *val;
}
