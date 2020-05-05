/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/


#include "callstack_impl.hpp"
#include "evtree/nodes/callable.hpp"

const cppjinja::evt::context_objects::queue cppjinja::evt::callstack_impl::current_params(const node* last) const
{
	std::vector<const context_object*> cnt;
	for(auto pos=stack.rbegin();pos!=stack.rend();++pos) {
		cnt.emplace_back(&pos->param_object.value());
		if(pos->calling == last) return cnt;
	}
	throw std::runtime_error("cannot find last node in callstack");
}

void cppjinja::evt::callstack_impl::pop()
{
	stack.pop_back();
}

void cppjinja::evt::callstack_impl::push(
          const cppjinja::evtnodes::callable* calling
        , context_objects::callable_params params)
{
	if(!calling) throw std::runtime_error("cannot push nullptr calling");
	stack.emplace_back(frame{calling, std::move(params)});
}
