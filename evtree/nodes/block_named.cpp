/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "block_named.hpp"
#include "parser/helpers.hpp"
#include "../evtree.hpp"
#include "evtree/exenv/context_objects/callable_params.hpp"
#include "evtree/exenv/callstack.hpp"
#include "evtree/exenv/expr_solver.hpp"

const cppjinja::ast::block_with_name& cppjinja::evtnodes::block_named::cur_ast() const
{
	return block;
}

bool cppjinja::evtnodes::block_named::has_nondefaulted_params() const
{
	for(auto& p:block.params) if(!p.value.has_value()) return true;
	return false;
}

cppjinja::evtnodes::block_named::block_named(cppjinja::ast::block_named nb)
    : block(std::move(nb))
{
}

void cppjinja::evtnodes::block_named::render(evt::exenv& env) const
{
	if(has_nondefaulted_params()) env.current_node(this);
	else {
		evt::raii_callstack_push calls(&env.calls(), this, evt::context_objects::callable_params({},{}));
		env.out() << evaluate(env);
	}
}

cppjinja::east::string_t cppjinja::evtnodes::block_named::evaluate(cppjinja::evt::exenv& env) const
{
	env.current_node(this);
	evt::raii_push_shadow_ctx ctx_maker(this, &env.ctx());
	auto fmt = inner_evaluate(env);
	return env.result();
}
