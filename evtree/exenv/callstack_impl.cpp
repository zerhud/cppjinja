/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/


#include "callstack_impl.hpp"
#include "evtree/nodes/callable.hpp"

void cppjinja::evt::callstack_impl::require_stack_is_not_empty() const
{
	if(stack.empty())
		throw std::runtime_error("stack is empty");
}

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

void cppjinja::evt::callstack_impl::pop()
{
	require_stack_is_not_empty();
	stack.pop_back();
}

void cppjinja::evt::callstack_impl::push(const evtnodes::callable* calling)
{
	if(calling == nullptr) throw std::runtime_error("cannot push nullptr calling");
	stack.emplace_back(frame{calling, {}});
}

cppjinja::east::string_t cppjinja::evt::callstack_impl::call(exenv* env,
        const cppjinja::evtnodes::callable* calling,
        std::vector<cppjinja::ast::function_call_parameter> params)
{
	struct pushpoper {
		callstack_impl* self;
		pushpoper(callstack_impl* s, const evtnodes::callable* c)
		    : self(s)
		{
			self->push(c);
		}
		~pushpoper()
		{
			self->pop();
		}
	} raii(this, calling);
	call_params(std::move(params));
	auto ret = calling->evaluate(*env);
	return ret;
}

std::vector<const cppjinja::evtnodes::callable*>
cppjinja::evt::callstack_impl::calling_stack() const
{
	require_stack_is_not_empty();
	std::vector<const cppjinja::evtnodes::callable*> ret;
	for(auto item = stack.rbegin();item!=stack.rend();++item)
		ret.emplace_back(item->calling);
	return ret;
}

std::shared_ptr<cppjinja::evtnodes::callable_multisolver>
cppjinja::evt::callstack_impl::make_params(exenv* env,
        const std::vector<cppjinja::ast::function_call_parameter>& params) const
{
	require_stack_is_not_empty();
	auto cparams = stack.back().calling->params();
	auto ret = std::make_shared<evtnodes::callable_multisolver>(env);
	for(auto& p:params) if(p.name.has_value()) ret->add(*p.name, p.value.get());
	for(auto& p:cparams) if(p.value.has_value()) ret->add(p.name, *p.value);
	for(std::size_t i=0;i<cparams.size();++i) {
		auto val = position_param(params, i);
		if(val) ret->add(cparams[i].name, *val);
	}
	return ret;
}

std::vector<cppjinja::ast::function_call_parameter>
cppjinja::evt::callstack_impl::call_params() const
{
	require_stack_is_not_empty();
	return stack.back().cparams;
}

void cppjinja::evt::callstack_impl::call_params(
        std::vector<cppjinja::ast::function_call_parameter> params)
{
	require_stack_is_not_empty();
	stack.back().cparams = params;
}
