/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <fstream>

#include "parser.hpp"
#include "parse.hpp"

#include "grammar/tmpls.hpp"

cppjinja::parser::parser(std::vector<std::filesystem::path> inc_dirs) : incs_(std::move(inc_dirs))
{
}

cppjinja::parser& cppjinja::parser::parse(std::filesystem::path file)
{
	if(!std::filesystem::exists(file))
		throw std::runtime_error("file " + file.string() + " doesn't exists");

	std::fstream fdata(file);
	return parse(fdata);
}

cppjinja::parser& cppjinja::parser::parse(std::istream& data)
{
	(void) data;
	//std::istream_iterator<char> end;
	//std::istream_iterator<char> begin(data);

	//auto tmpl = cppjinja::text::parse(text::tmpl, begin, end)

	return *this;
}

std::vector<cppjinja::ast::tmpl> cppjinja::parser::tmpls() const
{
	return {};
}

std::vector<cppjinja::ast::file> cppjinja::parser::files() const
{
	return {};
}

