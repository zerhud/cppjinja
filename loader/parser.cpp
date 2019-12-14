/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <fstream>
#include <boost/spirit/include/support_istream_iterator.hpp>

#include "parser.hpp"
#include "parser/parse.hpp"

#include "parser/grammar/tmpls.hpp"

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
	boost::spirit::istream_iterator end;
	boost::spirit::istream_iterator begin(data);

	auto tmpl = cppjinja::text::parse(text::tmpl, begin, end);
	//tmpls_.emplace_back(std::move(tmpl));

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

