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

cppjinja::evtree& cppjinja::evtree::add_tmpl(ast::tmpl& tmpl)
{
	templates.emplace_back(evt::tmpl_compiler()(tmpl));
	return *this;
}

const cppjinja::evt::compiled_tmpl& cppjinja::evtree::tmpl_by_node(
        const cppjinja::evt::node* n) const
{
	for(auto& t:templates) {
		auto pos = std::find_if(std::begin(t.nodes), std::end(t.nodes),
		                        [n](const auto& tn){return tn.get()==n;});
		if(pos != std::end(t.nodes)) return t;
	}
	throw std::runtime_error("no such node in tree");
}

const cppjinja::evtnodes::tmpl*
cppjinja::evtree::search_tmpl(const ast::string_t& name) const
{
	for(auto& t:templates)
		if(t.tmpl_node()->name() == name) return t.tmpl_node();
	return nullptr;
}

std::vector<const cppjinja::evt::node*> cppjinja::evtree::all_tree() const
{
	std::vector<const cppjinja::evt::node*> ret;
	for(auto& t:templates) for(auto& n:t.nodes) ret.emplace_back(n.get());
	assert(std::find(std::begin(ret), std::end(ret), nullptr) == std::end(ret));
	return ret;
}

std::vector<const cppjinja::evt::node*> cppjinja::evtree::children(
          const cppjinja::evt::node* selected
        , bool add_subs) const
{
	std::vector<const cppjinja::evt::node*> ret;
	if(!selected) return ret;

	const evt::compiled_tmpl& tmpl = tmpl_by_node(selected);
	for(auto& edge:tmpl.lrnd) if(edge.parent == selected) ret.emplace_back(edge.child);

	if(!add_subs) return ret;

	std::vector<const cppjinja::evt::node*> subs;
	for(auto& n:ret)
	{
		auto isubs = children(n);
		for(auto& n:isubs) subs.emplace_back(n);
	}

	for(auto& n:subs) ret.emplace_back(n);

	assert(std::find(std::begin(ret), std::end(ret), nullptr) == std::end(ret));
	return ret;
}

std::vector<const cppjinja::evtnodes::callable*>
cppjinja::evtree::roots(const evtnodes::tmpl* tmpl) const
{
	std::vector<const cppjinja::evtnodes::callable*> ret;
	for(auto& ct:templates) if(ct.tmpl_node() == tmpl) {
		for(auto& r:ct.roots) ret.emplace_back(r);
	}
	return ret;
}

std::string cppjinja::evtree::print_subtree(const cppjinja::evt::node* par) const
{
	if(templates.empty()) return "";
	if(!par) par = templates[0].tmpl_node();

	auto clist = children(par);
	std::string ret = '\'' + par->name() + "\'[";
	for(auto& c:clist) ret += print_subtree(c);
	ret += "]";
	return ret;
}

void cppjinja::evtree::render(
          std::ostream& to
        , const data_provider& data
        , const ast::string_t& name
        ) const
{
	if(templates.empty()) throw std::runtime_error("cannot render empty tree");
	const evtnodes::tmpl* tnode = search_tmpl(name);
	if(!tnode) tnode = templates[0].tmpl_node();

	evt::exenv_impl env(&data, this);
	to << tnode->evaluate(env);
}
