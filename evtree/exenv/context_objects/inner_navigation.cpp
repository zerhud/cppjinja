/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "inner_navigation.hpp"
#include "exenv.hpp"
#include "exenv/context_objects/callable_node.hpp"
#include "evtree.hpp"
#include "absd/simple_data_holder.hpp"

using namespace std::literals;
using cppjinja::evt::context_object;
using cppjinja::evt::context_objects::navigation_imp;
using cppjinja::evt::context_objects::navigation_tmpl;
using cppjinja::evt::context_objects::inner_navigation;
using cppjinja::evt::context_objects::navigation_single_tmpl;

navigation_imp::navigation_imp(exenv* e) : inner_navigation(e)
{
}

std::shared_ptr<context_object> navigation_imp::find(east::var_name n) const
{
	if(n.size() == 2) return find_in_imports(std::move(n));
	if(n.size() == 1) {
		auto found_tmpl = find_tmpl_by_import(n);
		if(found_tmpl) return find_in_tmpl(found_tmpl);
	}
	return nullptr;
}

navigation_tmpl::navigation_tmpl(exenv* e) : inner_navigation(e)
{
}

std::shared_ptr<context_object> navigation_tmpl::find(east::var_name n) const
{
	const bool single_name = n.size() == 1;
	auto ret = single_name ? find_in_roots(n) : nullptr;
	if(ret) return ret;
	const bool link_to_cur_tmpl = !n.empty() && n[0]=="self";
	n.erase(n.begin());
	if(link_to_cur_tmpl)
		return single_name ? find_in_cur_tmpl() : find_in_roots(n);
	return nullptr;
}

navigation_single_tmpl::navigation_single_tmpl(exenv* e, const cppjinja::evtnodes::tmpl* tmpl)
    : inner_navigation(e)
    , tmpl(tmpl)
{
	assert(tmpl);
}

std::shared_ptr<context_object> navigation_single_tmpl::find(east::var_name n) const
{
	return find_in_tmpl(tmpl, n);
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
	return find_in_tmpl(env->current_tmpl(), n);
}

std::shared_ptr<context_object> inner_navigation::find_in_imports(east::var_name n) const
{
	const evtnodes::tmpl* found_tmpl = find_tmpl_by_import(n);
	if(!found_tmpl) return nullptr;
	n.erase(n.begin());
	return find_in_tmpl(found_tmpl, n);
}

const cppjinja::evtnodes::tmpl* inner_navigation::find_tmpl_by_import(east::var_name n) const
{
	assert(!n.empty());
	auto cur_imports = env->current_tmpl()->imports();
	std::string first_var(n.front().begin(), n.front().end());
	for(auto& i:cur_imports) if(i.as == first_var) {
		auto ret = env->tmpl().search_tmpl(i.tmpl_name.empty() ? i.filename : i.tmpl_name);
		return ret;
	}
	return nullptr;
}

std::shared_ptr<context_object> inner_navigation::find_in_tmpl(
        const evtnodes::tmpl* t, east::var_name n) const
{
	assert(n.size()==1);
	assert(t != nullptr);

	auto roots = env->roots(t);
	for(auto& r:roots) if(r->name() == n[0]) {
		const evtnodes::tmpl* ctc = nullptr;
		if(env->current_tmpl() != t) ctc = t;
		return std::make_shared<evt::context_objects::callable_node>(env, r, ctc);
	}
	return nullptr;
}

std::shared_ptr<context_object> inner_navigation::find_in_tmpl(const evtnodes::tmpl* t) const
{
	assert(t != nullptr);
	return std::make_shared<navigation_single_tmpl>(env, t);
}

std::shared_ptr<context_object> inner_navigation::find_in_cur_tmpl() const
{
	return find_in_tmpl(env->current_tmpl());
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
