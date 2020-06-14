/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "builtins.hpp"
#include "tree.hpp"
#include "value.hpp"

#include "builtins/tests.hpp"

using namespace std::literals;

cppjinja::evt::context_objects::builtins::builtins()
{
	add_child("namespace", std::make_shared<jinja_namespace>());
	auto tests = std::make_shared<tree>();
	add_child("$tests", tests);
	tests->add("callable", std::make_shared<builtins_tests::callable>());
	tests->add("defined", std::make_shared<builtins_tests::defined>());
	tests->add("undefined", std::make_shared<builtins_tests::undefined>());
	tests->add("sameas", std::make_shared<builtins_tests::sameas>());
}

cppjinja::evt::context_objects::builtins::~builtins() noexcept
{

}


std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::builtin_function::bool_result(bool res) const
{
	static auto res_true = std::make_shared<value>(true, 1);
	static auto res_false = std::make_shared<value>(false, 1);
	return res ? res_true : res_false;
}

void cppjinja::evt::context_objects::builtin_function::add(
        cppjinja::east::string_t n, std::shared_ptr<cppjinja::evt::context_object> child)
{
	(void)n;
	(void)child;
	throw std::runtime_error("cannot add something to builtin function");
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::builtin_function::find(east::var_name n) const
{
	(void)n;
	throw std::runtime_error("cannot find something in builtin function");
}

cppjinja::east::value_term cppjinja::evt::context_objects::builtin_function::solve() const
{
	throw std::runtime_error("cannot solve builtin function");
}

cppjinja::json cppjinja::evt::context_objects::builtin_function::jval() const
{
	throw std::runtime_error("cannot solve builtin function");
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::jinja_namespace::call(
        std::vector<function_parameter> params) const
{
	if(params.empty()) throw std::runtime_error("cannot create empty namespace");
	auto ret = std::make_shared<tree>();
	for(auto& p:params)
		ret->add(*p.name, p.value);
	return ret;
}
