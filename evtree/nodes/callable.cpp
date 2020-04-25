/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "callable.hpp"
#include "exenv/callstack.hpp"
#include "exenv/raii.hpp"

std::optional<cppjinja::ast::value_term>
cppjinja::evtnodes::callable::param(
          const std::vector<cppjinja::ast::macro_parameter>& params
        , const ast::var_name& name
        , const evt::callstack& ctx
        ) const
{
	if(name.size()!=1) return std::nullopt;
	for(std::size_t outer = 0;outer<params.size();++outer)
	{
		auto& p = params[outer];
		if(p.name != name[0]) continue;

		auto ctx_params = ctx.call_params();
		for(std::size_t inner = 0;inner<ctx_params.size();++inner)
		{
			auto& cp = ctx_params[inner];
			bool found = cp.name.has_value() && *cp.name == name[0];
			if(!found && !cp.name) found = outer == inner;
			if(found) return cp.value.get();
		}

		if(p.value) return p.value;
		else throw std::runtime_error("requried parameter not specified");
	}

	return std::nullopt;
}

cppjinja::evtnodes::callable_solver::callable_solver(cppjinja::evt::exenv* e, const cppjinja::evtnodes::callable* b)
    : env(e)
    , block(b)
{
}

cppjinja::evtnodes::callable_solver::~callable_solver() noexcept
{
}

cppjinja::ast::value_term
cppjinja::evtnodes::callable_solver::call(cppjinja::ast::function_call fnc)
{
	if(!fnc.ref.empty())
		throw std::runtime_error("no such function");
	return ast::value_term{env->calls().call(env, block, std::move(fnc.params))};
}

cppjinja::ast::value_term
cppjinja::evtnodes::callable_solver::solve(cppjinja::ast::var_name)
{
	throw std::runtime_error("cannot solve callable block");
}

cppjinja::evtnodes::callable_multisolver::callable_multisolver(cppjinja::evt::exenv* e)
    : env(e)
{
}

cppjinja::evtnodes::callable_multisolver::~callable_multisolver() noexcept
{
}

void cppjinja::evtnodes::callable_multisolver::add(
          cppjinja::ast::string_t n
        , const callable* o)
{
	objs[n] = o;
}

void cppjinja::evtnodes::callable_multisolver::add(
        cppjinja::ast::string_t n, cppjinja::ast::value_term v)
{
	values[n] = std::move(v);
}

cppjinja::ast::value_term
cppjinja::evtnodes::callable_multisolver::call(cppjinja::ast::function_call fnc)
{
	if(fnc.ref.size()!=1) throw std::runtime_error("no such function");
	auto pos = objs.find(fnc.ref[0]);
	if(pos == objs.end()) throw std::runtime_error("no such function");

	return ast::value_term{env->calls().call(env, pos->second, std::move(fnc.params))};
}

cppjinja::ast::value_term
cppjinja::evtnodes::callable_multisolver::solve(cppjinja::ast::var_name var)
{
	if(var.size()!=1) throw std::runtime_error("no such variable");
	auto pos = values.find(var[0]);
	if(pos==values.end()) throw std::runtime_error("no such variable");
	return pos->second;
}
