/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "context_impl.hpp"
#include "eval/ast_cvt.hpp"

using namespace cppjinja::details;

void cppjinja::evt::context_impl::require_not_empty() const
{
	if(ctx.empty()) throw std::runtime_error("context is mepty");
}

cppjinja::evt::context_impl::frame& cppjinja::evt::context_impl::last_shadow()
{
	for(auto pos=ctx.rbegin();pos!=ctx.rend();++pos)
		if(pos->shadow) return *pos;
	throw std::runtime_error("no shadow on stack");
}

const cppjinja::evt::context_impl::frame& cppjinja::evt::context_impl::last_shadow() const
{
	return const_cast<context_impl*>(this)->last_shadow();
}

void cppjinja::evt::context_impl::create_shadow()
{
	std::vector<context_object*> queue;
	for(auto pos=ctx.rbegin();pos!=ctx.rend();++pos) {
		queue.push_back(&pos->ns);
		if(pos->shadow) break;
	}
	last_shadow().shadow.emplace(queue);
}

cppjinja::evt::context_impl::context_impl()
{
}

cppjinja::evt::context_impl::~context_impl() noexcept
{
}

void cppjinja::evt::context_impl::current_node(const cppjinja::evt::node* n)
{
	require_not_empty();
	ctx.back().node_stack.emplace_back(n);
}

const cppjinja::evt::node*
cppjinja::evt::context_impl::nth_node_on_stack(std::size_t ind) const
{
	require_not_empty();
	std::size_t i = ctx.back().node_stack.size() - ind - 1;
	return ctx.back().node_stack.at(i);
}

void cppjinja::evt::context_impl::pop(const cppjinja::evt::node* n)
{
	require_not_empty();
	if(ctx.back().maker != n)
		throw std::runtime_error("cannot pop that wans't pushed");
	ctx.pop_back();
	if(!ctx.empty()) create_shadow();
}

void cppjinja::evt::context_impl::push(const cppjinja::evt::node* n)
{
	last_shadow();
	ctx.emplace_back(frame{n, {}, std::stringstream{}, context_objects::tree(), std::nullopt});
	create_shadow();
}

void cppjinja::evt::context_impl::push_shadow(const cppjinja::evt::node* n)
{
	ctx.emplace_back(frame{n, {}, std::stringstream{}, context_objects::tree(), std::nullopt});
	ctx.back().shadow = context_objects::queue(&ctx.back().ns);
}

const cppjinja::evt::node* cppjinja::evt::context_impl::maker() const
{
	return last_shadow().maker;
}

std::ostream& cppjinja::evt::context_impl::out()
{
	return last_shadow().out;
}

std::string cppjinja::evt::context_impl::result() const
{
	return last_shadow().out.str();
}

cppjinja::evt::context_object& cppjinja::evt::context_impl::cur_namespace()
{
	assert(last_shadow().shadow.has_value());
	return *last_shadow().shadow;
}
