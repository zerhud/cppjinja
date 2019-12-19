/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "evaluate_tree.hpp"

using namespace cppjinja;

evaluate_tree::evaluate_tree(std::vector<ast::tmpl> tmpls)
{
	for(auto& ast_tmpl:tmpls)
		collection.emplace_back(std::move(ast_tmpl), this);
	if(collection.empty()) throw std::runtime_error("no template in evaluator, cannot eval");
}

void evaluate_tree::render(std::ostream& to, const data_provider& data, const ast::string_t& name) const
{
	assert(!collection.empty());
	if(name.empty()) to << collection[0].render(data);
	else for(auto& t:collection) if(t.name()==name) to << t.render(data);
}

std::vector<const eval_tree::tmpl*> evaluate_tree::all_tmpls() const
{
	std::vector<const eval_tree::tmpl*> ret;
	for(auto& t:collection) ret.emplace_back(&t);
	return ret;
}
