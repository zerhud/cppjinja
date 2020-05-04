/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

// need to include some file for work with streams
// if not crashes when print double to std::ostream&
#include <iomanip>
#include "ast.hpp"

cppjinja::east::array_v::array_v(const cppjinja::east::array_v& other)
{
	for(auto& i:other.items)
		items.emplace_back(std::make_unique<value_term>(*i));
}

cppjinja::east::array_v& cppjinja::east::array_v::operator =(
        const cppjinja::east::array_v& other)
{
	items.clear();
	for(auto& i:other.items)
		items.emplace_back(std::make_unique<value_term>(*i));
	return *this;
}

cppjinja::east::map_v::map_v(const cppjinja::east::map_v& other)
{
	for(auto&&[k,v]:other.items)
		items[k] = std::make_unique<value_term>(*v);
}

cppjinja::east::map_v& cppjinja::east::map_v::operator =(
        const cppjinja::east::map_v& other)
{
	items.clear();
	for(auto&&[k,v]:other.items)
		items[k] = std::make_unique<value_term>(*v);
	return *this;
}

bool cppjinja::east::operator ==(
        const cppjinja::east::map_v& left, const cppjinja::east::map_v& right)
{
	return left.items == right.items;
}

bool cppjinja::east::operator ==(
          const cppjinja::east::array_v& left
        , const cppjinja::east::array_v& right)
{
	if(left.items.size() != right.items.size()) return false;
	for(std::size_t i=0;i<left.items.size();++i)
		if(!(*left.items[i] == *right.items[i])) return false;
	return true;
}

std::ostream& cppjinja::east::operator << (
        std::ostream& out, const cppjinja::east::value_term& val)
{
	struct {
		std::ostream& out;
		void operator()(const double& v){out << v;}
		void operator()(const string_t& v){out << v;}
		void operator()(const array_v& v){
			for(auto& i:v.items) out << *i << " ";
		}
		void operator()(const map_v& v){
			auto print = [this](const auto& v){out << v;};
			for(auto& i:v.items) {
				std::visit(print, i.first);
				out << ':' << *i.second << ' ';
			}
		}
	} render{out};
	const value_term_var& stdval = val;
	std::visit(render, stdval);
	return out;
}

bool cppjinja::east::operator ==(
          const cppjinja::east::function_parameter& left
        , const cppjinja::east::function_parameter& right)
{
	return left.name == right.name && left.val == right.val;
}
