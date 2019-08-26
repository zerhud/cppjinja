/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

// this file must to be included in some namespace.
// before include command a gram_traits must to be defined.
// also all boost's includes and ast.hpp must to be included.

using block_t = gram_traits::types::block_t;
using block_ptr  = std::shared_ptr<gram_traits::types::block_t>;

template<typename T>
bool need_to_add(const block_t& block)
{
	if(block.cnt.empty()) return true;
	return !std::holds_alternative<T>(block.cnt.back());
}

template<typename Block, typename T>
T& add_and_get(Block& b)
{
	if(need_to_add<T>(b)) b.cnt.emplace_back(T{});
	return std::get<T>(b.cnt.back());
}

auto add_block_content = [](auto& ctx) {
	block_ptr b = x3::get<block_ptr>(ctx);
	gram_traits::concat(add_and_get<block_t, gram_traits::types::out_string_t>(*b), _attr(ctx));
};

auto op_out_is_start = [](auto& ctx) {
	parser_data& penv = x3::get<parser_data>(ctx);
	_pass(ctx) = gram_traits::compare(_attr(ctx), penv.output.b);
};

auto op_out_is_end = [](auto& ctx) {
	parser_data& penv = x3::get<parser_data>(ctx);
	_pass(ctx) = gram_traits::compare(_attr(ctx), penv.output.e);
};

auto op_out_define = [](auto& ctx) {
	block_ptr& out = x3::get<block_ptr>(ctx);
	st_out<gram_traits::types::out_string_t> val{};
	val.ref = _attr(ctx);
	out->cnt.emplace_back(std::move(val));
};

const x3::rule<struct op_out> op_out = "out_operator";
const auto op_out_def =
	(+x3::punct)[op_out_is_start] >> x3::skip(gram_traits::space)[
	   (+x3::alnum)[op_out_define]
	] >> (+x3::punct)[op_out_is_end]
	;

BOOST_SPIRIT_DEFINE(op_out)

const x3::rule<struct block, gram_traits::types::block_t> block = "block";
const auto block_def =
	*(
		  op_out
		| gram_traits::char_ [ add_block_content ]
	);

BOOST_SPIRIT_DEFINE(block)


//const x3::rule<struct out_string, gram_traits::types::out_string_t()> string = "string";
//const auto string_def = x3::lexeme[ *



