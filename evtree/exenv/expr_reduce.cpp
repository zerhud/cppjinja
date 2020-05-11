/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "expr_reduce.hpp"
#include "expr_eval.hpp"

cppjinja::evt::expr_reduce::expr_reduce(const exenv* e)
    : env(e)
{
}

cppjinja::east::map_v cppjinja::evt::expr_reduce::operator()(cppjinja::ast::expr_ops::dict v) const
{
	east::map_v ret;
	expr_eval slv(env);
	for(auto& item:v.items) {
		ret.items[item.name] =
		        std::make_unique<east::value_term>(slv(item.value.get())->solve());
	}
	return ret;
}

cppjinja::east::var_name cppjinja::evt::expr_reduce::operator()(cppjinja::ast::expr_ops::point v) const
{
	east::var_name ret;
	return ret;
}
