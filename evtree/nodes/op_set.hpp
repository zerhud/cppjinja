/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "../node.hpp"
#include "evtree/exenv/ctx_object.hpp"

namespace cppjinja::evtnodes {

class op_set : public evt::node {
	ast::op_set op;
	void inject_value(evt::exenv& env) const ;
	void inject_object(evt::exenv& env) const ;
public:
	op_set(ast::op_set o);

	evt::render_info rinfo() const override ;

	bool is_leaf() const override ;
	void render( evt::exenv& env ) const override ;
};

} // namespace cppjinja::evtnodes

