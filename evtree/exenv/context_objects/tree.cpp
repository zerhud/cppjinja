/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "tree.hpp"
#include "value.hpp"

#include <sstream>
#include <iomanip>
#include <cassert>

std::ostream& cppjinja::evt::context_objects::tree::print_solved_value(std::ostream& out, const cppjinja::evt::context_object& obj) const
{
	std::stringstream tmp;
	tmp << obj.solve();
	if(dynamic_cast<const value*>(&obj)) out << std::quoted(tmp.str());
	else out << tmp.str();
	return out;
}

std::ostream& cppjinja::evt::context_objects::tree::render_children_with_comma(std::ostream& out) const
{
	for(auto& child:children) {
		out << std::quoted(child.first) << ':';
		print_solved_value(out, *child.second) << ',';
	}
	return out;
}

void cppjinja::evt::context_objects::tree::add_child(
          cppjinja::east::string_t n
        , std::shared_ptr<cppjinja::evt::context_object> child)
{
	children[n] = std::move(child);
}

cppjinja::evt::context_objects::tree::tree()
{

}

cppjinja::evt::context_objects::tree::~tree() noexcept
{

}

void cppjinja::evt::context_objects::tree::add(
        east::string_t n, std::shared_ptr<cppjinja::evt::context_object> child)
{
	add_child(std::move(n), std::move(child));
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::tree::find(cppjinja::east::var_name n) const
{
	auto pos = children.find(n.at(0));
	if(pos==children.end()) return nullptr;
	if(n.size()==1) return pos->second;
	n.erase(n.begin());
	return pos->second->find(n);
}

absd::data cppjinja::evt::context_objects::tree::solve() const
{
	throw std::runtime_error("cannot solve tree");
//	using namespace std::literals;
//	std::stringstream out;
//	render_children_with_comma(out << '{') << '}';
//	auto ret = out.str();
//	return east::value_term(ret.size() == 2 ? ret : ret.erase(ret.size()-2, 1));
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::tree::call(std::pmr::vector<function_parameter> params) const
{
	(void)params;
	throw std::runtime_error("cannot call a tree");
}
