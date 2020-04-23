/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "tmpl.hpp"
#include "evtree/evtree.hpp"
#include "evtree/exenv/obj_holder.hpp"

cppjinja::evtnodes::tmpl::tmpl(cppjinja::ast::tmpl t)
    : itmpl_(std::move(t))
{
}

cppjinja::evt::render_info cppjinja::evtnodes::tmpl::rinfo() const
{
	return {false, false};
}

cppjinja::ast::string_t cppjinja::evtnodes::tmpl::name() const
{
	return itmpl_.name;
}

bool cppjinja::evtnodes::tmpl::is_leaf() const
{
	return false;
}

void cppjinja::evtnodes::tmpl::render(evt::exenv& ctx) const
{
	evt::raii_push_ctx ctx_maker(this, &ctx.ctx());
	ctx.current_node(this);

	auto children = ctx.children(this);
	for(auto&& child:children)
		if(child->name().empty())
		{
			auto* cb_child = dynamic_cast<const callable*>(child);
			evt::raii_push_callstack calls_maker(this, cb_child, &ctx.calls());
			child->render(ctx);
		}
}

cppjinja::east::string_t
cppjinja::evtnodes::tmpl::evaluate(cppjinja::evt::exenv& env) const
{
	evt::raii_push_ctx ctx_maker(this, &env.ctx());
	env.current_node(this);
	create_self_obj(&env);

	auto children = env.children(this);
	for(auto&& child:children)
		if(child->name().empty())
		{
			auto* cb_child = dynamic_cast<const callable*>(child);
			evt::raii_push_callstack calls_maker(this, cb_child, &env.calls());
			env.out() << cb_child->evaluate(env);
		}

	return env.result();
}

void cppjinja::evtnodes::tmpl::create_self_obj(cppjinja::evt::exenv* env) const
{
	auto children = env->children(this);
	auto self = std::make_shared<callable_multisolver>(env);
	for(auto& _child:children) {
		const callable* child = dynamic_cast<const callable*>(_child);
		auto child_obj = std::make_unique<callable_solver>(env, child);
		self->add(child->name(), child);
		env->globals().add(child->name(), std::move(child_obj));
	}
	if(!children.empty())
		env->globals().add("self", std::move(self));
}

std::optional<cppjinja::ast::value_term> cppjinja::evtnodes::tmpl::param(
          const cppjinja::evt::callstack& ctx
        , const cppjinja::ast::var_name& name
        ) const
{
	(void)ctx;
	(void)name;
	return std::nullopt;
}
