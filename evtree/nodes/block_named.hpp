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

class block_named : public node {
	ast::block_named block;
public:
	block_named(ast::block_named nb);
	render_info rinfo() const override ;
	ast::string_t name() const override ;
	bool is_leaf() const override ;
	void render(
	          std::ostream& out
	        , const evtree& tree
	        , evt::context& ctx
	        ) const override ;
};

} // namespace cppjinja::evtnodes
