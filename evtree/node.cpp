/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "node.hpp"
#include "parser/helpers.hpp"
#include "evtree.hpp"
#include "eval/ast.hpp"
#include "helpers/binary_op.hpp"
#include "helpers/loop_by_three.hpp"
#include "exenv/expr_solver.hpp"
#include "exenv/callstack.hpp"

using namespace std::literals;

void cppjinja::evt::node::add_parent(cppjinja::evt::node* np)
{
	assert(
	      std::find(
	                std::begin(parents_),
	                std::end(parents_),
	                np) == std::end(parents_)
	      );

	parents_.emplace_back(np);
}

std::vector<cppjinja::evt::node*> cppjinja::evt::node::parents(bool all)
{
	auto ret = parents_;
	if(all) for(auto&p:ret) {
		auto ppars = p->parents(true);
		ret.insert(ret.end(), ppars.begin(), ppars.end());
	}
	return ret;
}

std::vector<const cppjinja::evt::node*> cppjinja::evt::node::parents(bool all) const
{
	std::vector<const cppjinja::evt::node*> ret;
	for(auto& p:parents_) ret.emplace_back(p);
	if(all) for(auto&p:ret) {
		auto ppars = p->parents(true);
		ret.insert(ret.end(), ppars.begin(), ppars.end());
	}
	return ret;
}

bool cppjinja::evt::node::is_parent(const cppjinja::evt::node* n) const
{
	for(auto& p:parents_) if(p==n) return true;
	return false;
}

void cppjinja::evt::node::render_children(
        const std::vector<const node*>& children,
        exenv& ctx, render_info default_ri) const
{
	auto rnd = [&ctx,&default_ri]
	    (const node* p, const node* c, const node* n)
	{
		render_info ri = default_ri;
		if(p) ri.trim_left = p->rinfo().trim_right;
		if(n) ri.trim_right = n->rinfo().trim_left;
		ctx.rinfo(std::move(ri));
		c->render(ctx);
	};
	evtnodes::loop_by_three(children, rnd);
}

void cppjinja::evt::node::render_value(
          exenv& ctx
        , const cppjinja::ast::value_term& value
        ) const
{
	auto print = [&ctx](auto& v){ctx.out() << expr_solver(&ctx)(v);};
	boost::apply_visitor(print, value.var);
}
