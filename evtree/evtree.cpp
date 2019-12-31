/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "evtree.hpp"
#include "parser/helpers.hpp"

using namespace std::literals;

namespace cppjinja::details {

template<typename Src, typename Default>
static void insert_content(
          std::vector<std::unique_ptr<node>>& dest
        , node* parent
        , Src& src
        , Default& def
        )
{
	overloaded insterter {
		[&dest,parent](ast::forward_ast<ast::block_named>& nb)
		{
			node* tnode = dest.emplace_back(
			            std::make_unique<evtnodes::block_named>(nb.get())
			            ).get();
			if(parent) tnode->add_parent(parent);
			insert_content(dest, parent, nb.get(), nb.get());
		},
		[&dest,parent](ast::forward_ast<ast::block_macro>& mb)
		{
			// macro cannot be declared inside a block
			node* tnode = dest.emplace_back(
			            std::make_unique<evtnodes::block_macro>(mb.get())
			            ).get();
			if(parent) tnode->add_parent(parent);
			//insert_content(dest, paretn, mb.get(), mb.get());
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


cppjinja::evtree& cppjinja::evtree::add_tmpl(ast::tmpl &tmpl)
{
	std::vector<node*> cur_parents;
	for(auto& ex:tmpl.extends)
	{
		ast::string_t exname = ex.file_name;
		if(ex.tmpl_name) exname = *ex.tmpl_name;

		for(auto&& n:nodes)
			if(!n->is_leaf() && n->name() == exname)
				cur_parents.emplace_back(n.get());
	}

	if(cur_parents.size() != tmpl.extends.size())
		throw std::runtime_error("cannot find all parents");

	node* tnode = nodes.emplace_back(
	            std::make_unique<evtnodes::tmpl>(tmpl)).get();
	assert( tnode );

	for(auto&& p:cur_parents) tnode->add_parent(p);
	tbuild_blocks(tnode, tmpl);

	return *this;
}

void cppjinja::evtree::tbuild_blocks(cppjinja::node* p, ast::tmpl& t)
{
	ast::block_named main;
	main.name = ""s;
	details::insert_content(nodes, p, t, main);

	node* tnode = nodes.emplace_back(
	            std::make_unique<evtnodes::block_named>(std::move(main))
	            ).get();
	assert( tnode );
	tnode->add_parent(p);
}

const cppjinja::node* cppjinja::evtree::search_child(
          const cppjinja::ast::string_t& name
        , const cppjinja::node* par
        ) const
{
	for(auto& n:nodes)
		if(n->name() == name && n->is_parent(par))
			return n.get();

	auto ppars = par->parents();
	for(auto&& p:ppars)
		if(auto found = search_child(name, p); found)
			return found;

	return nullptr;
}

const cppjinja::node* cppjinja::evtree::search(
          const cppjinja::ast::var_name& name
        , const cppjinja::node* ctx
        ) const
{
	if(name.empty()) return nullptr;

	if(ctx == nullptr)
	{
		// absolute path (tmpl.callable)
		if(2<name.size()) return nullptr;
		for(auto& n:nodes)
		{
			if(!n->is_leaf() && n->name()==name[0])
			{
				if(name.size() == 1) return n.get();
				return search_child(name[1], n.get());
			}
		}
	}
	else if(name.size()==1)
	{
		auto pars = ctx->parents();
		for(auto& p:pars)
			if(auto found = search_child(name[0], p); found)
				return found;
	}

	return nullptr;
}

std::vector<const cppjinja::node*> cppjinja::evtree::all_tree() const
{
	std::vector<const cppjinja::node*> ret;
	ret.reserve(nodes.size());
	for(auto& n:nodes) ret.emplace_back(n.get());

	assert( std::find(std::begin(ret), std::end(ret), nullptr) == std::end(ret));

	return ret;
}

std::vector<const cppjinja::node*> cppjinja::evtree::children(
        const cppjinja::node* selected
        ) const
{
	std::vector<const cppjinja::node*> ret;
	std::vector<const cppjinja::node*> subs;

	for(auto& n:nodes) if(n->is_parent(selected)) ret.emplace_back(n.get());

	for(auto& n:ret)
	{
		auto isubs = children(n);
		for(auto& n:isubs) subs.emplace_back(n);
	}

	for(auto& n:subs) ret.emplace_back(n);

	assert( std::find(std::begin(ret), std::end(ret), nullptr) == std::end(ret));
	return ret;
}

void cppjinja::evtree::render(
          std::ostream &to
        , const data_provider &data
        , const ast::string_t &name
        ) const
{
	(void) to;
	(void) data;
	(void) name;
}
