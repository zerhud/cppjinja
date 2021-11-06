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
#include "nodes/tmpl.hpp"

using cppjinja::evt::context_object;
using cppjinja::evt::context_objects::callable_node;

callable_node::callable_node(exenv* e, const evtnodes::callable* n)
    : callable_node(e, n, nullptr)
{
}

callable_node::callable_node(exenv *e, const evtnodes::callable *n, const evtnodes::tmpl* ctc)
    : env(e)
    , node(n)
    , cross_tmpl_call(ctc)
{
}

callable_node::~callable_node() noexcept
{
}

bool callable_node::is_it(const cppjinja::evtnodes::callable* n) const
{
	return node == n;
}

void callable_node::add(
          cppjinja::east::string_t n
        , std::shared_ptr<context_object> child)
{
	(void)n;
	(void)child;
	throw std::runtime_error("cannot add child to a callable node");
}

std::shared_ptr<context_object> callable_node::find(cppjinja::east::string_t n) const
{
	(void)n;
	throw std::runtime_error("cannot find somthing in callable node");
}

absd::data callable_node::solve() const
{
	throw std::runtime_error("cannot solve an callable node");
}

std::shared_ptr<context_object> callable_node::call(
        std::pmr::vector<function_parameter> params) const
{
	assert(env);
	assert(node);
	using context_objects::callable_params;
	callable_params cparams(node->solved_params(*env), std::move(params));
	evt::raii_push_ctx tmpl_shadow(cross_tmpl_call, &env->ctx());
	evt::raii_callstack_push pusher(&env->calls(), node, std::move(cparams));
	return std::make_shared<value>(node->evaluate(*env));
}
