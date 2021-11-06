/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "builtins.hpp"
#include "tree.hpp"
#include "value.hpp"
#include "builtins/lambda.hpp"

#include <absd/simple_data_holder.hpp>

#include "builtins/tests.hpp"

#include <boost/algorithm/string/case_conv.hpp>

using namespace std::literals;
using namespace absd::literals;
using cppjinja::evt::context_objects::make_val;

cppjinja::evt::context_objects::builtins::builtins()
{
	add_child("join", std::make_shared<join>());
	add_child("namespace", std::make_shared<jinja_namespace>());
	auto tests = std::make_shared<tree>();
	add_child("$tests", tests);
	tests->add("callable", std::make_shared<builtins_tests::callable>());
	tests->add("defined", std::make_shared<builtins_tests::defined>());
	tests->add("undefined", std::make_shared<builtins_tests::undefined>());
	tests->add("sameas", std::make_shared<builtins_tests::sameas>());

	add_child("upper", fnc_upper());
}

cppjinja::evt::context_objects::builtins::~builtins() noexcept
{
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::builtins::fnc_upper() const
{
	return std::make_shared<lambda_function>(
	            [](std::pmr::vector<function_parameter> params)->std::shared_ptr<context_object>{
		assert(params.at(0).value!=nullptr);
		auto param = params.at(0).value->solve();
		if(param.is_empty()) return make_val(""_sd);
		auto lower = param.str();
		boost::algorithm::to_upper(lower);
		return make_val(absd::make_simple(lower));
	} );
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::builtin_function::result_bool(bool res) const
{
	static auto res_true = std::make_shared<value>(
	            absd::data{std::make_shared<absd::simple_data_holder>(true)});
	static auto res_false = std::make_shared<value>(
	            absd::data{std::make_shared<absd::simple_data_holder>(false)});
	return res ? res_true : res_false;
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::builtin_function::result_none() const
{
	static auto res = std::make_shared<value>(
	            absd::data{std::make_shared<absd::simple_data_holder>("")});
	return res;
}
std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::builtin_function::result_empty_str() const
{
	return result_none();
}

void cppjinja::evt::context_objects::builtin_function::add(
        cppjinja::east::string_t n, std::shared_ptr<cppjinja::evt::context_object> child)
{
	(void)n;
	(void)child;
	throw std::runtime_error("cannot add something to builtin function");
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::builtin_function::find(east::string_t n) const
{
	(void)n;
	throw std::runtime_error("cannot find something in builtin function");
}

absd::data cppjinja::evt::context_objects::builtin_function::solve() const
{
	throw std::runtime_error("cannot solve builtin function");
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::jinja_namespace::call(
        std::pmr::vector<function_parameter> params) const
{
	if(params.empty()) throw std::runtime_error("cannot create empty namespace");
	auto ret = std::make_shared<tree>();
	for(auto& p:params)
		ret->add(*p.name, p.value);
	return ret;
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::context_objects::join::call(
        std::pmr::vector<function_parameter> params) const
{
	typedef std::pmr::string str_t;
	if(params.size() < 2) return result_empty_str();

	std::size_t ind = 0;
	auto sep = params[ind].value->solve();
	std::basic_stringstream<char,str_t::traits_type,str_t::allocator_type> out;
	while(ind!=params.size()-2)
		out << params[++ind].value->solve() << sep;
	out << params[++ind].value->solve();
	auto hd = std::make_shared<absd::simple_data_holder>(out.str());
	return std::make_shared<value>(absd::data{std::move(hd)});
}
