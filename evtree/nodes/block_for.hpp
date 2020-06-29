/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "../node.hpp"

namespace cppjinja::evtnodes {

class block_for : public evt::node {
	ast::block_for abl;
public:
	block_for(ast::block_for ast_bl);
	evt::render_info rinfo() const override ;
	void render( evt::exenv& env ) const override ;
};

} // namespace cppjinja::evtnodes
