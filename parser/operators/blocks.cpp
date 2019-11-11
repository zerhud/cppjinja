/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <boost/spirit/home/karma.hpp>

#include "blocks.hpp"
#include "common.hpp"

template<typename T>
static std::string block_trim_to_str(const T& oc)
{
	return oc.trim ? "+" : "-";
}

static std::string open_block(const cppjinja::ast::block& obj)
{
	return block_trim_to_str(obj.left_open) + block_trim_to_str(obj.left_close);
}

static std::string close_block(const cppjinja::ast::block& obj)
{
	return block_trim_to_str(obj.right_open) + block_trim_to_str(obj.right_close);
}

std::ostream& std::operator << (std::ostream& out, const cppjinja::ast::block_raw& obj)
{
	return out << "raw: " << obj.value;
}

std::ostream& std::operator << (std::ostream& out, const cppjinja::ast::block_for& obj)
{
	using namespace boost::spirit::karma;
	return out
		<< open_block(obj) << "for" << close_block(obj) << ": "
		<< format( *char_ % '.', obj.vars)
		<< " in "
		<< obj.value.var
		;
}

std::ostream& std::operator << (std::ostream& out, const cppjinja::ast::block_macro& obj)
{
	return out << "macro: " << obj.name;
}
