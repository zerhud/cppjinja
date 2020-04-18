/*************************************************************************
 * Copyright © 2019-2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <iostream>
#include "evtree.hpp"
#include "exenv/impl.hpp"
#include "parser/helpers.hpp"

using namespace std::literals;

namespace cppjinja::details {

template<typename Block, typename... Args>
evt::node* create_node(
          std::vector<std::unique_ptr<evt::node>>& dest
        , evt::node* parent
        , Args&&... args)
{
	evt::node* block = dest.emplace_back(
	            std::make_unique<Block>(std::forward<Args>(args)...)
	            ).get();
	block->add_parent(parent);
	return block;
}

template<typename Src>
static void insert_content(
          std::vector<std::unique_ptr<evt::node>>& dest
        , evt::node* parent
        , Src& src
        , evt::node* def
        )
{
	overloaded insterter {
		[&dest,parent,def](ast::forward_ast<ast::block_named>& nb)
		{
			evt::node* tnode = create_node<evtnodes::block_named>(
			            dest, parent, nb.get());
			insert_content(dest, parent, nb.get(), tnode);

			if(!nb.get().params.empty()) return;

			ast::op_out print_this_block;
			print_this_block.value = ast::function_call{{tnode->name()}, {}};
			print_this_block.open.trim = nb.get().left_open.trim;
			print_this_block.close.trim = nb.get().right_close.trim;
			create_node<evtnodes::op_out>(dest, def, print_this_block);
		},
		[&dest,parent](ast::forward_ast<ast::block_macro>& mb)
		{
			// macro cannot be declared inside a block
			evt::node* tnode =
			        create_node<evtnodes::block_macro>(dest, parent, mb.get());
			insert_content(dest, parent, mb.get(), tnode);
		},
		[&dest,&def](ast::string_t& str)
		{
			create_node<evtnodes::content>(dest, def, str);
		},
		[&dest,&def](ast::op_out& obj)
		{
			create_node<evtnodes::op_out>(dest, def, obj);
		},
		[&dest,&def](ast::op_set& obj)
		{
			create_node<evtnodes::op_set>(dest, def, obj);
		},
		[&dest,&def](ast::forward_ast<ast::block_raw>& obj)
		{
			create_node<evtnodes::content>(
					dest, def, obj.get().value);
		},
		[&dest,parent,&def](ast::forward_ast<ast::block_if>& obj)
		{
			ast::block_if& oif = obj.get();
			auto* bl_if = create_node<evtnodes::block_if>
					( dest
					, def
					, oif
					);
			evt::render_info ri{
				oif.left_close.trim, oif.right_open.trim};
			auto* cnt_if   = create_node<evtnodes::content_block>(
					dest, bl_if, ri, "if"s
					);
			auto* cnt_else = create_node<evtnodes::content_block>(
					dest, bl_if, ri, "else"s
					);
			insert_content(dest, parent, oif, cnt_if);
			if(oif.else_block)
				insert_content(
					dest, parent,
					*oif.else_block, cnt_else);
		},
		[](auto&)//[&def](auto& cnt)
		{
			//def.content.emplace_back(std::move(cnt));
		}
	};

	decltype(src.content) tmp_cnt;
	tmp_cnt.swap(src.content);
	for(auto& cnt:tmp_cnt)  boost::apply_visitor(insterter, cnt.var);
}

} // namespace cppjinja::details


cppjinja::evtree& cppjinja::evtree::add_tmpl(ast::tmpl& tmpl)
{
	std::vector<evt::node*> cur_parents;
	for(auto& ex:tmpl.extends)
	{
		ast::string_t exname = ex.file_name;
		if(ex.tmpl_name) exname = *ex.tmpl_name;

		for(auto&& n:nodes)
			if(is_tmpl(*n) && n->name() == exname)
				cur_parents.emplace_back(n.get());
	}

	if(cur_parents.size() != tmpl.extends.size())
		throw std::runtime_error("cannot find all parents");

	evt::node* tnode = nodes.emplace_back(
	            std::make_unique<evtnodes::tmpl>(tmpl)).get();
	assert( tnode );

	for(auto&& p:cur_parents) tnode->add_parent(p);
	tbuild_blocks(tnode, tmpl);

	return *this;
}

void cppjinja::evtree::tbuild_blocks(cppjinja::evt::node* p, ast::tmpl& t)
{
	ast::block_named main;
	main.name = ""s;

	evt::node* mnode = nodes.emplace_back(
	            std::make_unique<evtnodes::block_named>(std::move(main))
	            ).get();
	mnode->add_parent(p);

	details::insert_content(nodes, p, t, mnode);

}

const cppjinja::evt::node* cppjinja::evtree::search_child(
          const cppjinja::ast::string_t& name
        , const cppjinja::evt::node* par
        ) const
{
	for(auto& n:nodes)
	{
		if(n->name() == name && n->is_parent(par))
			return n.get();
	}

	auto ppars = par->parents();
	for(auto&& p:ppars)
		if(auto found = search_child(name, p); found)
			return found;

	return nullptr;
}

bool cppjinja::evtree::is_tmpl(const cppjinja::evt::node& n) const
{
	return dynamic_cast<const evtnodes::tmpl*>(&n) != nullptr;
}

const cppjinja::evtnodes::tmpl* cppjinja::evtree::search_tmpl(
        const cppjinja::ast::var_name& name) const
{
	if(name.size()!=1) return nullptr;
	for(auto& n:nodes)
		if(is_tmpl(*n) && n->name()==name[0])
			return dynamic_cast<const evtnodes::tmpl*>(n.get());
	return nullptr;
}

const cppjinja::evt::node* cppjinja::evtree::search(
          const cppjinja::ast::var_name& name
        , const cppjinja::evt::node* ctx
        ) const
{
	if(name.empty()) return nullptr;

	if(ctx == nullptr)
	{
		// absolute path (tmpl.callable)
		if(2<name.size()) return nullptr;
		for(auto& n:nodes)
		{
			if(is_tmpl(*n) && n->name()==name[0])
			{
				if(name.size() == 1)
					throw std::runtime_error(name[0] + " is a template"s);
				return search_child(name[1], n.get());
			}
		}
	}
	else if(name.size()==1)
	{
		auto pars = ctx->parents();
		if(pars.empty()) return search_child(name[0], ctx);
		for(auto& p:pars)
			if(auto found = search_child(name[0], p); found)
				return found;
	}
	else if(name[0]=="self")
	{
		ast::var_name self_name = name;
		self_name.erase(self_name.begin());
		return search(self_name, ctx);
	}

	return nullptr;
}

std::vector<const cppjinja::evt::node*> cppjinja::evtree::all_tree() const
{
	std::vector<const cppjinja::evt::node*> ret;
	ret.reserve(nodes.size());
	for(auto& n:nodes) ret.emplace_back(n.get());

	assert( std::find(std::begin(ret), std::end(ret), nullptr)
			== std::end(ret));

	return ret;
}

std::vector<const cppjinja::evt::node*> cppjinja::evtree::children(
          const cppjinja::evt::node* selected
        , bool add_subs) const
{
	std::vector<const cppjinja::evt::node*> ret;
	for(auto& n:nodes) if(n->is_parent(selected)) ret.emplace_back(n.get());

	if(!add_subs) return ret;

	std::vector<const cppjinja::evt::node*> subs;
	for(auto& n:ret)
	{
		auto isubs = children(n);
		for(auto& n:isubs) subs.emplace_back(n);
	}

	for(auto& n:subs) ret.emplace_back(n);

	assert( std::find(std::begin(ret), std::end(ret), nullptr)
			== std::end(ret));
	return ret;
}

std::string cppjinja::evtree::print_subtree(const cppjinja::evt::node* par) const
{
	if(!par) for(auto& n:nodes) if(n->parents().empty()) par = n.get();

	auto clist = children(par);
	std::string ret = '\'' + par->name() + "\'[";
	for(auto& c:clist) ret += print_subtree(c);
	ret += "]";
	return ret;
}

void cppjinja::evtree::render(
          std::ostream &to
        , const data_provider &data
        , const ast::string_t &name
        ) const
{
	evt::node* tnode = nullptr;
	if(name.empty())
		for(auto& n:nodes) { if(is_tmpl(*n)) { tnode = n.get(); break; } }
	else
	{
		for(auto& n:nodes)
		{
			if(is_tmpl(*n) && n->name() == name)
			{
				tnode = n.get();
				break;
			}
		}
	}

	if(!tnode) throw std::runtime_error("cannot find " + name);

	auto* cb_tnode = dynamic_cast<evtnodes::callable*>(tnode);
	assert(cb_tnode);

	evt::exenv_impl env(&data, this);
	to << cb_tnode->evaluate(env);
}
