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

using namespace std::literals;
using obj_val = cppjinja::evt::context_objects::value;

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
	assert(value);
	assert(abl.vars.size()==1 || abl.vars.size()==2);
	eval_for(env, value->solve());
}

void cppjinja::evtnodes::block_for::eval_for(evt::exenv& env, absd::data val) const
{
	bool need_else = true;
	auto children = env.children(this);
	auto loop = std::make_shared<block_for_object>(val.size());
	for(auto& lvar:val.items()) {
		evt::raii_push_ctx ctx(this, &env.ctx());
		need_else = false;
		if(abl.vars.size()==2)
			env.locals().add(
			          abl.vars[0]
			        , std::make_shared<obj_val>(lvar.key()));
		env.locals().add(
		          abl.vars[abl.vars.size()-1]
		        , std::make_shared<obj_val>(lvar.value()));
		env.locals().add("loop", loop);
		children.at(0)->render(env);
		loop->next();
	}

	if(need_else && children.size()==2)
		children[1]->render(env);
}

cppjinja::evtnodes::block_for_object::block_for_object(std::size_t sz)
    : length(sz)
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
cppjinja::evtnodes::block_for_object::find(cppjinja::east::var_name n) const
{
	if(n==east::var_name{"index"s})
		return std::make_shared<evt::context_objects::value>(cur_iter+1);
	if(n==east::var_name{"index0"s} || n==east::var_name{"ind"s})
		return std::make_shared<evt::context_objects::value>(cur_iter);
	if(n == east::var_name{"length"s})
		return std::make_shared<evt::context_objects::value>(length);
	if(n == east::var_name{"cycle"})
		return std::make_shared<evt::context_objects::lambda_function>(
		[cur_iter=cur_iter](auto params) -> std::shared_ptr<evt::context_object>{
			json param_val = params.at(0).value->jval();
			auto val = param_val[cur_iter % param_val.size()];
			return std::make_shared<evt::context_objects::value>(val);
		});
	return nullptr;
}

absd::data cppjinja::evtnodes::block_for_object::solve() const
{
	throw std::runtime_error("cannot find something in loop");
}

std::shared_ptr<cppjinja::evt::context_object>
cppjinja::evtnodes::block_for_object::call(
    std::pmr::vector<function_parameter> params) const
{
	return nullptr;
}
