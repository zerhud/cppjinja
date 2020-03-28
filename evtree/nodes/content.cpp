/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "content.hpp"

#include <boost/algorithm/string.hpp>

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

cppjinja::evt::render_info cppjinja::evtnodes::content::rinfo() const
{
	return {false, false};
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
	auto ri = ctx.cur_render_info();
	if(!ri) ctx.out() << cnt;
	else {
		auto trimmed = cnt;
		if(ri->trim_left) boost::trim_left(trimmed);
		if(ri->trim_right) boost::trim_right(trimmed);
		ctx.out() << trimmed;
	}
}
