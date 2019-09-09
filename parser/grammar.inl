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

#ifndef FILE_INLINING
#include "traits.hpp"
#include "grammar.hpp"
namespace cppjinja::deubg {
using gram_traits = traits::unicode_utf8;
#endif

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

//-------------- debug block
auto printer = overloaded{
	  [](const std::string& s){std::cout << '|' << s << '|' << std::endl;}
	, [](const std::wstring& s){std::wcout << L'|' << s << L'|' << std::endl;}
	, [](const wchar_t s){std::wcout << L'|' << s << L'|' << std::endl;}
	, [](const st_if<std::string>&){}
	, [](const st_if<std::wstring>&){}
};
auto nop = [](auto&){}; //< debug lambda
auto pass = [](auto& ctx){_pass(ctx)=true;}; //< debug lambda
auto fail = [](auto& ctx){_pass(ctx)=false;}; //< debug lambda
auto print = [](auto& ctx){ printer(_attr(ctx)); };
auto print_and_fail = [](auto& ctx){ printer(_attr(ctx)); _pass(ctx)=false;};
//-------------- debug block

auto peq = [](auto& ctx){ _val(ctx) += _attr(ctx); };
template<typename T>
struct setter{
	T value;
	explicit setter(T v) : value(v) {}
	template<typename Ctx> void operator()(Ctx& c){_val(c) = value;}
};

template<typename>
struct is_ref_wrapper : std::false_type {};
template<typename T>
struct is_ref_wrapper<std::reference_wrapper<T>> : std::true_type {};

template<typename>
struct is_req_wrapper : std::false_type {};
template<typename T>
struct is_req_wrapper<boost::recursive_wrapper<T>> : std::true_type {};

template<typename T>
struct setter_field {
	T& field;
	explicit setter_field(T& f) : field(f) {}

	template<typename Ctx>
	std::enable_if_t<is_ref_wrapper<T>::value>
	operator()(Ctx& c){field.get() = _attr(c);}

	template<typename Ctx>
	std::enable_if_t<is_req_wrapper<T>::value>
	operator()(Ctx& c){field.get() = _attr(c);}

	template<typename Ctx>
	std::enable_if_t<!is_ref_wrapper<T>::value>
	operator()(Ctx& c){field = _attr(c);}
};

auto block_add_content = [](auto& ctx) {
	block_t& b = _val(ctx);
	gram_traits::concat(add_and_get<block_t, gram_traits::types::out_string_t>(b), _attr(ctx));
};
auto block_add_content_vec = [](auto& ctx){
	if(_val(ctx).empty()) {_val(ctx).emplace_back(_attr(ctx));return;}
	auto& back = _val(ctx).back();
	if(std::holds_alternative<gram_traits::types::out_string_t>(back))
	        std::get<gram_traits::types::out_string_t>(back) += _attr(ctx);
	else _val(ctx).emplace_back(_attr(ctx));
};

static struct  block_add_op_rw_t {
	template<typename Type, typename Value>
	void operator() (std::reference_wrapper<Type>& b, Value&& v){ b.get().cnt.emplace_back(std::forward<Value>(v)); }
	template<typename Type, typename Value>
	void operator() (std::vector<Type>& b, Value&& v){ b.emplace_back(std::forward<Value>(v)); }
	template<typename Type, typename Value>
	void operator() (Type& b, Value&& v){ b.cnt.emplace_back(std::forward<Value>(v)); }
} block_add_op_rw;

static struct block_set_ref_rw_t {
	template<typename Type, typename Value>
	void operator() (std::reference_wrapper<Type>& b, Value&& v){ b.get().ref = std::move(v); }
	template<typename Type, typename Value>
	void operator() (Type& b, Value&& v){ b.ref = std::move(v); }
} block_set_ref_rw ;

auto block_add_op = [](auto& ctx) {block_add_op_rw(_val(ctx),std::move(_attr(ctx)));};
auto block_set_ref = [](auto& ctx) {block_set_ref_rw(_val(ctx),std::move(_attr(ctx)));};
auto def_cnt = [](auto&c){_val(c).cnt=_attr(c);};
auto add_cnt = [](auto&c){_val(c).cnt+=_attr(c);};
auto clear_cnt = [](auto&c){_val(c).cnt.clear();};

auto exd = [](auto& ctx){ return x3::get<parser_data>(ctx); };
auto cmp = [](const auto& arg, auto& ctx){ _pass(ctx) = gram_traits::compare(_attr(ctx), arg); };

auto op_out_is_start = [](auto& ctx) { cmp(exd(ctx).output.b, ctx); };
auto op_out_is_end = [](auto& ctx) { cmp(exd(ctx).output.e, ctx); };
auto op_term_is_start = [](auto& ctx) { cmp(exd(ctx).term.b, ctx); };
auto op_term_is_end = [](auto& ctx) { cmp(exd(ctx).term.e, ctx); };
auto op_comment_is_start = [](auto& ctx) { cmp(exd(ctx).cmt.b, ctx); };
auto op_comment_is_end = [](auto& ctx) { cmp(exd(ctx).cmt.e, ctx); };
auto op_term_check_end = [](auto& ctx) {
	using str_t = gram_traits::types::out_string_t;

	std::string& val = _attr(ctx);
	auto& block = _val(ctx);

	if(!block.ref) {
		_pass(ctx)=false;
		return;
	}

	bool ok = false;
	if(std::holds_alternative<st_if<str_t>>(*block.ref))
		ok = val == "endif";
	if(std::holds_alternative<st_for<str_t>>(*block.ref))
		ok = val == "endfor";
	if(std::holds_alternative<st_raw>(*block.ref))
		ok = val == "endraw";
	if(std::holds_alternative<std::string>(*block.ref))
		ok = val == "endblock";
	_pass(ctx) = ok;
};

auto op_ref_define = [](auto& ctx) { _val(ctx).ref = _attr(ctx); };
auto op_params_define = [](auto& ctx) { _val(ctx).params.emplace_back(_attr(ctx)); };

const x3::rule<struct spec_symbols, gram_traits::types::out_string_t> spec_symbols = "spec_symbols";
const auto spec_symbols_def = x3::repeat(2)[gram_traits::char_("!#$%&()*+,-./:;<=>?@[\\]^_`{|}~")];

const x3::rule<struct single_var_name, std::string> single_var_name = "single_var_name";
const auto single_var_name_def = gram_traits::char_("A-Za-z_") >> *gram_traits::char_("0-9A-Za-z_");
const x3::rule<struct var_name_rule, std::vector<std::string>> var_name_rule = "var_name_rule";
const auto var_name_rule_def = single_var_name % '.';

const x3::rule<struct qstr, gram_traits::types::out_string_t> quoted_string = "string";
const auto quoted_string_def =
	 (x3::lit("'") >> *(('\\' > x3::char_("'")) | ~x3::char_("\\'")) >> x3::lit("'"))
	|(x3::lit('"') >> *(('\\' > x3::char_('"')) | ~x3::char_("\\\"")) >> x3::lit('"'))
;

const x3::rule<struct fnc_call_tag, fnc_call<gram_traits::types::out_string_t>> fnc_call_rule = "fnc_call";
const auto fnc_call_rule_def = x3::skip(gram_traits::space)[
	   var_name_rule[op_ref_define]
	>> x3::char_('(')
	>> -((quoted_string[op_params_define] | var_name_rule[op_params_define] /*| fnc_call_rule[op_params_define]*/) % ',')
	>> ')' ]
;

const x3::rule<struct op_comment, st_comment<gram_traits::types::out_string_t>> op_comment = "comment";
const auto op_comment_def =
	  (spec_symbols[op_comment_is_start] >> spec_symbols[op_comment_is_end])
	| (spec_symbols[op_comment_is_start]
	>> +(gram_traits::char_[add_cnt] >> !spec_symbols[op_comment_is_end])
	>> gram_traits::char_
	>> spec_symbols[op_comment_is_end])
	| (spec_symbols[op_comment_is_start] >> gram_traits::char_[clear_cnt][add_cnt] >> spec_symbols[op_comment_is_end])
;

const x3::rule<struct op_out, st_out<gram_traits::types::out_string_t>> op_out = "out_operator";
const auto op_out_def =
	   spec_symbols[op_out_is_start]
	>> x3::skip(gram_traits::space)[
	     ( fnc_call_rule[op_ref_define] | quoted_string[op_ref_define] | var_name_rule[op_ref_define] )
	  >> -( '|' >> (fnc_call_rule[op_params_define] | var_name_rule[op_params_define]) % '|')
	>> spec_symbols[op_out_is_end] ] ;


const x3::rule<struct raw_text, gram_traits::types::out_string_t> raw_text = "raw_text";
const auto raw_text_def = +(gram_traits::char_[peq] >> (!(
	spec_symbols[op_term_is_start] >> *gram_traits::space >> x3::lit("endraw") >> *gram_traits::space >> spec_symbols[op_term_is_end]
	))) >> gram_traits::char_
;

const x3::rule<struct free_text, gram_traits::types::out_string_t> free_text = "free_text";
const auto free_text_def = 
	*(gram_traits::char_[peq] >> (!(
	  spec_symbols[op_out_is_start]
	| spec_symbols[op_term_is_start]
	))) >> gram_traits::char_
;

const x3::rule<struct value_term_tag, value_term<gram_traits::types::out_string_t>> value_term_r = "value_term";
const auto value_term_r_def = var_name_rule | quoted_string ;

const x3::rule<struct comparation_tag, comparator> comparator_r = "comparator";
const auto comparator_r_def = 
	  (x3::lit("==") | x3::lit("is"))[setter(comparator::eq)]
	| (x3::lit("<") | x3::lit("less"))[setter(comparator::less)]
	| (x3::lit("=<") | x3::lit("leq"))[setter(comparator::less_eq)]
;

const x3::rule<struct op_for, st_for<gram_traits::types::out_string_t>> op_for = "op_for";
const auto op_for_def =
	   *gram_traits::space
	>> x3::lit("for") >> +gram_traits::space
	>> (single_var_name[op_params_define] % ',') >> +gram_traits::space
	>> x3::lit("in") >> +gram_traits::space
	>> value_term_r[op_ref_define] >> *gram_traits::space
	;

const x3::rule<struct op_if, st_if<gram_traits::types::out_string_t>> op_if = "op_if";
const auto op_if_def =
	   *gram_traits::space
	>> x3::lit("if") >> +gram_traits::space
	>> value_term_r[([](auto&c){_val(c).left=_attr(c);})] >> -(+gram_traits::space
	>> comparator_r[([](auto&c){_val(c).op=_attr(c);})] >> +gram_traits::space
	>> value_term_r[([](auto&c){_val(c).right=_attr(c);})])
	>> *gram_traits::space
	;

auto cnt_if_raw = [](auto& ctx){
	auto& ref = _val(ctx).ref;
	if(!ref || !std::holds_alternative<st_raw>(*ref)) {
		_pass(ctx)=false;
		return;
	}

	_val(ctx).cnt.emplace_back(_attr(ctx));
};
const x3::rule<struct op_raw, st_raw> op_raw = "op_raw";
const auto op_raw_def = *gram_traits::space[nop] >> x3::lit("raw") >> *gram_traits::space[nop];

const x3::rule<struct block_content_tag, block_content<gram_traits::types::out_string_t>> block_content_r = "block_content";
const x3::rule<struct block_r1, block_t> block_r1 = "block_r1";
const x3::rule<struct block_tag, std::reference_wrapper<gram_traits::types::block_t>> block = "block";
const auto block_r1_def =
	   (spec_symbols[op_term_is_start]
	>> (op_if[block_set_ref] | op_for[block_set_ref] | op_raw[block_set_ref] | (x3::lit("block") >> +gram_traits::space >> single_var_name[block_set_ref]) ))
	>  spec_symbols[op_term_is_end]
	>> -(raw_text[cnt_if_raw] | block_content_r[def_cnt])
	>> x3::skip(gram_traits::space)[
		   spec_symbols[op_term_is_start]
		>> (+x3::alnum)[op_term_check_end]
		>> spec_symbols[op_term_is_end]
	]
	;
const auto block_def =
	*(
		  op_out[ block_add_op ]
		| block_r1[ block_add_op ]
		| gram_traits::char_ [ block_add_content ]
	);
const auto block_content_r_def =
	*(
	op_out[ block_add_op ] | op_comment[block_add_op] | block_r1[ block_add_op ] | free_text [ block_add_content_vec ]
	)
;

BOOST_SPIRIT_DEFINE(block)
BOOST_SPIRIT_DEFINE(op_out)
BOOST_SPIRIT_DEFINE(quoted_string)
BOOST_SPIRIT_DEFINE(spec_symbols)
BOOST_SPIRIT_DEFINE(single_var_name)
BOOST_SPIRIT_DEFINE(var_name_rule)
BOOST_SPIRIT_DEFINE(fnc_call_rule)
BOOST_SPIRIT_DEFINE(block_r1)
BOOST_SPIRIT_DEFINE(free_text)
BOOST_SPIRIT_DEFINE(block_content_r)
BOOST_SPIRIT_DEFINE(op_if)
BOOST_SPIRIT_DEFINE(comparator_r)
BOOST_SPIRIT_DEFINE(value_term_r)
BOOST_SPIRIT_DEFINE(op_for)
BOOST_SPIRIT_DEFINE(op_raw)
BOOST_SPIRIT_DEFINE(raw_text)
BOOST_SPIRIT_DEFINE(op_comment)

#ifndef FILE_INLINING
} // namespace cppjinja::deubg
#endif
