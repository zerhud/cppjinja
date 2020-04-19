/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "tmpl_compiler.hpp"

#include "nodes/tmpl.hpp"
#include "nodes/content.hpp"
#include "nodes/block_named.hpp"
#include "nodes/op_out.hpp"

typedef std::unique_ptr<cppjinja::evt::node> node_ptr;
typedef std::vector<node_ptr> vec_type;

cppjinja::evt::compiled_tmpl
cppjinja::evt::tmpl_compiler::operator()(cppjinja::ast::tmpl t)
{
	result.nodes.emplace_back(std::make_unique<evtnodes::tmpl>(t));
	result.nodes.emplace_back(std::make_unique<evtnodes::block_named>(ast::block_named{}));

	result.lctx.emplace_back(result.nodes[0].get(), result.nodes[1].get());

	ctx_stack.emplace_back(result.nodes[1].get());
	rnd_stack.emplace_back(result.nodes[1].get());

	for(auto& c:t.content) boost::apply_visitor(*this, c.var);

	return std::move(result);
}

void cppjinja::evt::tmpl_compiler::operator()(cppjinja::ast::string_t& cnt)
{
	assert(!ctx_stack.empty());
	assert(!rnd_stack.empty());
	node* created = result.nodes.emplace_back(
	            std::make_unique<evtnodes::content>(cnt)).get();
	result.lctx.emplace_back(ctx_stack.back(), created);
	result.lrnd.emplace_back(rnd_stack.back(), created);
}

void cppjinja::evt::tmpl_compiler::operator()(ast::forward_ast<ast::block_named>& obj)
{
	ast::op_out out;
	result.nodes.emplace_back(std::make_unique<evtnodes::op_out>(out));

	result.nodes.emplace_back(std::make_unique<evtnodes::block_named>(obj));
	for(auto& c:obj.get().content) boost::apply_visitor(*this, c.var);
}

bool cppjinja::evt::operator ==(
        const cppjinja::evt::edge& l, const cppjinja::evt::edge& r) noexcept
{
	return l.parent == r.parent && l.child == r.child;
}
