/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "block_set.hpp"
#include "evtree/exenv/context_objects/value.hpp"

cppjinja::evtnodes::block_set::block_set(cppjinja::ast::block_set nb)
    : block(std::move(nb))
{
}

cppjinja::evt::render_info cppjinja::evtnodes::block_set::inner_ri() const
{
	return evt::render_info{ block.left_close.trim, block.right_open.trim };
}

cppjinja::evt::render_info cppjinja::evtnodes::block_set::rinfo() const
{
	return {block.left_open.trim, block.right_close.trim};
}

void cppjinja::evtnodes::block_set::render(cppjinja::evt::exenv& env) const
{
	env.current_node(this);
	evt::raii_push_ctx ctx_holder(this, &env.ctx());
	render_children(env.children(this), env, inner_ri());
	auto child = std::make_shared<evt::context_objects::value>(env.result(),1);
	env.locals().add(block.name, child);
}
