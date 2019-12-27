/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "evaluate_tree.hpp"

#include "parser/helpers.hpp"

using namespace cppjinja;

namespace cppjinja::details {

template<typename Src, typename Default>
static void insert_content(eval_tree::tmpl& dest, Src& src, Default& def)
{
	overloaded insterter {
		[&dest](ast::forward_ast<ast::block_named>& nb)
		{
			insert_content(dest, nb.get(), nb.get());
			dest.add_child(eval_tree::block(nb.get(), &dest));
		},
		[&dest](ast::forward_ast<ast::block_macro>& mb)
		{
			// macro cannot be declared inside a block
			dest.add_child(eval_tree::block(mb.get(), &dest));
		},
		[&def](auto& cnt)
		{
			def.content.emplace_back(std::move(cnt));
		}
	};

	decltype(src.content) tmp_cnt;
	tmp_cnt.swap(src.content);
	for(auto& cnt:tmp_cnt)  boost::apply_visitor(insterter, cnt.var);
}

} // namespace cppjinja::details

bool evaluate_tree::insert_to_tree(ast::tmpl& t)
{
	eval_tree::tmpl* inserted = nullptr;
	if(t.extends.empty()) inserted = &roots.emplace_back(t, this);
	else
	{
		inserted = &collection.emplace_back(t, this);
		for(auto ex:t.extends)
		{
			auto tname = ex.tmpl_name;
			if(!tname) tname = ex.file_name;
			eval_tree::tmpl* base = find_tmpl_in_tree(roots_ptrs(), *tname);
			if(base) base->add_child(inserted);
			else throw std::runtime_error("cannot fid "+*tname);
		}
	}

	if(inserted)
	{
		ast::block_named main_block;
		details::insert_content(*inserted, t, main_block);
		if(!main_block.content.empty())
			inserted->add_child(
			            eval_tree::block(std::move(main_block), inserted)
			            );
	}

	return inserted != nullptr;
}

eval_tree::tmpl* evaluate_tree::find_tmpl_in_tree(
          const std::vector<eval_tree::tmpl*>& tree
        , const std::string& name
        ) const
{
	for(auto& t:tree) if(t->name() == name) return t;
	for(auto& t:tree)
	{
		auto ret = find_tmpl_in_tree(t->child_tmpls(), name);
		if(ret) return ret;
	}
	return nullptr;
}

std::vector<eval_tree::tmpl*> evaluate_tree::roots_ptrs()
{
	std::vector<eval_tree::tmpl*> ret;
	for(auto& t:roots) ret.emplace_back(&t);
	return ret;
}

evaluate_tree::evaluate_tree(std::vector<ast::tmpl> tmpls)
{
	bool inserted = false;
	do{
		inserted = false;
		auto rpos = std::remove_if(
		            std::begin(tmpls), std::end(tmpls),
		            [this](auto&& t){return insert_to_tree(t);});
		inserted = inserted || rpos != std::end(tmpls);
		tmpls.erase(rpos, std::end(tmpls));
	} while(inserted);

	if(!tmpls.empty()) throw std::runtime_error("cannot create tree");
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
	for(auto& t:roots) ret.emplace_back(&t);
	return ret;
}

const eval_tree::block* evaluate_tree::find_block(
          ast::var_name name
        , const eval_tree::block* cur
        ) const
{
	assert(cur);

	const eval_tree::tmpl* par = cur->parent();
	if(name.size()==1)
	{
		for(auto& block:par->child_blocks())
			if(block->name() && *block->name() == name[0])
				return block;
	}

	return nullptr;
}
