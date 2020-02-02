/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "content.hpp"

using namespace std::literals;

cppjinja::evtnodes::content::content(cppjinja::ast::string_t c)
    : cnt(std::move(c))
{
}

void cppjinja::evtnodes::content::trim_left()
{
}

void cppjinja::evtnodes::content::trim_right()
{
}

cppjinja::evt::node::render_info cppjinja::evtnodes::content::rinfo() const
{
	return render_info{ {false, false}, {false, false} };
}

cppjinja::ast::string_t cppjinja::evtnodes::content::name() const
{
	return "content"s;
}

bool cppjinja::evtnodes::content::is_leaf() const
{
	return true;
}

void cppjinja::evtnodes::content::render(cppjinja::evt::context& ctx) const
{
	ctx.current_node(this);
	ctx.out() << cnt;
}