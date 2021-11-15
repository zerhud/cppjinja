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

cppjinja::evt::context_object_ptr
cppjinja::evt::expr_eval::make_value_object(eval_type v)
{
	return std::make_shared<cppjinja::evt::context_objects::value>(std::move(v));
}

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
	if(d.is_empty()) return term{ast::string_t("")};
	throw std::runtime_error("cannot apply this operator on such type");
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::point& t) const
{
	auto solved_point = solve_point_arg(env->all_ctx(), t);
	assert(solved_point);
	return solved_point->solve();
}

cppjinja::evt::context_object_ptr cppjinja::evt::expr_eval::solve_point_arg(
        const evt::context_object& left,
        cppjinja::ast::expr_ops::point_element& expr) const
{
	auto setter = [this,&left](auto& e) { return solve_point_arg(left, e); };
	return boost::apply_visitor(setter, expr);
}

cppjinja::evt::context_object_ptr cppjinja::evt::expr_eval::solve_point_arg(
        const evt::context_object& left,
        cppjinja::ast::expr_ops::single_var_name& epxr) const
{
	std::pmr::string ln(epxr.name.begin(),epxr.name.end());
	return left.find(ln);
}

cppjinja::evt::context_object_ptr cppjinja::evt::expr_eval::solve_point_arg(
        const evt::context_object& left,
        ast::expr_ops::expr& expr) const
{
	eval_type solved = expr_eval(env)(expr);
	east::string_t key;
	if(solved.is_integer()) key = std::to_string((std::int64_t)solved).c_str();
	else key = solved.str();
	return left.find(key);
}

cppjinja::evt::context_object_ptr cppjinja::evt::expr_eval::solve_point_arg(
        const evt::context_object& left,
        cppjinja::ast::expr_ops::point& expr) const
{
	auto obj_left = solve_point_arg(left, expr.left);
	return solve_point_arg(*obj_left, expr.right);
}

cppjinja::evt::context_object::function_parameter
cppjinja::evt::expr_eval::make_param(cppjinja::ast::expr_ops::expr& pexpr) const
{
	context_object::function_parameter ret;
	if(auto* asg = boost::get<ast::expr_ops::eq_assign>(&pexpr); asg) {
		ret.name = make_param_name(asg->names.at(0));
		ret.value = make_value_object((*this)(asg->value.get()));
	}
	else
		ret.value = make_value_object((*this)(pexpr));
	return ret;
}

std::optional<cppjinja::east::string_t> cppjinja::evt::expr_eval::make_param_name(ast::expr_ops::lvalue& name) const
{
	std::string n = boost::get<ast::expr_ops::single_var_name>(name).name;
	return std::pmr::string(n.begin(),n.end());
}

cppjinja::evt::expr_eval::eval_type cppjinja::evt::expr_eval::filter_content(
        eval_type base, cppjinja::ast::expr_ops::filter_call& call) const
{
	auto filter = solve_ref(call.ref);

	std::pmr::vector<context_object::function_parameter> params;
	params.emplace_back(context_object::function_parameter{"$", make_value_object(std::move(base))});
	for(auto& p:call.args) params.emplace_back(make_param(p.get()));

	return filter->call(params)->solve();
}
cppjinja::evt::context_object_ptr cppjinja::evt::expr_eval::solve_ref(
        cppjinja::ast::expr_ops::lvalue& ref) const
{
	return boost::apply_visitor(
	            [this](auto& r) {
		return solve_point_arg(env->all_ctx(), r);
	}, ref);
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

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(cppjinja::ast::expr_ops::expr t) const
{
	return boost::apply_visitor(*this, t);
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator () (ast::expr_ops::lvalue ref) const
{
	return solve_ref(ref)->solve();
}

bool cppjinja::evt::expr_eval::operator()(ast::expr_ops::expr_bool e) const
{
	auto val = boost::apply_visitor(*this, e);
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
	return env->all_ctx().find(tn)->solve();
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
	auto cmp = [&t](const eval_type& left, const eval_type& right) -> bool
	{
	    using ast::expr_ops::cmp_op;
	    if(t.op == cmp_op::eq) return left == right;
	    if(t.op == cmp_op::neq) return left != right;
	    if(t.op == cmp_op::less) return left < right;
	    if(t.op == cmp_op::more) return left > right;
	    if(t.op == cmp_op::less_eq) return left <= right;
	    if(t.op == cmp_op::more_eq) return left >= right;
	    assert(false);
	    return false;
	};
	return cvt(cmp( visit(t.left), visit(t.right) ));
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::logic_check& t) const
{
	absd::data left = visit(t.left);
	bool bleft = to_bool(left);
	if(!bleft && t.op == ast::expr_ops::logic_op::op_and)
		return create_data(false);
	if(bleft && t.op == ast::expr_ops::logic_op::op_or)
		return create_data(true);
	return cvt(boost::apply_visitor(
	            expr_evals::logic_check(t.op),
	            to_term(left),
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
	return call_obj->call(params)->solve();
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::filter& t) const
{
	auto base = (*this)(t.base.get());
	for(auto& f:t.filters) base = filter_content(base, f);
	return base;
}

cppjinja::evt::expr_eval::eval_type
cppjinja::evt::expr_eval::operator ()(ast::expr_ops::op_if& t) const
{
	if((*this)(t.cond.get())) return visit(t.term);
	if(t.alternative) return visit(*t.alternative);
	return create_data("");
}
