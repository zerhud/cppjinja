/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "block_for.hpp"
#include "../exenv/expr_eval.hpp"
#include "evtree/exenv/context_objects/value.hpp"
#include "evtree/exenv/context_objects/builtins/lambda.hpp"

#include <absd/simple_data_holder.hpp>

using namespace std::literals;
using obj_val = cppjinja::evt::context_objects::value;
using absd::make_simple;

static std::shared_ptr<obj_val> make_val(cppjinja::evt::exenv& env, absd::data v)
{
	std::pmr::polymorphic_allocator aloc(env.storage().get());
	return std::allocate_shared<obj_val>(aloc, std::move(v));
}
template<typename T>
static std::shared_ptr<obj_val> make_val(cppjinja::evt::exenv& env, T&& v)
{
	std::pmr::polymorphic_allocator aloc(env.storage().get());
	auto h = std::allocate_shared<absd::simple_data_holder>(
	            aloc, env.storage(), std::forward<T>(v) );
	return std::allocate_shared<obj_val>(aloc, absd::data{std::move(h)});
}

template<typename T>
absd::data cppjinja::evtnodes::block_for_object::create_data(T v) const
{
	return make_simple(mem, v);
}

cppjinja::evtnodes::block_for::block_for(cppjinja::ast::block_for ast_bl)
    : abl(ast_bl)
{
	if(abl.vars.size() !=1 && abl.vars.size() != 2)
		throw std::runtime_error("for can itearate only with two or one variables");
}

cppjinja::evt::render_info cppjinja::evtnodes::block_for::rinfo() const
{
	return {};
}

void cppjinja::evtnodes::block_for::render(cppjinja::evt::exenv& env) const
{
	env.current_node(this);
	auto value = evt::expr_eval(&env)(abl.value);
	assert(abl.vars.size()==1 || abl.vars.size()==2);
	eval_for(env, value);
}

void cppjinja::evtnodes::block_for::eval_for(evt::exenv& env, absd::data val) const
{
	bool need_else = true;
	rc_children = env.children(this);
	if(val.is_string()) need_else = eval_for_str(env, val.str());
	if(val.is_array()) need_else = eval_for_arr(env, val.as_array());
	if(val.is_object()) need_else = eval_for_obj(env, val.as_map());
	if(need_else && rc_children.size()==2)
		rc_children[1]->render(env);
}

bool cppjinja::evtnodes::block_for::eval_condition(evt::exenv& env) const
{
	if(!abl.condition) return true;
	return evt::expr_eval(&env)(*abl.condition);
}

bool cppjinja::evtnodes::block_for::eval_for_str(
        evt::exenv& env, std::pmr::string val) const
{
	auto loop = std::make_shared<block_for_object>(env.storage(), val.size());
	for(auto& lvar:val) {
		evt::raii_push_ctx ctx(this, &env.ctx());
		env.locals().add(tps(abl.vars[0]), make_val(env, lvar));
		env.locals().add("loop", loop);
		if(eval_condition(env))
			rc_children.at(0)->render(env);
		loop->next();
	}
	return val.empty();
}

bool cppjinja::evtnodes::block_for::eval_for_arr(
        evt::exenv& env, std::pmr::vector<absd::data> val) const
{
	auto loop = std::make_shared<block_for_object>(env.storage(), val.size());
	for(auto& lvar:val) {
		evt::raii_push_ctx ctx(this, &env.ctx());
		env.locals().add(tps(abl.vars[0]), make_val(env, lvar));
		env.locals().add("loop", loop);
		if(eval_condition(env))
			rc_children.at(0)->render(env);
		loop->next();
	}
	return val.empty();
}

bool cppjinja::evtnodes::block_for::eval_for_obj(
        evt::exenv& env, std::pmr::map<std::pmr::string, absd::data> val) const
{
	auto loop = std::make_shared<block_for_object>(env.storage(), val.size());
	for(auto& [key,lvar]:val) {
		evt::raii_push_ctx ctx(this, &env.ctx());
		if(abl.vars.size()==2)
			env.locals().add(tps(abl.vars[0]), make_val(env, key));
		env.locals().add(tps(abl.vars[abl.vars.size()-1]), make_val(env, lvar));
		env.locals().add("loop", loop);
		if(eval_condition(env))
			rc_children.at(0)->render(env);
		loop->next();
	}
	return val.empty();
}

cppjinja::evtnodes::block_for_object::block_for_object(
        std::shared_ptr<std::pmr::memory_resource> r, std::size_t sz)
    : length(sz)
    , mem(std::move(r))
{
}

void cppjinja::evtnodes::block_for_object::next()
{
	++cur_iter;
}

void cppjinja::evtnodes::block_for_object::add(
        cppjinja::east::string_t, std::shared_ptr<cppjinja::evt::context_object>)
{
	throw std::runtime_error("cannot add something to loop");
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evtnodes::block_for_object::find(cppjinja::east::string_t n) const
{
	if(n=="index")
		return std::make_shared<evt::context_objects::value>(create_data(cur_iter+1));
	if(n=="index0" || n=="ind")
		return std::make_shared<evt::context_objects::value>(create_data(cur_iter));
	if(n == "length")
		return std::make_shared<evt::context_objects::value>(create_data(length));
	if(n == "last")
		return std::make_shared<evt::context_objects::value>(create_data(cur_iter+1 == length));
	if(n == "cycle")
		return std::make_shared<evt::context_objects::lambda_function>(
		[cur_iter=cur_iter](auto params) -> std::shared_ptr<evt::context_object>{
			auto param_val = params.at(0).value->solve().as_array();
			auto val = param_val[cur_iter % param_val.size()];
			return std::make_shared<evt::context_objects::value>(val);
		});
	throw std::runtime_error("cannot find variable "s + n.c_str());
}

absd::data cppjinja::evtnodes::block_for_object::solve() const
{
	throw std::runtime_error("cannot find something in loop");
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evtnodes::block_for_object::call(
    std::pmr::vector<function_parameter> params) const
{
	// the recursive call (same block other data, renders inside parent)
	(void)params;
	throw std::logic_error("not ready yet");
}
