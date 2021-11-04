/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "value.hpp"

using namespace std::literals;

cppjinja::evt::context_objects::value::value(absd::data j)
    : src(std::move(j))
{
}

cppjinja::evt::context_objects::value::~value() noexcept
{
}

void cppjinja::evt::context_objects::value::add(
          cppjinja::east::string_t n
        , std::shared_ptr<cppjinja::evt::context_object> child)
{
	(void)n;
	(void)child;
	throw std::runtime_error("cannot add a child to a value");
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::value::find(cppjinja::east::var_name n) const
{
	if(n.size()==1) {
		if(src.is_object()) return std::make_shared<value>(src[n[0]]);
		if(src.is_array()) return std::make_shared<value>(src[std::stoi(n[0].c_str())]);
	}

	std::string vn;
	for(auto& v:n) vn += v + '.';
	if(2 <= vn.size()) vn.erase(vn.size()-1, 1);
	throw std::runtime_error("cannot find a child in a value [" + vn + "]");
}

absd::data cppjinja::evt::context_objects::value::solve() const
{
	return src;
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::value::call(
        std::pmr::vector<function_parameter> params) const
{
	(void)params;
	throw std::runtime_error("cannot call a value");
}
