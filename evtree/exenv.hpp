/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "declarations.hpp"
#include "exenv/context.hpp"
#include "exenv/callstack.hpp"
#include "eval/eval.hpp"

namespace cppjinja::evt {

struct render_info
{
	bool trim_left:4;
	bool trim_right:4;
};

class exenv final {
	const evtree* compiled_template;
	const data_provider* user_data;
	std::ostream& ostream;
	context exectx;
	callstack execalls;
	std::optional<render_info> cur_rinfo;
public:
	exenv(const data_provider* prov, const evtree* tmpl, std::ostream& out);
	const evtree& tmpl() const ;
	const data_provider* data() const ;
	std::ostream& out() ;

	context& ctx() ;
	const context& ctx() const ;

	callstack& calls() ;
	const callstack& calls() const ;
	std::optional<render_info> rinfo() const ;
	void rinfo(std::optional<render_info> ri) ;
};

} // namespace cppjinja::evt
