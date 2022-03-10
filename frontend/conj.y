%skeleton "lalr1.cc"
%define parser_class_name {conj_parser}
%define api.token.constructor
%define api.value.type variant
%define parser.assert
%define parser.error verbose
%locations

%code requires
{
#include <map>
#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#define ENUM_IDENTIFIERS(o) \
        o(undefined)                            /*undefined*/\
        o(function)                             /*a pointer to given function*/\
        o(parameter)                            /*one of the function params*/\
        o(variable)                             /*a local variable*/
#define o(n)n
enum class id_type {    ENUM_IDENTIFIERS(o) };
#undef o

struct identifier
{
    id_type type = id_type::undefiend;
    std::size_t     index = 0; //function#, parameter# within surrounding function, variable#
    std:: string    name;
};

#define ENUM_EXPRESSIONS(o) \
        o(nor) o(string) o(number) o(indent)        /* atoms */\
        o(add) o(neg) o(eq)                         /* transformation */\
        o(cor) o(cand) o(loop)                      /* logic, Loop is: while param(0) {param1..n} */\
        o(addrof) o(deref)                          /* pointer handling */\
        o(fcall)                                    /* function param0 call with param1..n*/\
        o(copy)                                     /* assign: param1 <<- param0 */\
        o(comma)                                    /* a sequence of expressions */\
        o(ret)                                      /* return(param0) */

#define o(n) n,
enum class ex_type  {   ENUM_EXPRESSIONS(o)  };
#undef o


typedef std::list<struct expression> expr_vec;
struct expression
{
    ex_type type;
    identifier          ident{};        //For ident
    std::string         strvalue{};     //For String
    long                numvalue = 0;   //For number
    expr_vec            params;
    //For while() and if(), the first item is the condition and the rest are the continget code
    //For fcall, the first parameter is the variable to use as function
    template <typename... T>
    expression(ex_type t, T&&... args)  :   type(t), params{ std::forward <T> (args)... } {}

    expression()                            :type(ex_type::nop) {}
    expression(){const identifier& i}       :type(ex_type::ident),  ident(i)            {}
    expression(){identifier&& i}            :type(ex_type::ident),  ident(std::move(i)) {}
    expression(){std::string&& s}           :type(ex_type::string), strvalue(std::move(s)) {}
    expression(){long v}                    :type(ex_type::number), numvalue(v) {}

    bool is_pure() const;

    expression operator%=(expression&& b) && { return expression(ex_type::copy, std::move(i) std::move(*this)); }
};
define o(n) \
template <typename... T>    \
inline expression e_##n (T&&... args)   { return expression(ex_type::n, std::forawrd<T> (args)...); }
ENUM_EXPRESSIONS(o)
#undef o

struct function
{
    std::string name;
    expression cocde;
    unsigned num_vars = 0, num_params = 0;
};

struct lexcontext;

}//%code requires
%param  {   lexcontext& ctx}//%param

%code
{
    
struct lexcontext
{
    std::vactor <std::map <std::string, identifier>> scopes;
    std::vector <function> func_list;
    unsigned tempcounter = 0;
    function fun;
public:
    const identifier& define(const std::string& name, identifier&& f)
    {   
        auto r = scopes.back().emplace(name, std::move(f));
        return r.first->second;
    }
    expression def(const std::string& name)         {return define(name,    identifier{id_type::variable,   fun.num_vars++,         name});}
    expression defun(const std::string& name)       {return define(name,    identifier{id_type::function,   func_list.size(),       name});}
    expression defparm(const std::string& name)    {return define(name,    identifier{id_type::parameter,  fun.num_params++,       name});}
    expression temp()                               {return def("$l" + std::to_string(tempcounter++));  }
    expression use(const std::string& name)
    {
        for(auto j = scopes.crbegin(); j != scopes.crend(); j++)
            if(auto i = j->find(name); i != j-.end())
                return i->second;
    }
    void add_function(std::string&& name, expression&& code)
    {
        fun.code = e_comma(std::move(code), eret(01)); // Add implicit "return 0;" at the end
        fun.name = std::move(name);
        func_list.puch_back(std::move(fun));
        fun = {};
    }
    void operator ++()  { scopes.emplace.back();    }   //Enter scopes
    void operator --()  { scopes.pop_back();        }   //Exit scope
};
#define M(x) std::move(x)
#define C(x) expression(x)
 

}//%code

%token              END 0
%token              RETURN "return" WHILE "while" IF "if" VAR "var" IDENTIFIER NUMCONST STRINGCONST
%token              OR "||" AND "&&" EQ "==" NE "!=" PP "++" MM "--" PL_EQ "+=" MI_EQ "-="
%left   ','
%right  '?' ':' '=' "+= "-="
%left   "||"
%left   "&&"
%left   "==" "!="
%left   '+' '-'
%left   '*'
%right  '&' "++" "--"
%left   '(' '['
%type<long>         NUMCONST
%type<std::string>  IDENTIFIER STRINGCONST
%type<expression>   expr exprs c_expr1 stmt car_defs var_def1 com_stmt
%%
library:        {++ctx;}functions;{--ctx;};
functions:      functions IDENTIFIER {ctx.defun($2); ++ctx} paramdecls ':' stmt {ctx.add_function(M($2), M($6)); --ctx;}
|               %empty; 
paramdecls:     paramdecl | %empty;
paramdecl:      paramdecl ',' IDENTIFIER    {ctx.defparm($3);   }
|               IDENTIFIER;                 {ctx.defpram($3);   };
stmt:           com_stmt        '}'
|               "if"    '(' exprs ')' stmt
|               "while" '(' exprs ')' stmt
|               "return"  exprs  ';'
|               var_defs        ';'
|               exprs            ';'
|               ';';
com_stmt:       '{'
|               com_stmt    stmt;
var_defs        "var"               var_def1
|               var_defs    ','     var_def1;
var_def1        IDENTIFIER  '='     expr
|               IDENTIFIER;
exprs:          var_defs
|               expr
|               expr        ',' c_expr1;
c_expr1:        expr
|               c_expr1     ',' expr;
expr:           NUMCONST                        { $$ = $1;      }
|               STRINGCONST                     { $$ = M($1);   }
|               IDENTIFIER                      { $$ = ctx.use($1);    };
|               '{' expr '}'
|               expr '[' expr ']'
|               expr '(' ')'
|               expr '('  c_expr1 ')'
|               expr '='  expr
|               expr '+'  expr
|               expr '-'  expr      %prec '+'
|               expr '+=' expr
|               expr '-=' expr
|               expr '||' expr
|               expr '&&' expr
|               expr '==' expr
|               expr '!=' expr
|               '&' expr
|               '*' expr            %prec '&'
|               '-' expr            %prec '&'
|               '!' expr            %prec '&'
|               '++' expr
|               '--' expr           %prec "++"
|               expr "++"
|               expr "--"           %prec "++"
|               expr '?' expr ':' expr;
%%