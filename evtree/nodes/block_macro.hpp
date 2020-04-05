/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "callable.hpp"

namespace cppjinja::evtnodes {

class block_macro : public callable {
	ast::block_macro block;
	evt::render_info inner_ri() const ;
public:
	block_macro(ast::block_macro nb);
	evt::render_info rinfo() const override ;
	ast::string_t name() const override ;
	bool is_leaf() const override ;
	void render(evt::context& ctx) const override ;

	std::optional<ast::value_term> param(
	          const evt::context& ctx
	        , const ast::var_name& name) const override ;
};

} // namespace cppjinja::evtnodes
