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

cppjinja::node::render_info cppjinja::evtnodes::tmpl::rinfo() const
{
	return render_info{ false, false };
}

cppjinja::ast::string_t cppjinja::evtnodes::tmpl::name() const
{
	return itmpl_.name;
}

bool cppjinja::evtnodes::tmpl::is_leaf() const
{
	return false;
}

void cppjinja::evtnodes::tmpl::render(
          std::ostream& out
        , const evtree& tree
        , evt::context& ctx
        ) const
{
	ctx.push_callstack(this);

	auto children = tree.children(this);
	//for(auto&& child:children) ctx.add_context(child);
	//for(auto&& child:children) child->render(out, tree, ctx);
	for(auto&& child:children)
		if(child->name().empty())
			child->render(out, tree, ctx);

	ctx.pop_callstack(this);
}
