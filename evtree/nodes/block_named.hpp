/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "block_with_name.hpp"

namespace cppjinja::evtnodes {

class block_named : public block_with_name {
	ast::block_named block;

	bool has_nondefaulted_params() const ;
	const ast::block_with_name& cur_ast() const override ;
public:
	block_named(ast::block_named nb);
	void render(evt::exenv& env) const override ;
	absd::data evaluate( evt::exenv& env) const override ;
};

} // namespace cppjinja::evtnodes
