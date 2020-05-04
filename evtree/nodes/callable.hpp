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

class callable : virtual public evt::named_node {
public:
	virtual east::string_t evaluate(evt::exenv& env) const =0 ;
	virtual std::vector<east::function_parameter> solved_params(evt::exenv& env) const =0 ;
};

} // namespace cppjinja::evtnodes

