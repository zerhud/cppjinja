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

class op_out : public evt::node {
	ast::op_out op;
public:
	op_out(ast::op_out o);

	evt::render_info rinfo() const override ;
	void render( evt::exenv& env ) const override ;
};

} // namespace cppjinja::evtnodes
