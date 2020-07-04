/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "block_for.hpp"
#include "../exenv/expr_eval.hpp"
#include "evtree/exenv/context_objects/value.hpp"

using obj_val = cppjinja::evt::context_objects::value;

cppjinja::evtnodes::block_for::block_for(cppjinja::ast::block_for ast_bl)
    : abl(ast_bl)
{
	if(abl.vars.size() !=1 && abl.vars.size() != 2)
		throw std::runtime_error("for can itearate only with two or one variables");
}

cppjinja::evt::render_info cppjinja::evtnodes::block_for::rinfo() const
{
	return {};
}

void cppjinja::evtnodes::block_for::render(cppjinja::evt::exenv& env) const
{
	env.current_node(this);
	evt::raii_push_ctx ctx(this, &env.ctx());
	auto value = evt::expr_eval(&env)(abl.value);
	assert(value);
	assert(abl.vars.size()==1 || abl.vars.size()==2);
	if(abl.vars.size() == 1) add_single(env, value->jval());
	else if(abl.vars.size() == 2) add_two(env, value->jval());
}

void cppjinja::evtnodes::block_for::add_single(evt::exenv& env, cppjinja::json val) const
{
	assert(abl.vars.size() == 1);
	for(auto& lvar:val) {
		env.locals().add(abl.vars[0], std::make_shared<obj_val>(lvar, 1));
		render_children(env.children(this), env, rinfo());
	}
}

void cppjinja::evtnodes::block_for::add_two(cppjinja::evt::exenv& env, cppjinja::json val) const
{
	assert(abl.vars.size() == 2);
	for(auto& lvar:val.items()) {
		env.locals().add(abl.vars[0], std::make_shared<obj_val>(lvar.key(), 1));
		env.locals().add(abl.vars[1], std::make_shared<obj_val>(lvar.value(), 1));
		render_children(env.children(this), env, rinfo());
	}
}
