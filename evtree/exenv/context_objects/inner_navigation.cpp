/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "inner_navigation.hpp"
#include "exenv.hpp"
#include "exenv/context.hpp"
#include "exenv/context_objects/callable_node.hpp"
#include "evtree.hpp"
#include "absd/simple_data_holder.hpp"

using namespace std::literals;
using cppjinja::evt::context_object;
using cppjinja::evt::context_objects::navigation_env;
using cppjinja::evt::context_objects::navigation_tmpl;
using cppjinja::evt::context_objects::inner_navigation;

navigation_env::navigation_env(exenv* e) : inner_navigation(e)
{
}

std::shared_ptr<context_object> navigation_env::find(east::var_name n) const
{
	if(n.size() != 2) return nullptr;
	return find_in_imports(std::move(n));
}

navigation_tmpl::navigation_tmpl(exenv* e) : inner_navigation(e)
{
}

std::shared_ptr<context_object> navigation_tmpl::find(east::var_name n) const
{
	if(n.size() != 1) return nullptr;
	return find_in_roots(std::move(n));
}

inner_navigation::inner_navigation(exenv *e)
    : env(e)
{
	if(!env) throw std::runtime_error(
	            "cannot create inner navigation obejct "
	            "without environment object");
}

void inner_navigation::add(east::string_t n, std::shared_ptr<context_object> child)
{
	(void) n;
	(void) child;
	throw std::logic_error("cannot add somthing to inner navigation object");
}

std::shared_ptr<context_object> inner_navigation::find(east::var_name n) const
{
	if(n.size() == 1) return find_in_roots(n);
	if(n.size() == 2) return find_in_imports(n);
	return nullptr;
}

std::shared_ptr<context_object> inner_navigation::find_in_roots(east::var_name n) const
{
	assert(n.size() == 1);
	return find_in_tmpl(env->ctx().current_tmpl(), n);
}

std::shared_ptr<context_object> inner_navigation::find_in_imports(east::var_name n) const
{
	auto cur_imports = env->ctx().current_tmpl()->imports();
	std::string first_var(n.front().begin(), n.front().end());
	for(auto& i:cur_imports) if(i.as == first_var) {
		auto* found_tmpl = env->tmpl().search_tmpl(i.filename);
		n.erase(n.begin());
		return find_in_tmpl(found_tmpl, n);
	}
	return nullptr;
}

std::shared_ptr<context_object> inner_navigation::find_in_tmpl(
        const evtnodes::tmpl* t, east::var_name n) const
{
	assert(n.size()==1);

	auto roots = env->roots(t);
	for(auto& r:roots) if(r->name() == n[0])
		return std::make_shared<evt::context_objects::callable_node>(env, r);
	throw std::runtime_error("cannot find "s + n[0].c_str());
}

absd::data inner_navigation::solve() const
{
	throw std::logic_error("cannot solve inner navigation objec");
}

std::shared_ptr<context_object> inner_navigation::call(
    std::pmr::vector<function_parameter> params) const
{
	(void) params;
	throw std::logic_error("cannot call inner navigation objec");
}
