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
public:
	using path_solver = std::function<std::filesystem::path(std::filesystem::path)>;
private:
	path_solver solver_;
	std::vector<std::filesystem::path> cur_dir_stack_;
	std::list<ast::file> files_;
	ast::file* cur_file_ = nullptr;
	ast::string_t cur_file_name_;

	std::filesystem::path solve_path(const std::filesystem::path& to) const ;
	void parse(const ast::op_include& inc);
	void parse(ast::op_import& imp);
public:
	parser(const parser&) =delete ;
	parser& operator = (const parser&) =delete ;

	/// create parser with default solver
	/// (cannot actualy solve anything)
	parser();

	/// @param ps functor for solve path, must returns
	/// absolute path, receive relative. the pricority of
	/// include is second, first is the same directory.
	parser(path_solver ps);

	parser& parse(std::filesystem::path file);
	parser& parse(std::istream& data);

	std::vector<ast::tmpl> tmpls() const ;
	std::vector<ast::file> files() const ;
};

} // namespace cppjinja
