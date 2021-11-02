/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "../context_object.hpp"

namespace cppjinja::evt::context_objects {

class inner_navigation : public context_object {
	exenv* env;
protected:
	const evtnodes::tmpl* find_tmpl_by_import(east::var_name n) const ;
	std::shared_ptr<context_object> find_in_cur_tmpl() const;
	std::shared_ptr<context_object> find_in_tmpl(const evtnodes::tmpl* t) const;
	std::shared_ptr<context_object> find_in_tmpl(const evtnodes::tmpl* t, east::var_name n) const;
	std::shared_ptr<context_object> find_in_roots(east::var_name n) const;
	std::shared_ptr<context_object> find_in_imports(east::var_name n) const;
public:
	inner_navigation(exenv* e);

	void add(east::string_t n, std::shared_ptr<context_object> child) override ;
	std::shared_ptr<context_object> find(east::var_name n) const override ;
	absd::data solve() const override ;
	std::shared_ptr<context_object> call(
	        std::pmr::vector<function_parameter> params) const override ;
};

class navigation_imp : public inner_navigation {
public:
	navigation_imp(exenv* e);
	std::shared_ptr<context_object> find(east::var_name n) const override ;
};

class navigation_tmpl : public inner_navigation
{
public:
	navigation_tmpl(exenv* e);
	std::shared_ptr<context_object> find(east::var_name n) const override ;
};

class navigation_single_tmpl : public inner_navigation
{
	const evtnodes::tmpl* tmpl;
public:
	navigation_single_tmpl(exenv* e, const evtnodes::tmpl* tmpl);
	std::shared_ptr<context_object> find(east::var_name n) const override ;
};
} // namespace cppjinja::evt::context_objects