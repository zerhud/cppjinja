/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/


#include "callstack_impl.hpp"
#include "evtree/nodes/callable.hpp"

std::optional<cppjinja::ast::value_term>
cppjinja::evt::callstack_impl::position_param(
          const std::vector<cppjinja::ast::function_call_parameter>& params
        , std::size_t ind) const
{
	for(std::size_t i=0;i<params.size();++i) {
		if(params[i].name.has_value()) return std::nullopt;
		if(i==ind) return params[i].value.get();
	}
	return std::nullopt;
}

cppjinja::east::string_t cppjinja::evt::callstack_impl::call(exenv* env,
        const cppjinja::evtnodes::callable* calling,
        std::vector<cppjinja::ast::function_call_parameter> params)
{
	if(!env || !calling) throw std::runtime_error("cannot call with nullptrs");
	struct pushpoper {
		callstack_impl* self;
		pushpoper(callstack_impl* s, const evtnodes::callable* c)
		    : self(s)
		{
			self->stack.emplace_back(frame{c, obj_holder()});
		}
		~pushpoper()
		{
			self->stack.pop_back();
		}
	} raii(this, calling);
	make_params_holder(std::move(params));
	return calling->evaluate(*env);
}

std::vector<const cppjinja::evt::obj_holder*>
cppjinja::evt::callstack_impl::param_stack(const cppjinja::evt::node* last) const
{
	std::vector<const cppjinja::evt::obj_holder*> ret;
	for(auto pos = stack.rbegin();pos!=stack.rend();++pos) {
		ret.emplace_back(&pos->params);
		if(pos->calling == last) return ret;
	}
	if(!stack.empty())
		throw std::runtime_error("no last parameter found");
	return ret;
}

void cppjinja::evt::callstack_impl::make_params_holder(std::vector<cppjinja::ast::function_call_parameter> params)
{
	obj_holder& hld = stack.back().params;
	auto cparams = stack.back().calling->params();
	for(auto& p:cparams) if(p.value.has_value()) hld.add(p.name, std::make_unique<var_solver>(*p.value));
	for(auto& p:params) if(p.name.has_value()) hld.add(*p.name, std::make_unique<var_solver>(p.value.get()));
	for(std::size_t i=0;i<cparams.size();++i) {
		auto val = position_param(params, i);
		if(val) hld.add(cparams[i].name, std::make_unique<var_solver>(*val));
	}
}
