/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <string>
#include <vector>
#include "parser/ast/tmpls.hpp"

namespace cppjinja {

class data_provider {
public:
	virtual ~data_provider() noexcept =default ;

	virtual std::string solve(const ast::var_name& val) const =0 ;
	virtual std::string solve(const ast::function_call& val) const =0 ;
};

class evaluator {
	std::vector<ast::tmpl> tmpls_;
	ast::tmpl* tmpl_ = nullptr;
	mutable const data_provider* data_ = nullptr;
	mutable std::vector<const std::vector<ast::block_content>*> data_stack_;

	const ast::block_content* search_by_name(const ast::var_name& name) const ;

	std::string render(const ast::block_content& cnt, bool render_data) const ;
	void render(std::ostream& to, const std::vector<ast::block_content>& cnt) const ;
	std::string render(const ast::value_term& val, const std::vector<ast::filter_call>& filters) const ;
	std::string render(const ast::var_name& var) const ;
	std::string render(const ast::function_call& var) const ;
	std::string render(const ast::binary_op& var) const ;
	std::string render(const std::string& base, const ast::filter_call& filter) const ;
public:
	evaluator(std::vector<ast::tmpl> tmpls);
	void render(std::ostream& to, const data_provider& data) const ;
};

} // namespace cppjinja
