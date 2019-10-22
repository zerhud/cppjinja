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
	, [](const std::vector<std::string>& v){ std::cout << "vec: "; for(auto&s:v)std::cout << s << ", "; std::cout<<std::endl; }
	, [](const boost::spirit::x3::unused_type&){std::cout<<std::endl;}
	, [](const st_if<std::string>&){}
	, [](const st_if<std::wstring>&){}
};
auto nop = [](auto&){}; //< debug lambda
auto pass = [](auto& ctx){_pass(ctx)=true;}; //< debug lambda
auto fail = [](auto& ctx){_pass(ctx)=false;}; //< debug lambda
auto print = [](auto& ctx){ printer(_attr(ctx)); };
auto print_and_fail = [](auto& ctx){ printer(_attr(ctx)); _pass(ctx)=false;};
struct print_ex {
	std::string ex;
	print_ex(std::string_view s) : ex(s) {}
	template<typename C>
	void operator() (C&& c){std::cout << ex;print(std::forward<C>(c));}
};

//-------------- debug block

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

template<typename Obj, typename Value>
void empback_helper(Obj& obj, Value&& val) { obj.emplace_back(std::forward<Value>(val)); }
template<typename Obj, typename Value>
void empback_helper(std::reference_wrapper<Obj>& obj, Value&& val) { obj.get().emplace_back(std::forward<Value>(val)); }

auto set =       [](auto& ctx)  { _val(ctx) = _attr(ctx); };
auto set_name =  [](auto& ctx)  {_val(ctx).name = _attr(ctx); };
auto setrw_name =[](auto& ctx)  {_val(ctx).get().name = _attr(ctx); };
auto set_value = [](auto& ctx) {_val(ctx).value = _attr(ctx); };
auto set_ref =   [](auto& ctx) {block_set_ref_rw(_val(ctx),std::move(_attr(ctx)));};
auto set_cnt =   [](auto& ctx) {_val(ctx).cnt = _attr(ctx);};
auto set_op =    [](auto& ctx) {_val(ctx).op = _attr(ctx);};
auto set_left =  [](auto& ctx) {_val(ctx).left = _attr(ctx);};
auto set_right = [](auto& ctx) {_val(ctx).right = _attr(ctx);};

auto empback = [](auto& ctx){ empback_helper(_val(ctx),_attr(ctx)); };
auto empback_cnt = [](auto& ctx){block_add_op_rw(_val(ctx),std::move(_attr(ctx)));};
auto empback_names = [](auto& ctx) { _val(ctx).names.emplace_back(_attr(ctx)); };
auto empback_params = [](auto& ctx) { _val(ctx).params.emplace_back(_attr(ctx)); };
auto empback_to_unnamed = [](auto& c) { _val(c).unnamed_block().cnt.emplace_back(_attr(c)); };
auto empback_cnt_to_unnamed = [](auto& ctx) {
	auto& b = _val(ctx).unnamed_block();
	gram_traits::concat(add_and_get<block_t, gram_traits::types::out_string_t>(b), _attr(ctx)); };

auto peq = [](auto& ctx){ _val(ctx) += _attr(ctx); };
auto peq_cnt = [](auto&c){_val(c).cnt+=_attr(c);};

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
	if(std::holds_alternative<st_macro<str_t>>(*block.ref))
		ok = val == "endmacro";
	_pass(ctx) = ok;
};

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
	   var_name_rule[set_ref]
	>> x3::char_('(')
	>> -((quoted_string[empback_params] | var_name_rule[empback_params] /*| fnc_call_rule[empback_params]*/) % ',')
	>> ')' ]
;

const x3::rule<struct op_comment, st_comment<gram_traits::types::out_string_t>> op_comment = "comment";
const auto op_comment_def =
	  (spec_symbols[op_comment_is_start] >> spec_symbols[op_comment_is_end])
	| (spec_symbols[op_comment_is_start]
	>> +(gram_traits::char_[peq_cnt] >> !spec_symbols[op_comment_is_end])
	>> gram_traits::char_
	>> spec_symbols[op_comment_is_end])
	   | (
	       spec_symbols[op_comment_is_start]
	    >> gram_traits::char_[clear_cnt][peq_cnt]
	    >> spec_symbols[op_comment_is_end]
	    )
;

const x3::rule<struct op_out, st_out<gram_traits::types::out_string_t>> op_out = "out_operator";
const auto op_out_def =
	   spec_symbols[op_out_is_start]
	>> x3::skip(gram_traits::space)[
	     ( fnc_call_rule[set_ref] | quoted_string[set_ref] | var_name_rule[set_ref] )
	  >> -( '|' >> (fnc_call_rule[empback_params] | var_name_rule[empback_params]) % '|')
	>> spec_symbols[op_out_is_end] ] ;


const x3::rule<struct raw_text, gram_traits::types::out_string_t> raw_text = "raw_text";
const auto raw_text_def = +(gram_traits::char_[peq] >> (!(
	spec_symbols[op_term_is_start] >> *gram_traits::space >> x3::lit("endraw") >> *gram_traits::space >> spec_symbols[op_term_is_end]
	))) >> gram_traits::char_
;

const x3::rule<struct free_text, gram_traits::types::out_string_t> free_text = "free_text";
const auto free_text_def = 
	   !(
	   spec_symbols[op_out_is_start]
	   | spec_symbols[op_term_is_start]
	   )
	>> *(gram_traits::char_[peq]
		>> (!(
		   spec_symbols[op_out_is_start]
		   | spec_symbols[op_term_is_start]
		   ))
		   )
	>> gram_traits::char_
;

const x3::rule<struct value_term_tag, value_term<gram_traits::types::out_string_t>> value_term_r = "value_term";
const auto value_term_r_def = fnc_call_rule | quoted_string | var_name_rule;
struct value_term_tag {
	template<typename T, typename Iter, typename Context>
	inline void on_success(const Iter& first, const Iter& last, T& ast, Context& ctx)
	{
		auto beg = first;
		while(beg!=last) std::cout << *beg;
		std::cout << std::endl;
	}
};

const x3::rule<struct comparation_sym_tag, comparator> comparation_sym_op = "comparation_sym_op";
const auto comparation_sym_op_def = 
	  x3::lit("==") [setter(comparator::eq)]
	| x3::lit("<" ) [setter(comparator::less)]
	| x3::lit("=<") [setter(comparator::less_eq)]
	| x3::lit(">" ) [setter(comparator::more)]
	| x3::lit(">=") [setter(comparator::more_eq)]
;

const x3::rule<struct comparation_char_tag, comparator> comparation_char_op = "comparation_char_op";
const auto comparation_char_op_def = 
	  x3::lit("is")   [setter(comparator::eq)]
	| x3::lit("less") [setter(comparator::less)]
	| x3::lit("leq")  [setter(comparator::less_eq)]
	| x3::lit("more") [setter(comparator::more)]
	| x3::lit("meq")  [setter(comparator::more_eq)]
;

const x3::rule<struct comparator_tag, comparator> comparator_r = "comparator";
const auto comparator_r_def = 
	  ( *gram_traits::space >> comparation_sym_op [set] >> *gram_traits::space)
	| ( +gram_traits::space >> comparation_char_op[set] >> +gram_traits::space)
	;

const x3::rule<struct op_for, st_for<gram_traits::types::out_string_t>> op_for = "op_for";
const auto op_for_def =
	   *gram_traits::space
	>> x3::lit("for") >> +gram_traits::space
	>> (single_var_name[empback_params] % ',') >> +gram_traits::space
	>> x3::lit("in") >> +gram_traits::space
	>> value_term_r[set_ref] >> *gram_traits::space
	;

const x3::rule<struct op_macro_param, macro_parameter<gram_traits::types::out_string_t>> op_macro_param = "op_macro_param";
const x3::rule<struct op_macro, st_macro<gram_traits::types::out_string_t>> op_macro = "op_macro";
const auto op_macro_param_def = single_var_name[set_name] >> -(x3::lit("=") >>  value_term_r[set_value]) ;
const auto op_macro_def = x3::skip(gram_traits::space)[
	   x3::lit("macro")
	>> single_var_name[set_name]
	>> x3::lit("(")
	>> -(op_macro_param[empback_params] % ',')
	>> x3::lit(")")] >> *gram_traits::space ;

const x3::rule<struct op_if, st_if<gram_traits::types::out_string_t>> op_if = "op_if";
const auto op_if_def =
	   *gram_traits::space
	>> x3::lit("if") >> +gram_traits::space
	>> value_term_r[set_left]
	>> -(comparator_r[set_op] >> value_term_r[set_right])
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

const x3::rule<struct op_set_tag, cppjinja::set_op<gram_traits::types::out_string_t>> op_set_r = "op_set";
const auto op_set_r_def = x3::skip(gram_traits::space)[
	   spec_symbols[op_term_is_start]
	>> x3::lit("set")
	>> (single_var_name[empback_names] % ',')
	>> x3::lit("=")
	>> value_term_r[set_value]
	>> spec_symbols[op_term_is_end]];

const x3::rule<struct op_extends_tag, extends_op<gram_traits::types::out_string_t>> op_extends_r = "extends_op";
const auto op_extends_r_def = x3::skip(gram_traits::space)[
	   spec_symbols[op_term_is_start]
	>> x3::lit("extends")
	>> quoted_string [set_name]
	>> spec_symbols[op_term_is_end]];

const x3::rule<struct op_raw, st_raw> op_raw = "op_raw";
const auto op_raw_def = *gram_traits::space[nop] >> x3::lit("raw") >> *gram_traits::space[nop];

const x3::rule<struct named_block, std::string> named_block = "named_block";
const auto named_block_def = *gram_traits::space >> x3::lit("block")
	>> +gram_traits::space >> single_var_name[set] >> *gram_traits::space ;

const x3::rule<struct jtmpl_tag, jtmpl> jtmpl_rule = "jtmpl_rule";
const auto jtmpl_rule_def = *gram_traits::space[nop] >> x3::lit("template") >> -(+gram_traits::space >> single_var_name[set_name]) ;

const x3::rule<struct jtmpl_end_tag> jtmpl_end_rule = "jtmpl_end_rule";
const auto jtmpl_end_rule_def = x3::skip(gram_traits::space)[ spec_symbols [op_term_is_start] >> x3::lit("endtemplate") >> spec_symbols [op_term_is_end] ];

const x3::rule<struct block_content_tag, block_content<gram_traits::types::out_string_t>> block_content_r = "block_content";
const x3::rule<struct block_content_vec_tag, block_content<gram_traits::types::out_string_t>> block_content_vec_r = "block_content";
const x3::rule<struct block_r1, block_t> block_r1 = "block_r1";
const x3::rule<struct block_tag, std::reference_wrapper<gram_traits::types::block_t>> block = "block";
const x3::rule<struct block_jtmpl, block_t> block_jtmpl_rule = "block_jtmpl";

const auto block_jtmpl_rule_def =
	   spec_symbols [op_term_is_start]
	>> *gram_traits::space[nop]
	>> jtmpl_rule   [set_ref]
	>> *gram_traits::space[nop]
	>> spec_symbols [op_term_is_end]
	>> +( !jtmpl_end_rule >> block_content_r[empback_cnt] )
	>> jtmpl_end_rule
;
 
const auto block_r1_def =
	   (spec_symbols  [op_term_is_start]
	>> ( op_if        [set_ref]
	   | op_macro     [set_ref]
	   | op_for       [set_ref]
	   | op_raw       [set_ref]
	   | named_block  [set_ref]
	   )  )
	>  spec_symbols  [op_term_is_end]
	>> -(raw_text[cnt_if_raw] | block_content_vec_r[empback_cnt])
	>> x3::skip(gram_traits::space)[
	   spec_symbols[op_term_is_start]
	>> (+x3::alnum)[op_term_check_end]
	>> spec_symbols[op_term_is_end]
	]
	;
const auto block_def =
	(+block_jtmpl_rule           [ empback_cnt ]
	)| (*(
		  op_out             [ empback_cnt ]
		| op_set_r           [ empback_cnt ]
		| op_extends_r       [ empback_cnt ]
		| block_r1           [ empback_cnt ]
		| gram_traits::char_ [ block_add_content ]
	));
const auto block_content_vec_r_def = +block_content_r;
const auto block_content_r_def = (
	  op_out    [ set ]
	| op_comment[ set ]
	| op_set_r  [ set ]
	| op_extends_r [ set ]
	| block_r1  [ set ]
	| free_text [ set ]
	) ;



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
BOOST_SPIRIT_DEFINE(named_block)
BOOST_SPIRIT_DEFINE(op_macro)
BOOST_SPIRIT_DEFINE(op_macro_param)
BOOST_SPIRIT_DEFINE(jtmpl_rule)
BOOST_SPIRIT_DEFINE(block_jtmpl_rule)
BOOST_SPIRIT_DEFINE(block_content_vec_r)
BOOST_SPIRIT_DEFINE(jtmpl_end_rule)
BOOST_SPIRIT_DEFINE(comparation_sym_op)
BOOST_SPIRIT_DEFINE(comparation_char_op)
BOOST_SPIRIT_DEFINE(op_set_r)
BOOST_SPIRIT_DEFINE(op_extends_r)

#ifndef FILE_INLINING
} // namespace cppjinja::deubg
#endif

