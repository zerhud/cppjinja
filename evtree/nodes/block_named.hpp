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

class block_named : public callable {
	ast::block_named block;
	using callable::param;
public:
	block_named(ast::block_named nb);
	evt::render_info rinfo() const override ;
	ast::string_t name() const override ;
	bool is_leaf() const override ;
	void render(evt::context& ctx) const override ;

	std::optional<ast::value_term> param(
	          evt::context& ctx
	        , const ast::var_name& name
	        ) const override ;
};

} // namespace cppjinja::evtnodes
