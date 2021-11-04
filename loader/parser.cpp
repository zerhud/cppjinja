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

std::filesystem::path cppjinja::parser::solve_path(const std::filesystem::path& to) const
{
	using namespace std::filesystem;

	if(to.is_absolute()) return to;

	for(auto& d:cur_dir_stack_) if(exists(d/to)) return d / to;
	for(auto& d:incs_)          if(exists(d/to)) return d / to;

	return absolute(to);
}

void cppjinja::parser::parse(const ast::op_include& inc)
{
	std::filesystem::path fn = inc.filename;
	fn = solve_path(fn);
	assert(fn.is_absolute());

	if(inc.ignore_missing && *inc.ignore_missing) {
		if(!std::filesystem::exists(fn)) return;
	}

	//TODO: with or without context ignored for now

	parse(fn);
}

void cppjinja::parser::parse(ast::op_import& imp)
{
	std::filesystem::path fn = solve_path(imp.filename);
	assert(fn.is_absolute());
	imp.filename = fn;
	parse(fn);
}

cppjinja::parser::parser(std::vector<std::filesystem::path> inc_dirs)
    : incs_(std::move(inc_dirs))
{
}

cppjinja::parser& cppjinja::parser::parse(std::filesystem::path file)
{
	if(!std::filesystem::exists(file))
		throw std::runtime_error("file " + file.string() + " doesn't exists");

	cur_file_ = &files_.emplace_back();
	cur_file_->name = file.generic_string();
	cur_file_name_ = absolute(file).generic_string();

	std::fstream fdata(file);

	cur_dir_stack_.push_back(file.parent_path());
	parse(fdata);
	cur_dir_stack_.pop_back();

	return *this;
}

cppjinja::parser& cppjinja::parser::parse(std::istream& data)
{
	data >> std::noskipws;
	boost::spirit::istream_iterator end;
	boost::spirit::istream_iterator begin(data);

	if(!cur_file_) cur_file_ = &files_.emplace_back();
	*cur_file_ = cppjinja::text::parse(text::file, begin, end);
	if(!cur_file_name_.empty())
		cur_file_->name = cur_file_name_;

	for(auto& imp:cur_file_->imports) imp.filename = solve_path(imp.filename);

	if(cur_file_->tmpls.size() == 1 && cur_file_->tmpls[0].name.empty())
		cur_file_->tmpls[0].name = cur_file_->name;
	for(auto& tmpl:cur_file_->tmpls) {
		tmpl.file_name = cur_file_->name;
		tmpl.file_imports = cur_file_->imports;
	}

	for(auto& imp:cur_file_->imports) parse(imp);
	for(auto& inc:cur_file_->includes) parse(inc);

	return *this;
}

std::vector<cppjinja::ast::tmpl> cppjinja::parser::tmpls() const
{
	std::vector<cppjinja::ast::tmpl> tmpls;
	for(auto& f:files_) for(auto& t:f.tmpls) tmpls.emplace_back(t);
	return tmpls;
}

std::vector<cppjinja::ast::file> cppjinja::parser::files() const
{
	std::vector<cppjinja::ast::file> ret;
	for(auto& f:files_) ret.emplace_back(f);
	return ret;
}

