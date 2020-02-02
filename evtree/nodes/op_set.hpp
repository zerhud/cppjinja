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

class op_set : public evt::node {
	ast::op_set op;
public:
	op_set(ast::op_set o);

	render_info rinfo() const override ;
	ast::string_t name() const override ;
	bool is_leaf() const override ;
	void render( evt::context& ctx ) const override ;
};

} // namespace cppjinja::evtnodes

