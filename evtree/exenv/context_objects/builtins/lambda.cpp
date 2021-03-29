/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "lambda.hpp"

cppjinja::evt::context_objects::lambda_function::lambda_function(
        cppjinja::evt::context_objects::lambda_function::fnc_type fnc)
    : lambda(std::move(fnc))
{
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::lambda_function::call(
        std::pmr::vector<cppjinja::evt::context_object::function_parameter> params) const
{
	return lambda ? lambda(std::move(params)) : result_none();
}
