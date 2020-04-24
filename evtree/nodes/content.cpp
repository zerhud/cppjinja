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

cppjinja::evt::render_info cppjinja::evtnodes::content::rinfo() const
{
	return {false, false};
}


void cppjinja::evtnodes::content::render(evt::exenv& env) const
{
	env.current_node(this);
	auto ri = env.rinfo();
	if(!ri) env.out() << cnt;
	else {
		auto trimmed = cnt;
		if(ri->trim_left) boost::trim_left(trimmed);
		if(ri->trim_right) boost::trim_right(trimmed);
		env.out() << trimmed;
	}
}
