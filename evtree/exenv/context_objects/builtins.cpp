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

using namespace std::literals;

cppjinja::evt::context_objects::builtins::builtins()
{
	add_child("namespace", std::make_shared<jinja_namespace>());
}

cppjinja::evt::context_objects::builtins::~builtins() noexcept
{

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

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::jinja_namespace::call(
        std::vector<cppjinja::east::function_parameter> params) const
{
	if(params.empty()) throw std::runtime_error("cannot create empty namespace");
	auto ret = std::make_shared<tree>();
	for(auto& p:params)
		ret->add(*p.name, std::make_shared<value>(*p.val));
	return ret;
}
