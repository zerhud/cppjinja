/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "queue.hpp"

using namespace std::literals;

cppjinja::evt::context_objects::queue::queue(
        std::initializer_list<const context_object*> cnt)
    : queue(std::vector<const cppjinja::evt::context_object*>(std::move(cnt)))
{
}

cppjinja::evt::context_objects::queue::queue(
        std::vector<const cppjinja::evt::context_object*> cnt)
    : content(cnt)
{

}

cppjinja::evt::context_objects::queue::~queue() noexcept
{

}

void cppjinja::evt::context_objects::queue::add(
          cppjinja::east::string_t n
        , std::shared_ptr<cppjinja::evt::context_object> child)
{
	if(!child) throw std::runtime_error("cannot add empty child");
	if(!n.empty()) throw std::runtime_error("cannot add named child");
	for(auto& c:content) if(c==child.get()) throw std::runtime_error("cannot add twice");
	content.push_back(child.get());
}

std::shared_ptr<cppjinja::evt::context_object> cppjinja::evt::context_objects::queue::find(cppjinja::east::var_name n) const
{
	for(auto& c:content) if(auto f = c->find(n);f) return f;
	return nullptr;
}

cppjinja::east::value_term cppjinja::evt::context_objects::queue::solve() const
{
	throw std::runtime_error("cannot solve in queue context object");
}

cppjinja::east::value_term cppjinja::evt::context_objects::queue::call(
        std::vector<cppjinja::east::function_parameter> params) const
{
	(void)params;
	throw std::runtime_error("cannot call in queue context object");
}
