/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "tests.hpp"
#include "../callable_node.hpp"
#include "../tree.hpp"
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

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::builtins_tests::upper::call(
        std::vector<cppjinja::evt::context_object::function_parameter> params) const
{
	auto val = params.at(0).value->jval();
	for(auto& v:val.get<std::string>())
		if(!std::isupper(v))
			return std::make_shared<value>(false, 1);
	return std::make_shared<value>(true, 1);
}

std::shared_ptr<cppjinja::evt::context_object> cppjinja::evt::context_objects::builtins_tests::lower::call(std::vector<cppjinja::evt::context_object::function_parameter> params) const
{
	auto val = params.at(0).value->jval();
	for(auto& v:val.get<std::string>())
		if(!std::islower(v))
			return std::make_shared<value>(false, 1);
	return std::make_shared<value>(true, 1);
}

std::shared_ptr<cppjinja::evt::context_object> cppjinja::evt::context_objects::builtins_tests::iterable::call(std::vector<cppjinja::evt::context_object::function_parameter> params) const
{
	bool res = !!dynamic_cast<const tree*>(params.at(0).value.get());
	res = res || params.at(0).value->jval().is_object();
	res = res || params[0].value->jval().is_array();
	return std::make_shared<value>(res, 1);
}
