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

class callable : virtual public evt::node {
protected:
	std::optional<ast::value_term> param(
	          const std::vector<ast::macro_parameter>& params
	        , const ast::var_name& name
	        , evt::context& ctx) const ;
public:
	virtual bool render_param(
			  evt::context& ctx
			, const ast::var_name& pname
			) const =0 ;

	virtual std::optional<ast::value_term> param(
	          evt::context& ctx
	        , const ast::var_name& name) const =0 ;
};

} // namespace cppjinja::evtnodes

