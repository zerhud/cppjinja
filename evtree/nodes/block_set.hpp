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

class block_set : public evt::node {
	ast::block_set block;
	evt::render_info inner_ri() const ;
public:
	block_set(ast::block_set nb);
	evt::render_info rinfo() const override ;
	void render(evt::exenv& env) const override ;
};

} // namespace cppjinja::evtnodes
