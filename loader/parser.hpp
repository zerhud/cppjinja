/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <list>
#include <vector>
#include <string>
#include <filesystem>
#include "parser/ast/tmpls.hpp"

namespace cppjinja {

class parser final {
	std::vector<std::filesystem::path> cur_dir_stack_;
	std::vector<std::filesystem::path> incs_;
	std::list<ast::file> files_;
	ast::file* cur_file_ = nullptr;
	ast::string_t cur_file_name_;

	std::filesystem::path solve_path(const std::filesystem::path& to) const ;
	void parse(const ast::op_include& inc);
	void parse(ast::op_import& imp);
public:
	parser(const parser&) =delete ;
	parser& operator = (const parser&) =delete ;

	/// @param inc_dirs include directories for search
	/// includes, extends and so on... the pricority of
	/// include is second, first is the same directory.
	parser(std::vector<std::filesystem::path> inc_dirs);

	parser& parse(std::filesystem::path file);
	parser& parse(std::istream& data);

	std::vector<ast::tmpl> tmpls() const ;
	std::vector<ast::file> files() const ;
};

} // namespace cppjinja
