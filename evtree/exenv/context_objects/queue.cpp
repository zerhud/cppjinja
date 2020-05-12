/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "queue.hpp"

using namespace std::literals;

cppjinja::evt::context_objects::queue::queue()
{
}

cppjinja::evt::context_objects::queue::queue(std::initializer_list<cppjinja::evt::context_objects::queue> cnt)
{
	for(auto& c:cnt) for(auto& oc:c.content) content.emplace_back(oc);
	for(auto& c:cnt) for(auto& oc:c.const_content) const_content.emplace_back(oc);
}

cppjinja::evt::context_objects::queue::queue(std::initializer_list<context_object*> cnt)
    : queue(std::vector<cppjinja::evt::context_object*>(std::move(cnt)))
{
}

cppjinja::evt::context_objects::queue::queue(std::initializer_list<const cppjinja::evt::context_object*> cnt)
    : queue(std::vector<const cppjinja::evt::context_object*>(std::move(cnt)))
{
}

cppjinja::evt::context_objects::queue::queue(std::vector<context_object*> cnt)
    : content(std::move(cnt))
{
}

cppjinja::evt::context_objects::queue::queue(std::vector<const cppjinja::evt::context_object*> cnt)
    : const_content(std::move(cnt))
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
	if(content.empty()) throw std::runtime_error("cannot add empty child");
	content.front()->add(std::move(n), std::move(child));
}

std::shared_ptr<cppjinja::evt::context_object> cppjinja::evt::context_objects::queue::find(cppjinja::east::var_name n) const
{
	for(auto& c:content) if(auto f = c->find(n);f) return f;
	for(auto& c:const_content) if(auto f = c->find(n);f) return f;
	return nullptr;
}

cppjinja::east::value_term cppjinja::evt::context_objects::queue::solve() const
{
	throw std::runtime_error("cannot solve in queue context object");
}

cppjinja::json cppjinja::evt::context_objects::queue::jval() const
{
	throw std::runtime_error("cannot solve in queue context object");
}

std::shared_ptr<cppjinja::evt::context_object> cppjinja::evt::context_objects::queue::call(
        std::vector<cppjinja::east::function_parameter> params) const
{
	(void)params;
	throw std::runtime_error("cannot call in queue context object");
}
