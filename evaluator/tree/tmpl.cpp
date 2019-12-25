/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/


#include "tmpl.hpp"
#include "parser/helpers.hpp"
#include "evaluator/evaluate_tree.hpp"
#include "../ast_cvt.hpp"

using namespace cppjinja::eval_tree;
using namespace std::literals;

void tmpl::extract_blocks(std::vector<cppjinja::ast::block_content>& cnt)
{
	overloaded need_extract {
		  [this](const ast::forward_ast<ast::block_named>& o){
			blocks_.emplace_back(o.get(), this);
			return true;
		}
		, [this](const ast::forward_ast<ast::block_macro>& o){
			blocks_.emplace_back(o.get(), this);
			return true;
		}
		, [](const auto&){return false;}
	};

	for(auto& c:cnt) boost::apply_visitor(need_extract, c.var);
	//auto rpos = std::remove_if(
	              //cnt.begin()
	            //, cnt.end()
	            //, [&need_extract](auto& cnt){
	                //return boost::apply_visitor(need_extract, cnt.var);
	            //});
	//cnt.erase(rpos, cnt.end());
}

tmpl::tmpl(ast::tmpl cur, const evaluate_tree* et)
    : cur_tmpl_(std::move(cur)), evaluator_(et)
{
	extract_blocks(cur_tmpl_.content);

	ast::block_named main_block;
	for(auto& cnt:cur_tmpl_.content)
		main_block.content.emplace_back(std::move(cnt));
	cur_tmpl_.content.clear();

	blocks_.emplace_back(std::move(main_block), this);
}

std::string tmpl::name() const
{
	return cur_tmpl_.name;
}

cppjinja::ast::string_t tmpl::render(const cppjinja::data_provider& data) const
{
	ast::string_t ret;
	cur_data_ = &data;

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

cppjinja::ast::string_t tmpl::render(const cppjinja::ast::function_call& fnc) const
{
	assert(cur_data_);
	assert(!fnc.ref.empty());

	const ast::var_name& ref = fnc.ref;

	if(ref.size()==1) for(auto& child:blocks_)
	{
		assert(child.name().has_value());
		if(*child.name() == ref[0]) return child.render(fnc.params, *cur_data_);
		return cur_data_->render(details::east_cvt::cvt(fnc));
	}

	if(ref.at(0) == "super"s) ;
	return "tmpl_function";
}
