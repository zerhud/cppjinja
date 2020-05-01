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
		if(t.tmpl_name == name) return t.tmpl_node();
	return nullptr;
}

std::vector<const cppjinja::evt::node*> cppjinja::evtree::children(
          const cppjinja::evt::node* selected) const
{
	if(!selected) return {};
	return tmpl_by_node(selected).render_tree.children(selected);
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

void cppjinja::evtree::render(
          std::ostream& to
        , const data_provider& data
        , const ast::string_t& name
        ) const
{
	if(templates.empty()) throw std::runtime_error("cannot render empty tree");
	const evtnodes::tmpl* tnode = search_tmpl(name);
	if(!tnode) throw std::runtime_error("no such template: " + name);

	evt::exenv_impl env(&data, this);
	evt::raii_push_ctx ctx_maker(tnode, &env.ctx());
	tnode->render(env);
	to << env.result();
}
