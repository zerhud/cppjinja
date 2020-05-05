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
	void add_var_name(evt::exenv& env, ast::var_name name) const ;
	void inject_value(evt::exenv& env) const ;
	void inject_object(evt::exenv& env) const ;
	void inject_call_object(evt::exenv& env) const ;
	void inject_array_calls(evt::exenv& env) const ;
public:
	op_set(ast::op_set o);

	evt::render_info rinfo() const override ;
	void render( evt::exenv& env ) const override ;
};

} // namespace cppjinja::evtnodes

