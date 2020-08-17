/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "../node.hpp"
#include "../exenv/context_object.hpp"

namespace cppjinja::evtnodes {

class block_for : public evt::node {
	ast::block_for abl;
	void eval_for(evt::exenv& env, json val) const ;
public:
	block_for(ast::block_for ast_bl);
	evt::render_info rinfo() const override ;
	void render( evt::exenv& env ) const override ;
};

class block_for_object : public evt::context_object {
	std::size_t length;
	std::size_t cur_iter = 0;
public:
	block_for_object(std::size_t sz);
	void next();

	void add(east::string_t n, std::shared_ptr<context_object> child) override ;
	std::shared_ptr<context_object> find(east::var_name n) const override ;
	east::value_term solve() const override ;
	json jval() const override ;
	std::shared_ptr<context_object> call(
	        std::vector<function_parameter> params) const override ;
};

} // namespace cppjinja::evtnodes
