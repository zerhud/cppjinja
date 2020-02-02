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

cppjinja::evt::node::render_info cppjinja::evtnodes::tmpl::rinfo() const
{
	return render_info{ {false, false}, {false, false} };
}

cppjinja::ast::string_t cppjinja::evtnodes::tmpl::name() const
{
	return itmpl_.name;
}

bool cppjinja::evtnodes::tmpl::is_leaf() const
{
	return false;
}

void cppjinja::evtnodes::tmpl::render(evt::context& ctx) const
{
	ctx.current_node(this);
	ctx.push_callstack(this, false);

	auto children = ctx.tree().children(this);
	for(auto&& child:children) ctx.add_context(child);
	for(auto&& child:children)
		if(child->name().empty())
			child->render(ctx);

	ctx.pop_callstack(this);
}
