/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "debug_operators.hpp"

#include <iomanip>

std::ostream& cppjinja::operator << (std::ostream& out, comparator obj)
{
	if(obj==comparator::eq) out << "==";
	else if(obj==comparator::less) out << "<";
	else if(obj==comparator::more) out << ">";
	else if(obj==comparator::less_eq) out << "=<";
	else if(obj==comparator::more_eq) out << ">=";
	else if(obj==comparator::no) ;
	else assert(false);
	return out;
}

std::ostream& cppjinja::operator << (std::ostream& out, const cppjinja::var_name& obj)
{
	for(auto& i:obj) out << i << '.';
	return out;
}

std::ostream& cppjinja::operator << (std::ostream& out, const st_raw& obj)
{
	out << "[raw " << &obj << ']';
	return out;
}

std::ostream& cppjinja::operator << (std::ostream& out, const macro_parameter& obj)
{
	out << obj.name;
	if(obj.value_t == macro_parameter::type::str) out << "=" << std::quoted(obj.value);
	else if(obj.value_t != macro_parameter::type::no) out << "=" << obj.value;
	return out;
}

std::ostream& cppjinja::operator << (std::ostream& out, const st_macro& obj)
{
	out << "macro: " << obj.name << '(';
	for(auto& p:obj.params) out << p << ',';
	out << ')';
	return out;
}
