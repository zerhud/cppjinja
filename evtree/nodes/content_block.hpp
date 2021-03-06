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

class content_block : public evt::node {
	evt::render_info rinfo_;
public:
	content_block(evt::render_info ri);
	evt::render_info rinfo() const override ;
	void render(evt::exenv& ctx) const override ;
};

} // namespace cppjinja::evtnodes
