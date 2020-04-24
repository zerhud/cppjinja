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

class block_if : public evt::node {
	ast::block_if block;
	evt::render_info rinfo_for_children() const ;
public:
	block_if(ast::block_if nb);
	evt::render_info rinfo() const override ;
	bool is_leaf() const override ;
	void render(evt::exenv& ctx) const override ;
};

} // namespace cppjinja::evtnodes
