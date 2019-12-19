/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/


#include "tmpl.hpp"
#include "evaluator/evaluate_tree.hpp"

using namespace cppjinja::eval_tree;

tmpl::tmpl(ast::tmpl cur, const evaluate_tree* et)
    : cur_tmpl_(std::move(cur)), evaluator_(et)
{
	ast::block_named main_block;
	for(auto& cnt:cur_tmpl_.content) main_block.content.emplace_back(std::move(cnt));
	cur_tmpl_.content.clear();
	blocks_.emplace_back(std::move(main_block), evaluator_);
}

std::string tmpl::name() const
{
	return cur_tmpl_.name;
}

cppjinja::ast::string_t tmpl::render(const cppjinja::data_provider& data) const
{
	ast::string_t ret;

	for(auto& bl:blocks_) {
		const bool is_main_block = bl.name() && bl.name()->empty();
		if(is_main_block) ret += bl.render({}, data);
	}

	return ret;
}

bool tmpl::has_block(cppjinja::ast::string_t name) const
{
	return false;
}
