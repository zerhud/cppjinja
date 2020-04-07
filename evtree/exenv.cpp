/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "exenv.hpp"
#include "eval/ast_cvt.hpp"
#include "helpers/binary_op.hpp"

using namespace cppjinja::details;

cppjinja::evt::exenv::exenv(
          const cppjinja::data_provider* prov
        , const cppjinja::evtree* tree
        , std::ostream& out
        )
    : compiled_template(tree)
    , user_data(prov)
    , ostream(out)
{
}

const cppjinja::evtree& cppjinja::evt::exenv::tmpl() const
{
	assert(compiled_template);
	return *compiled_template;
}

const cppjinja::data_provider* cppjinja::evt::exenv::data() const
{
	assert(user_data);
	return user_data;
}

std::ostream& cppjinja::evt::exenv::out()
{
	return ostream;
}

cppjinja::east::value_term cppjinja::evt::exenv::solve_value(
        const cppjinja::ast::value_term& val) const
{
	struct {
		const exenv* self;
		typedef east::value_term ret_t;

		east::value_term make_array(
		            const std::vector<ast::forward_ast<ast::value_term>>& fields)
		{
			east::array_v ret;
			for(auto& i:fields)
				ret.items.push_back(
				            std::make_unique<east::value_term>(
				                self->solve_value(i.get())));
			return ret;
		}

		ret_t operator()(const double& obj) { return obj; }
		ret_t operator()(const ast::string_t& obj) { return obj; }
		ret_t operator()(const ast::tuple_v& obj) { return make_array(obj.fields); }
		ret_t operator()(const ast::array_v& obj) { return make_array(obj.fields); }
		ret_t operator()(const ast::var_name& obj)
		{
			//return self->solve_var_name(obj);
			return 1.1;
		}
		ret_t operator()(const ast::function_call& obj)
		{
			east::function_call call;
			call.ref = east_cvt::cvt(obj.ref);
			for(auto& src:obj.params)
				call.params.emplace_back(east::function_parameter{
				            src.name,
				            self->solve_value(src.value.get())});
			return self->user_data->value(call);
		}
		ret_t operator()(const ast::binary_op& op)
		{
			auto left = self->solve_value(op.left);
			auto right = self->solve_value(op.right);
			evtnodes::binary_op_helper cmp(op.op);
			return std::visit(cmp,
			            (east::value_term_var&)left,
			            (east::value_term_var&)right);
		}
	} rnd{this};

	return boost::apply_visitor(rnd, val.var);
}

cppjinja::evt::context_new& cppjinja::evt::exenv::ctx()
{
	return exectx;
}

cppjinja::evt::callstack& cppjinja::evt::exenv::calls()
{
	return execalls;
}
