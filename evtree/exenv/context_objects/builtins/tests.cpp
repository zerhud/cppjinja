/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "tests.hpp"
#include "../callable_node.hpp"
#include "../value.hpp"

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::builtins_tests::callable::call(
        std::vector<cppjinja::evt::context_object::function_parameter> params) const
{
	auto res = dynamic_cast<const callable_node*>(params.at(0).value.get());
	return std::make_shared<value>(!!res, 1);
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::builtins_tests::defined::call(
        std::vector<cppjinja::evt::context_object::function_parameter> params) const
{
	bool res = params.at(0).value != nullptr;
	return std::make_shared<value>(res, 1);
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::builtins_tests::undefined::call(
        std::vector<cppjinja::evt::context_object::function_parameter> params) const
{
	bool res = params.at(0).value == nullptr;
	return std::make_shared<value>(res, 1);
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::builtins_tests::sameas::call(
        std::vector<cppjinja::evt::context_object::function_parameter> params) const
{
	bool res = params.at(0).value.get() == params.at(1).value.get();
	return std::make_shared<value>(res, 1);
}
