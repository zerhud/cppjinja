/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/


#include "callstack.hpp"

void cppjinja::evt::callstack::require_stack_is_not_empty() const
{
	if(stack.empty())
		throw std::runtime_error("stack is empty");
}

void cppjinja::evt::callstack::pop(const cppjinja::evt::node* caller)
{
	require_stack_is_not_empty();
	if(stack.back().caller != caller)
		throw std::runtime_error("stack back and pop argument are missmatch");
	stack.pop_back();
}

void cppjinja::evt::callstack::push(
          const cppjinja::evt::node* caller
        , const evtnodes::callable* calling)
{
	if(caller == nullptr) throw std::runtime_error("cannot push nullptr caller");
	if(calling == nullptr) throw std::runtime_error("cannot push nullptr calling");
	stack.emplace_back(frame{caller, calling, {}});
}

const cppjinja::evt::node* cppjinja::evt::callstack::caller() const
{
	require_stack_is_not_empty();
	return stack.back().caller;
}

std::vector<const cppjinja::evtnodes::callable*>
cppjinja::evt::callstack::calling_stack() const
{
	require_stack_is_not_empty();
	std::vector<const cppjinja::evtnodes::callable*> ret;
	for(auto item = stack.rbegin();item!=stack.rend();++item)
		ret.emplace_back(item->calling);
	return ret;
}

std::vector<cppjinja::ast::function_call_parameter>
cppjinja::evt::callstack::call_params() const
{
	require_stack_is_not_empty();
	return stack.back().cparams;
}

void cppjinja::evt::callstack::call_params(
        std::vector<cppjinja::ast::function_call_parameter> params)
{
	require_stack_is_not_empty();
	stack.back().cparams = params;
}

cppjinja::evt::raii_push_callstack::raii_push_callstack(
          const cppjinja::evt::node* n
        , const evtnodes::callable* c
        , cppjinja::evt::callstack* m
        )
    : ctx(m)
    , maker(n)
{
	ctx->push(maker, c);
}

cppjinja::evt::raii_push_callstack::~raii_push_callstack()
{
	ctx->pop(maker);
}
