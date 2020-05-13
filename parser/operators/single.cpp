/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "single.hpp"

std::ostream& std::operator << (std::ostream& out, const cppjinja::ast::op_out& obj)
{
	return out << "out: " << obj.value;
}

std::ostream& std::operator << (std::ostream& out, const cppjinja::ast::op_comment& obj)
{
	return out << "comment: " << obj.value;
}

std::ostream& std::operator << (std::ostream& out, const cppjinja::ast::op_set& obj)
{
	return out << obj.value;
}

std::ostream& std::operator << (std::ostream& out, const cppjinja::ast::op_include& obj)
{
	return out << obj.filename;
}

