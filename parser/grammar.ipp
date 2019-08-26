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
	block_t& b = _val(ctx);
	gram_traits::concat(add_and_get<block_t, gram_traits::types::out_string_t>(b), _attr(ctx));
};

auto is_op_out_start = [](auto& ctx) {
	parser_data penv = x3::get<parser_data>(ctx);
	//const bool is_start = gram_traits::compare(_attr(ctx), penv.output.b);
	const bool is_start = false;//_attr(ctx) == penv.output.b;

	//if(is_start) {
		//block_t& b = _val(ctx);
		//b.cnt.emplace_back(st_out<gram_traits::types::out_string_t>{});
	//}

	return is_start;
};

const x3::rule<struct op_out, st_out<gram_traits::types::out_string_t>> op_out = "out operator";
const auto op_out_def =
	+x3::punct[is_op_out_start] >> +x3::alnum >> +x3::punct[is_op_out_start]
        ;

const x3::rule<struct block, gram_traits::types::block_t> block = "block";
const auto block_def =
	*(
		  op_out
		| gram_traits::char_ [ add_block_content ]
	);

BOOST_SPIRIT_DEFINE(block)
BOOST_SPIRIT_DEFINE(op_out)


//const x3::rule<struct out_string, gram_traits::types::out_string_t()> string = "string";
//const auto string_def = x3::lexeme[ *



