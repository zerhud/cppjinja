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
using cppjinja::evt::context_objects::inner_navigation;
using cppjinja::evt::context_objects::navigation_single_tmpl;

navigation_imp::navigation_imp(exenv* e) : inner_navigation(e)
{
}

std::shared_ptr<context_object> navigation_imp::find(east::string_t n) const
{
	auto found_tmpl = find_tmpl_by_import(n);
	if(!found_tmpl && n=="self") return find_in_tmpl(cur_tmpl());
	return found_tmpl ? find_in_tmpl(found_tmpl) : nullptr;
}

navigation_single_tmpl::navigation_single_tmpl(exenv* e, const cppjinja::evtnodes::tmpl* tmpl)
    : inner_navigation(e)
    , tmpl(tmpl)
{
}

std::shared_ptr<context_object> navigation_single_tmpl::find(east::string_t n) const
{
	auto self = tmpl ? tmpl : cur_tmpl();
	return find_in_pars(self, n);
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

const cppjinja::evtnodes::tmpl* inner_navigation::cur_tmpl() const
{
	assert(env);
	return env->current_tmpl();
}

const cppjinja::evtnodes::tmpl* inner_navigation::find_tmpl(const east::string_t& n) const
{
	ast::string_t ast_name(n.begin(),n.end());
	return env->tmpl().search_tmpl(ast_name);
}

const cppjinja::evtnodes::tmpl* inner_navigation::find_tmpl_by_import(east::string_t n) const
{
	assert(!n.empty());
	auto cur_imports = env->current_tmpl()->imports();
	std::string first_var(n.begin(), n.end());
	for(auto& i:cur_imports) if(i.as == first_var) {
		auto ret = env->tmpl().search_tmpl(i.tmpl_name.name.empty() ? i.filename : i.tmpl_name.name);
		return ret;
	}
	return nullptr;
}

std::shared_ptr<context_object> inner_navigation::find_in_pars(
        const evtnodes::tmpl* t, const east::string_t& n) const
{
	auto found = find_in_tmpl(t, n);
	if(found) return found;
	for(auto& par:t->parents()) {
		auto p = find_tmpl(par);
		found = find_in_tmpl(p, n);
		if(!found) found = find_in_pars(p, n);
		if(found) break;
	}
	return found;
}

std::shared_ptr<context_object> inner_navigation::find_in_tmpl(
        const evtnodes::tmpl* t, east::string_t n) const
{
	assert(t != nullptr);

	auto roots = env->roots(t);
	for(auto& r:roots) if(r->name() == n) {
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
