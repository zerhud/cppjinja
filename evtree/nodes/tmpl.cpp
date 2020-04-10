/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "tmpl.hpp"
#include "../evtree.hpp"

cppjinja::evtnodes::tmpl::tmpl(cppjinja::ast::tmpl t)
    : itmpl_(std::move(t))
{
}

cppjinja::evt::render_info cppjinja::evtnodes::tmpl::rinfo() const
{
	return {false, false};
}

cppjinja::ast::string_t cppjinja::evtnodes::tmpl::name() const
{
	return itmpl_.name;
}

bool cppjinja::evtnodes::tmpl::is_leaf() const
{
	return false;
}

void cppjinja::evtnodes::tmpl::render(evt::exenv& ctx) const
{
	evt::raii_push_ctx ctx_maker(this, &ctx.ctx());
	ctx.current_node(this);

	auto children = ctx.children(this);
	for(auto&& child:children)
		if(child->name().empty())
		{
			auto* cb_child = dynamic_cast<const callable*>(child);
			evt::raii_push_callstack calls_maker(this, cb_child, &ctx.calls());
			child->render(ctx);
		}
}
