/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "evtree.hpp"
#include "exenv/context.hpp"

namespace cppjinja::evt {

class exenv final {
	const evtree* compiled_template;
	const data_provider* user_data;
	std::ostream& ostream;
public:
	exenv(const data_provider* prov, const evtree* tmpl, std::ostream& out);
	const evtree& tmpl() const ;
	const data_provider* data() const ;
	std::ostream& out() ;

	east::value_term solve_value(const cppjinja::ast::value_term& val) const ;
};

} // namespace cppjinja::evt
