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

auto nop = [](auto& ctx){_pass(ctx)=true;}; //< debug lambda

auto block_add_content = [](auto& ctx) {
	block_t& b = _val(ctx);
	gram_traits::concat(add_and_get<block_t, gram_traits::types::out_string_t>(b), _attr(ctx));
};

auto block_add_op = [](auto& ctx) {_val(ctx).get().cnt.emplace_back(_attr(ctx));};

auto exd = [](auto& ctx){ return x3::get<parser_data>(ctx); };
auto cmp = [](const auto& arg, auto& ctx){ _pass(ctx) = gram_traits::compare(_attr(ctx), arg); };

auto op_out_is_start = [](auto& ctx) { cmp(exd(ctx).output.b, ctx); };
auto op_out_is_end = [](auto& ctx) { cmp(exd(ctx).output.e, ctx); };

auto op_out_define = [](auto& ctx) {
	st_out<gram_traits::types::out_string_t>& tval = _val(ctx);
	tval.ref = _attr(ctx);
};

const x3::rule<struct spec_symbols, gram_traits::types::out_string_t> spec_symbols = "spec_symbols";
const auto spec_symbols_def = +gram_traits::char_("!#$%&()*+,-./:;<=>?@[\\]^_`{|}~");

const x3::rule<struct qstr, gram_traits::types::out_string_t> quoted_string = "string";
const auto quoted_string_def =
	 (x3::lit("'") >> *(('\\' > x3::char_("'")) | ~x3::char_("\\'")) >> x3::lit("'"))
	|(x3::lit('"') >> *(('\\' > x3::char_('"')) | ~x3::char_("\\\"")) >> x3::lit('"'))
;

const x3::rule<struct op_out, st_out<gram_traits::types::out_string_t>> op_out = "out_operator";
const auto op_out_def = spec_symbols[op_out_is_start] >> x3::skip(gram_traits::space)[ quoted_string[op_out_define] >> spec_symbols[op_out_is_end] ] ;

const x3::rule<struct block, std::reference_wrapper<gram_traits::types::block_t>> block = "block";
const auto block_def =
	*(
		  op_out[ block_add_op ]
		| gram_traits::char_ [ block_add_content ]
	);

BOOST_SPIRIT_DEFINE(block)
BOOST_SPIRIT_DEFINE(op_out)
BOOST_SPIRIT_DEFINE(quoted_string)
BOOST_SPIRIT_DEFINE(spec_symbols)
