/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "callable.hpp"

namespace cppjinja::evtnodes {

class block_named : public callable {
	ast::block_named block;
	bool has_nondefaulted_params() const ;
	[[nodiscard]] evt::raii_result_format result_format_raii(evt::exenv& env) const ;
public:
	block_named(ast::block_named nb);
	evt::render_info rinfo() const override ;
	ast::string_t name() const override ;
	void render(evt::exenv& env) const override ;

	east::string_t evaluate( evt::exenv& env) const override ;

	std::vector<ast::macro_parameter> params() const override ;
};

} // namespace cppjinja::evtnodes
