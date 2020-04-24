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

class tmpl : public evt::named_node {
	ast::tmpl itmpl_;
	ast::string_t render_resutl;
	void create_self_obj(evt::exenv* env) const ;
public:
	tmpl(ast::tmpl t);

	evt::render_info rinfo() const override ;

	ast::string_t name() const override ;
	bool is_leaf() const override ;
	void render(evt::exenv& env) const override ;
	ast::string_t rendered() const ;
};

}  // namespace cppjinja::evtnodes
