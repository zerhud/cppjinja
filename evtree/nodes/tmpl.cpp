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

void cppjinja::evtnodes::tmpl::render(evt::exenv& env) const
{
	env.current_node(this);
	create_self_obj(&env);

	auto children = env.roots(this);
	for(auto&& child:children)
		if(child->name().empty())
		{
			auto* cb_child = dynamic_cast<const callable*>(child);
			evt::raii_push_callstack calls_maker(cb_child, &env.calls());
			env.out() << cb_child->evaluate(env);
		}
}

void cppjinja::evtnodes::tmpl::create_self_obj(cppjinja::evt::exenv* env) const
{
	auto children = env->roots(this);
	auto self = std::make_shared<callable_multisolver>(env);
	for(auto& child:children) {
		auto child_obj = std::make_unique<callable_solver>(env, child);
		self->add(child->name(), child);
		env->globals().add(child->name(), std::move(child_obj));
	}
	if(!children.empty())
		env->globals().add("self", std::move(self));
}
