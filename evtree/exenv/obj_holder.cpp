/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/


#include "obj_holder.hpp"
#include "ctx_object.hpp"

using namespace std::literals;


cppjinja::evt::obj_holder::~obj_holder() noexcept
{
}

void cppjinja::evt::obj_holder::add(cppjinja::ast::string_t n, std::shared_ptr<cppjinja::evt::ctx_object> o)
{
	map[n] = std::move(o);
}

void cppjinja::evt::obj_holder::rem(const cppjinja::ast::string_t& n)
{
	auto pos = map.find(n);
	if(pos==map.end())
		throw std::runtime_error("cannot remove "s + n);
	map.erase(pos);
}

std::optional<cppjinja::ast::value_term>
cppjinja::evt::obj_holder::call(cppjinja::ast::function_call fnc) const
{
	auto pos = map.find(fnc.ref[0]);
	if(pos!=map.end()){
		fnc.ref.erase(fnc.ref.begin());
		return pos->second->call(fnc);
	}
	return std::nullopt;
}

std::optional<cppjinja::ast::value_term> cppjinja::evt::obj_holder::solve(cppjinja::ast::var_name var) const
{
	auto pos = map.find(var[0]);
	if(pos!=map.end()){
		var.erase(var.begin());
		return pos->second->solve(var);
	}
	return std::nullopt;
}
