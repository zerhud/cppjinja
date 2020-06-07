/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "callable_node.hpp"
#include "../raii.hpp"
#include "value.hpp"

cppjinja::evt::context_objects::callable_node::callable_node(
          exenv* e
        , const evtnodes::callable* n)
    : env(e)
    , node(n)
{
}

cppjinja::evt::context_objects::callable_node::~callable_node() noexcept
{
}

bool cppjinja::evt::context_objects::callable_node::is_it(const cppjinja::evtnodes::callable* n) const
{
	return node == n;
}

void cppjinja::evt::context_objects::callable_node::add(
          cppjinja::east::string_t n
        , std::shared_ptr<cppjinja::evt::context_object> child)
{
	(void)n;
	(void)child;
	throw std::runtime_error("cannot add child to a callable node");
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::callable_node::find(cppjinja::east::var_name n) const
{
	(void)n;
	throw std::runtime_error("cannot find somthing in callable node");
}

cppjinja::east::value_term cppjinja::evt::context_objects::callable_node::solve() const
{
	throw std::runtime_error("cannot solve an callable node");
}

cppjinja::json cppjinja::evt::context_objects::callable_node::jval() const
{
	throw std::logic_error("not ready yet");
}

std::shared_ptr<cppjinja::evt::context_object> cppjinja::evt::context_objects::callable_node::call(
        std::vector<function_parameter> params) const
{
	assert(env);
	assert(node);
	context_objects::callable_params cparams(node->solved_params(*env), std::move(params));
	evt::raii_callstack_push pusher(&env->calls(), node, std::move(cparams));
	return std::make_shared<value>(node->evaluate(*env));
}
