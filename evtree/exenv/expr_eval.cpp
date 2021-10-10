/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "expr_eval.hpp"

#include <iomanip>

#include "evtree/exenv.hpp"
#include "context_objects/value.hpp"

#include "expr/math.hpp"
#include "expr/cmp_check.hpp"
#include "expr/logic_check.hpp"

#include "absd/simple_data_holder.hpp"

using namespace std::literals;

template<typename T>
absd::data cppjinja::evt::expr_eval::create_data(T&& arg) const
{
	auto ret = std::allocate_shared<absd::simple_data_holder>(
	            std::pmr::polymorphic_allocator(env->storage().get()),
	            env->storage());
	if constexpr (std::is_same_v<std::decay_t<T>,std::pmr::string>)
	        ret->str() = std::move(arg);
	else if constexpr (std::is_same_v<std::decay_t<T>,std::string>)
	        ret->str().append(arg.begin(),arg.end());
	else *ret = std::forward<T>(arg);
	return absd::data{ret};
}

absd::data cppjinja::evt::expr_eval::cvt(const cppjinja::ast::expr_ops::term& v) const
{
	auto cvt = [this](const auto& v) -> eval_type { return create_data(v); };
	return boost::apply_visitor(cvt, v);
}

std::pmr::string cppjinja::evt::expr_eval::to_str(const absd::data& v) const
{
	typedef std::pmr::string str_t;
	std::basic_stringstream<char,str_t::traits_type,str_t::allocator_type> out_s;
	absd::to_json_printer{out_s}(v);
	return out_s.str();
}

bool cppjinja::evt::expr_eval::to_bool(const absd::data& v) const
{
	if(v.is_string()) return !v.str().empty();
	if(v.is_integer()) return !!(std::int64_t)v;
	if(v.is_float()) return !!(double)v;
	if(v.is_boolean()) return !!v;
	return false;
}

cppjinja::ast::expr_ops::term cppjinja::evt::expr_eval::to_term(const absd::data& d) const
{
	using ast::expr_ops::term;
	if(d.is_integer()) return term{(std::int64_t)d};
	if(d.is_float()) return term{(double)d};
	if(d.is_string()) return term{d.hstr()};
	if(d.is_boolean()) return term{(bool)d};
	throw std::runtime_error("cannot apply this operator on such type");
}

void cppjinja::evt::expr_eval::solve_point_arg(cppjinja::ast::expr_ops::point_element& left) const
{
	auto setter = [this](auto& l) { solve_point_arg(l); };
	boost::apply_visitor(setter, left);
}

void cppjinja::evt::expr_eval::solve_point_arg(cppjinja::ast::expr_ops::single_var_name& left) const
{
	std::pmr::string ln(left.name.begin(),left.name.end());
	if(!result) result = env->all_ctx().find(east::var_name{ln});
	else result = result->find(east::var_name{ln});
}

void cppjinja::evt::expr_eval::solve_point_arg(ast::expr_ops::expr& left) const
{
	if(!result) throw std::runtime_error("wrong access element");
	auto solved = expr_eval(env)(left);
	result = result->find(east::var_name{solved->solve().str()});
}

void cppjinja::evt::expr_eval::solve_point_arg(cppjinja::ast::expr_ops::point& left) const
{
	(*this)(left);
}

cppjinja::evt::context_object::function_parameter
cppjinja::evt::expr_eval::make_param(cppjinja::ast::expr_ops::expr& pexpr) const
{
	context_object::function_parameter ret;
	if(auto* asg = boost::get<ast::expr_ops::eq_assign>(&pexpr); asg) {
		ret.name = make_param_name(asg->names.at(0));
		ret.value = (*this)(asg->value.get());
	}
	else
		ret.value = (*this)(pexpr);
	return ret;
}

std::optional<cppjinja::east::string_t> cppjinja::evt::expr_eval::make_param_name(ast::expr_ops::lvalue& name) const
{
	std::string n = boost::get<ast::expr_ops::single_var_name>(name).name;
	return std::pmr::string(n.begin(),n.end());
}

void cppjinja::evt::expr_eval::filter_content(cppjinja::ast::expr_ops::filter_call& call) const
{
	assert(result);

	auto base = result;
	result.reset();
	auto filter = solve_ref(call.ref);

	std::pmr::vector<context_object::function_parameter> params;
	params.emplace_back(context_object::function_parameter{"$", std::move(base)});
	for(auto& p:call.args) params.emplace_back(make_param(p.get()));

	result = filter->call(params);
}

std::shared_ptr<cppjinja::evt::context_object> cppjinja::evt::expr_eval::solve_ref(cppjinja::ast::expr_ops::lvalue& ref) const
{
	assert(!result);
	boost::apply_visitor([this](auto& r){solve_point_arg(r);}, ref);
	if(!result) throw std::runtime_error("cannot solve filter");
	auto ret = result;
	result.reset();
	return ret;
}

absd::data cppjinja::evt::expr_eval::perform_test(cppjinja::ast::expr_ops::cmp_check& t) const
{
	assert(t.op == ast::expr_ops::cmp_op::test);
	auto* check_fnc = boost::get<ast::expr_ops::fnc_call>(&t.right.get());
	auto* check_vas = boost::get<ast::expr_ops::single_var_name>(&t.right.get());
	auto* check_var = boost::get<ast::expr_ops::point>(&t.right.get());
	assert(check_fnc || check_vas || check_var);
	ast::expr_ops::fnc_call check;
	ast::expr_ops::point ref;
	ref.left = ast::expr_ops::single_var_name{"$tests"s};
	if(check_fnc) boost::apply_visitor([&ref](auto&v){ref.right = v;}, check_fnc->ref);
	else if(check_vas) ref.right = *check_vas;
	else ref.right = *check_var;
	check.ref = ref;
	check.args.insert(check.args.begin(), t.left);
	return (*this)(check);
}

cppjinja::evt::expr_eval::expr_eval(const exenv* e)
    : env(e)
{
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::expr_eval::operator ()(cppjinja::ast::expr_ops::expr t) const
{
	auto solved = boost::apply_visitor(*this, t);
	if(result) return result;
	return std::make_shared<context_objects::value>(solved);
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evt::expr_eval::operator () (ast::expr_ops::lvalue ref) const
{
	result.reset();
	return solve_ref(ref);
}

bool cppjinja::evt::expr_eval::operator()(ast::expr_ops::expr_bool e) const
{
	result.reset();
	auto val = boost::apply_visitor(*this, e);
	//if(result) val = result->solve();
	//result.reset();
	return to_bool(val);
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(cppjinja::ast::expr_ops::term& t) const
{
	return cvt(t);
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::single_var_name& t) const
{
	std::pmr::string tn(t.name.begin(),t.name.end());
	result = env->all_ctx().find(east::var_name{tn});
	return result->solve();
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::list& t) const
{
	auto ret = std::allocate_shared<absd::simple_data_holder>(
	            std::pmr::polymorphic_allocator(env->storage().get()),
	            env->storage());
	for(auto& item:t.items) ret->push_back(visit(item));
	return absd::data{ret};
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::tuple& t) const
{
	auto ret = std::allocate_shared<absd::simple_data_holder>(
	            std::pmr::polymorphic_allocator(env->storage().get()),
	            env->storage());
	for(auto& item:t.items) ret->push_back(visit(item));
	return absd::data{ret};
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::dict& t) const
{
	auto ret = std::allocate_shared<absd::simple_data_holder>(
	            std::pmr::polymorphic_allocator(env->storage().get()),
	            env->storage());
	for(auto& item:t.items) ret->put(item.name, visit(item.value));
	return absd::data{ret};
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::eq_assign&) const
{
	throw std::logic_error("cannot solve eq_assign expression");
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::in_assign&) const
{
	throw std::logic_error("cannot solve in_assign expression");
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::math& t) const
{
	return cvt(boost::apply_visitor(
	            expr_evals::math(t.op),
	            to_term(visit(t.left)),
	            to_term(visit(t.right))
	            ));
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::concat& t) const
{
	return create_data(to_str(visit(t.left)) + to_str(visit(t.right)));
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::in_check& t) const
{
	auto term = eval(t.term);
	auto object = eval(t.object);
	if(object.is_string() && term.is_string())
		return create_data(object.str().find(term.str()) != std::string::npos);
	if(term.is_object() || term.is_array())
		throw std::runtime_error("cannot use array with 'in' operator");
	auto info = object.src()->reflect();
	if(info.size != 0) {
		for(std::size_t i=0;i<info.size;++i)
			if(object[i]==term) return create_data(true);
		return create_data(false);
	}
	if(!info.keys.empty()) {
		for(auto& key:info.keys)
			if(key == term) return create_data(true);
		return create_data(false);
	}
	throw std::runtime_error("use 'in' operator only for array maps and strings");
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::cmp_check& t) const
{
	if(t.op == ast::expr_ops::cmp_op::test) return perform_test(t);
	return cvt(boost::apply_visitor(
	            expr_evals::cmp_check(t.op),
	            to_term(visit(t.left)),
	            to_term(visit(t.right))
	            ));
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::logic_check& t) const
{
	return cvt(boost::apply_visitor(
	            expr_evals::logic_check(t.op),
	            to_term(visit(t.left)),
	            to_term(visit(t.right))
	            ));
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::negate& t) const
{
	auto solved = visit(t.arg);
	if(solved.is_boolean()) return create_data(!solved);
	if(solved.is_integer()) return create_data(!((std::int64_t)solved));
	if(solved.is_float()) return create_data(!((double)solved));
	throw std::runtime_error("cannot nagate such value");
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::fnc_call& t) const
{
	auto call_obj = solve_ref(t.ref);
	std::pmr::vector<context_object::function_parameter> params(env->storage().get());
	for(auto& p:t.args) params.emplace_back(make_param(p.get()));
	result = call_obj->call(params);
	return create_data(false);
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::filter& t) const
{
	result = (*this)(t.base.get());
	for(auto& f:t.filters) filter_content(f);
	return create_data(false);
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::point& t) const
{
	solve_point_arg(t.left);
	if(!result) throw std::runtime_error("cannot find");
	solve_point_arg(t.right);
	if(!result) throw std::runtime_error("cannot find");
	return create_data(std::pmr::string{});
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::op_if& t) const
{
	if((*this)(t.cond.get())) return visit(t.term);
	if(t.alternative) return visit(*t.alternative);
	return create_data("");
}
