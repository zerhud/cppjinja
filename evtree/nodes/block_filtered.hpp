/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "../node.hpp"

namespace cppjinja::evtnodes {

class block_filtered : public evt::node {
	ast::block_filtered ast;
public:
	block_filtered(ast::block_filtered nb);
	void render(evt::exenv& env) const override ;
	evt::render_info rinfo() const override ;
};

} // namespace cppjinja::evtnodes
