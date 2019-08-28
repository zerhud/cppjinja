/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "ast.hpp"

std::ostream& cppjinja::operator << (std::ostream& out, const cppjinja::var_name& obj)
{
	for(auto& i:obj) out << i << '.';
	return out;
}

std::ostream& cppjinja::operator << (std::ostream& out, const cppjinja::fnc_call& obj)
{
	out << obj.ref << '(';
	auto printer = overloaded{
		[&out](const auto& i){out << i;},
		[&out](const boost::recursive_wrapper<fnc_call>& i){out << i.get();}
	};
	for(auto& i:obj.params) {
		std::visit(printer,i);
		out << ',';
	}
	out << ')';
	return out;
}

