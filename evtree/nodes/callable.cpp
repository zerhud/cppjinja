/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "callable.hpp"

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
