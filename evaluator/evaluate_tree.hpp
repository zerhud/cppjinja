/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include <ostream>

#include "eval.hpp"
#include "tree/tmpl.hpp"

namespace cppjinja {

class evaluate_tree
{
	std::vector<eval_tree::tmpl> collection;
public:
	evaluate_tree(std::vector<ast::tmpl> tmpls);

	void render(std::ostream& to, const data_provider& data, const ast::string_t& name) const ;

	std::vector<const eval_tree::tmpl*> all_tmpls() const ;
};

} // namespace cppjinja
