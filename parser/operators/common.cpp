/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "common.hpp"
#include <boost/spirit/home/karma.hpp>

std::ostream& std::operator << (std::ostream& out, const cppjinja::ast::var_name& obj)
{
	using namespace boost::spirit::karma;
	return out << format( *char_ % '.', obj);
}

std::ostream& std::operator << (std::ostream& out, const cppjinja::ast::function_call_parameter& obj)
{
	if(obj.name) out << *obj.name << '=';
	return out << obj.value;
}

std::ostream& std::operator << (std::ostream& out, const cppjinja::ast::function_call& obj)
{
	out << obj.ref << '(';
	//TODO: print params
	return out << ')';
}

std::ostream& std::operator << (std::ostream& out, const cppjinja::ast::binary_op& obj)
{
	return out << obj.left << obj.op << obj.right;
}

std::ostream& std::operator << (std::ostream& out, const boost::spirit::x3::forward_ast<cppjinja::ast::value_term>& obj)
{
	return out << (obj.get());
}

std::ostream& std::operator << (std::ostream& out, const cppjinja::ast::value_term& obj)
{
	out << obj.var;
	return out;
}

std::ostream& std::operator << (std::ostream& out, const cppjinja::ast::comparator& obj)
{
	if(obj == cppjinja::ast::comparator::eq) out << "==";
	else out << static_cast<int>(obj);
	return out;
}

