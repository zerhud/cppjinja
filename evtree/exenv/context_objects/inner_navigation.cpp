/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "inner_navigation.hpp"

using cppjinja::evt::context_objects::inner_navigation;

inner_navigation::inner_navigation(exenv *e)
    : env(e)
{
	if(!env) throw std::runtime_error(
	            "cannot create inner navigation obejct "
	            "without environment object");
}

void inner_navigation::add(east::string_t n, std::shared_ptr<context_object> child)
{
	throw std::logic_error("cannot add somthing to inner navigation object");
}

std::shared_ptr<context_object> inner_navigation::find(east::var_name n) const
{
	return nullptr;
}

absd::data inner_navigation::solve() const {}
std::shared_ptr<context_object> inner_navigation::call(
    std::pmr::vector<function_parameter> params) const {}
