/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "../node.hpp"

namespace cppjinja::evtnodes {

class block_macro : public evt::node {
	ast::block_macro block;
public:
	block_macro(ast::block_macro nb);
	render_info rinfo() const override ;
	ast::string_t name() const override ;
	bool is_leaf() const override ;
	void render(evt::context& ctx) const override ;
};

} // namespace cppjinja::evtnodes
