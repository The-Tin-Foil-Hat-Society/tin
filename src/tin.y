/* if you're using vscode and a yacc syntax highlighter, ignore the "Semantic value used inside actions but has not declared the type" error, it's a bug in the extension,
   in general ignore vscode error highlighting, rely on gcc output for warnings and errors */

%{
#include <string.h>
#include "module.h"
#include "ast.h"
#include "parser.tab.h"
#include "lex.yy.h"

/* #define YYDEBUG 1 */

void yyerror (yyscan_t* locp, module* mod, const char* msg);

%}

%code requires
{
#include "module.h"
#include "ast.h"
}

%define api.pure full
%define parse.error verbose
%define api.value.type { ast_node* } 
%lex-param {void* scanner}
%parse-param {void* scanner}{module* mod}

%token IDENTIFIER FLOAT INTEGER STRING
%token ALLOC ASM BREAK CONT FOR FREE FUNC IF ELSE INCLUDE INPUT PRINT RETURN WHILE
%token I8 U8 I16 U16 I32 U32 I64 U64 F32 F64 VOID PTR REF BOOL BOOL_LIT
%token IS ADD SUB MUL DIV POW MOD ROTL ROTR SHIFTL SHIFTR LT GT LE GE EQ NE AND NOT OR BAND BOR BXOR
%token SEMI_COLON COLON DOUBLE_COLON COMMA BRACKET_L BRACKET_R BRACE_L BRACE_R SQUARE_BRACKET_L SQUARE_BRACKET_R 

/* operator precedence */
%left BOR
%left BXOR
%left BAND
%left SHIFTL SHIFTR
%left ROTL ROTR
%left ADD SUB
%left DIV MUL MOD
%left POW
%left BRACKET_L

/* get rid of s/r conflict for dangling else's */
%precedence FAKE_ELSE
%precedence ELSE

%start program

%%

program
    : { $$ = mod->ast_root; $$->src_line = module_get_src_line(mod, 0); } /* passed from main */
    | program statement { ast_add_child($1, $2); if ($2->src_line == 0) { $2->src_line = module_get_src_line(mod, yyget_lineno(scanner)); } }
    | program function  { ast_add_child($1, $2); }
    ;

data_type
    : I8    { $$ = ast_new(AstDataType); $$->value.dtype = data_type_new("i8"); }
    | U8    { $$ = ast_new(AstDataType); $$->value.dtype = data_type_new("u8"); }
    | I16   { $$ = ast_new(AstDataType); $$->value.dtype = data_type_new("i16"); }
    | U16   { $$ = ast_new(AstDataType); $$->value.dtype = data_type_new("u16"); }
    | I32   { $$ = ast_new(AstDataType); $$->value.dtype = data_type_new("i32"); }
    | U32   { $$ = ast_new(AstDataType); $$->value.dtype = data_type_new("u32"); }
    | I64   { $$ = ast_new(AstDataType); $$->value.dtype = data_type_new("i64"); }
    | U64   { $$ = ast_new(AstDataType); $$->value.dtype = data_type_new("u64"); }
    | F32   { $$ = ast_new(AstDataType); $$->value.dtype = data_type_new("f32"); }
    | F64   { $$ = ast_new(AstDataType); $$->value.dtype = data_type_new("f64"); }
    | BOOL  { $$ = ast_new(AstDataType); $$->value.dtype = data_type_new("bool"); }
    | VOID  { $$ = ast_new(AstDataType); $$->value.dtype = data_type_new("void"); }
    | PTR data_type { $$ = $2; $$->value.dtype->pointer_level += 1; } 
    ;

simple_identifier
    : IDENTIFIER { $$ = $1; } 
    | IDENTIFIER DOUBLE_COLON simple_identifier { $$ = $3; $1->type = AstNamespace; ast_add_child($$, $1); } 
    ;

identifier
    : simple_identifier { $$ == $1; }
    | MUL identifier { $$ = $2; ast_add_child($$, ast_new(AstReference)); }
    | REF identifier { $$ = $2; ast_add_child($$, ast_new(AstDereference)); }
    ;

simple_expression
    : INTEGER { $$ = yylval; }
    | FLOAT { $$ = yylval; }
    | STRING { $$ = yylval; } 
    | BOOL_LIT { $$ = yylval; }
    | func_call { $$ = $1; }
    | identifier { $$ = $1; } 
    | identifier SQUARE_BRACKET_L expression SQUARE_BRACKET_R { ast_node* offset_node = ast_new(AstOffset); ast_add_child(offset_node, $3); ast_add_child($$, offset_node); } /* a[x+1] */
    | BRACKET_L conditional_expression BRACKET_R { $$ = $2; }
    | NOT simple_expression { $$ = ast_new(AstNot); ast_add_child($$, $2); }
    ;

expression
    : simple_expression { $$ = $1; }
    | expression MOD expression { $$ = ast_new(AstMod); ast_add_child($$, $1); ast_add_child($$, $3); }
    | expression POW expression { $$ = ast_new(AstPow); ast_add_child($$, $1); ast_add_child($$, $3); }
    | expression DIV expression { $$ = ast_new(AstDiv); ast_add_child($$, $1); ast_add_child($$, $3); }
    | expression MUL expression { $$ = ast_new(AstMul); ast_add_child($$, $1); ast_add_child($$, $3); }
    | expression ADD expression { $$ = ast_new(AstAdd); ast_add_child($$, $1); ast_add_child($$, $3); }
    | expression SUB expression { $$ = ast_new(AstSub); ast_add_child($$, $1); ast_add_child($$, $3); }
    | expression BAND expression   { $$ = ast_new(AstBitwiseAnd);  ast_add_child($$, $1); ast_add_child($$, $3); }
    | expression BOR expression    { $$ = ast_new(AstBitwiseOr);   ast_add_child($$, $1); ast_add_child($$, $3); }
    | expression BXOR expression   { $$ = ast_new(AstBitwiseXor);  ast_add_child($$, $1); ast_add_child($$, $3); }
    | expression SHIFTL expression { $$ = ast_new(AstShiftLeft);   ast_add_child($$, $1); ast_add_child($$, $3); }
    | expression SHIFTR expression { $$ = ast_new(AstShiftRight);  ast_add_child($$, $1); ast_add_child($$, $3); }
    ;

/* TODO: these and other complex rules and abstractions (i.e. circular shifts, for loops) could be written using macros once those are implemented */
operator_assignment
    : identifier MOD IS expression { $$ = ast_new(AstAssignment); ast_add_child($$, $1); ast_node* expression = ast_new(AstMod); ast_add_child(expression, ast_copy($1)); ast_add_child(expression, $4); ast_add_child($$, expression); }
    | identifier POW IS expression { $$ = ast_new(AstAssignment); ast_add_child($$, $1); ast_node* expression = ast_new(AstPow); ast_add_child(expression, ast_copy($1)); ast_add_child(expression, $4); ast_add_child($$, expression); }
    | identifier DIV IS expression { $$ = ast_new(AstAssignment); ast_add_child($$, $1); ast_node* expression = ast_new(AstDiv); ast_add_child(expression, ast_copy($1)); ast_add_child(expression, $4); ast_add_child($$, expression); }
    | identifier MUL IS expression { $$ = ast_new(AstAssignment); ast_add_child($$, $1); ast_node* expression = ast_new(AstMul); ast_add_child(expression, ast_copy($1)); ast_add_child(expression, $4); ast_add_child($$, expression); }
    | identifier ADD IS expression { $$ = ast_new(AstAssignment); ast_add_child($$, $1); ast_node* expression = ast_new(AstAdd); ast_add_child(expression, ast_copy($1)); ast_add_child(expression, $4); ast_add_child($$, expression); }
    | identifier SUB IS expression { $$ = ast_new(AstAssignment); ast_add_child($$, $1); ast_node* expression = ast_new(AstSub); ast_add_child(expression, ast_copy($1)); ast_add_child(expression, $4); ast_add_child($$, expression); }
    | identifier BAND IS expression   { $$ = ast_new(AstAssignment); ast_add_child($$, $1); ast_node* expression = ast_new(AstBitwiseAnd);  ast_add_child(expression, ast_copy($1)); ast_add_child(expression, $4); ast_add_child($$, expression); }
    | identifier BOR IS expression    { $$ = ast_new(AstAssignment); ast_add_child($$, $1); ast_node* expression = ast_new(AstBitwiseOr);   ast_add_child(expression, ast_copy($1)); ast_add_child(expression, $4); ast_add_child($$, expression); }
    | identifier BXOR IS expression   { $$ = ast_new(AstAssignment); ast_add_child($$, $1); ast_node* expression = ast_new(AstBitwiseXor);  ast_add_child(expression, ast_copy($1)); ast_add_child(expression, $4); ast_add_child($$, expression); }
    | identifier SHIFTL IS expression { $$ = ast_new(AstAssignment); ast_add_child($$, $1); ast_node* expression = ast_new(AstShiftLeft);   ast_add_child(expression, ast_copy($1)); ast_add_child(expression, $4); ast_add_child($$, expression); }
    | identifier SHIFTR IS expression { $$ = ast_new(AstAssignment); ast_add_child($$, $1); ast_node* expression = ast_new(AstShiftRight);  ast_add_child(expression, ast_copy($1)); ast_add_child(expression, $4); ast_add_child($$, expression); }
    ;

assignment
    : operator_assignment      { $$ = $1; }
    | identifier IS expression { $$ = ast_new(AstAssignment); ast_add_child($$, $1); ast_add_child($$, $3); }
    | definition IS expression { $$ = ast_new(AstAssignment); ast_add_child($$, $1); ast_add_child($$, $3); }
    ;

definition
    : data_type identifier { $$ = ast_new(AstDefinition); ast_add_child($$, $1); ast_add_child($$, $2); }
    ;

relational_expression
    : expression
    | relational_expression LT expression  { $$ = ast_new(AstLessThan); ast_add_child($$, $1); ast_add_child($$, $3); }
    | relational_expression GT expression  { $$ = ast_new(AstGreaterThan); ast_add_child($$, $1); ast_add_child($$, $3); }
    | relational_expression LE expression  { $$ = ast_new(AstLessThanOrEqual); ast_add_child($$, $1); ast_add_child($$, $3); }
    | relational_expression GE expression  { $$ = ast_new(AstGreaterThanOrEqual); ast_add_child($$, $1); ast_add_child($$, $3); }
    ;

equality_expression
    : relational_expression  { $$ = $1; }
    | equality_expression EQ relational_expression  { $$ = ast_new(AstEqual); ast_add_child($$, $1); ast_add_child($$, $3); }
    | equality_expression NE relational_expression  { $$ = ast_new(AstNotEqual); ast_add_child($$, $1); ast_add_child($$, $3); }
    ;

logical_and_expression
    : equality_expression  { $$ = $1; }
    | logical_and_expression AND equality_expression  { $$ = ast_new(AstAnd); ast_add_child($$, $1); ast_add_child($$, $3); }
    ;

logical_or_expression 
    : logical_and_expression  { $$ = $1; }
    | logical_or_expression OR logical_and_expression  { $$ = ast_new(AstOr); ast_add_child($$, $1); ast_add_child($$, $3); }
    ;

conditional_expression
    : logical_or_expression  { $$ = $1; }
    ;

if
    : IF    { $$ = ast_new(AstIf); $$->src_line = module_get_src_line(mod, yyget_lineno(scanner)); }
    ; /* make the node here so we can get the first source line */

if_statement
    : if BRACKET_L conditional_expression BRACKET_R statement ELSE statement  { $$ = $1; ast_add_child($$, $3); ast_add_child($$, $5); ast_add_child($$, $7); }
	| if BRACKET_L conditional_expression BRACKET_R statement %prec FAKE_ELSE { $$ = $1; ast_add_child($1, $3); ast_add_child($$, $5); }
    ;

for
    : FOR { $$ = ast_new(AstFor); $$->src_line = module_get_src_line(mod, yyget_lineno(scanner)); }
    ;

for_statement
    : for BRACKET_L assignment SEMI_COLON conditional_expression SEMI_COLON assignment BRACKET_R scope { $$ = $1; ast_add_child($$, $3); ast_add_child($$, $5); ast_add_child($$, $7); ast_add_child($$, $9); }
    ;

while
    : WHILE { $$ = ast_new(AstWhile); $$->src_line = module_get_src_line(mod, yyget_lineno(scanner)); }
    ;

/* TODO: do while? not neccessary though */
while_statement
    : while BRACKET_L conditional_expression BRACKET_R scope { $$ = $1; ast_add_child($$, $3); ast_add_child($$, $5); }
    ;

jump_statement
    : CONT SEMI_COLON               { $$ = ast_new(AstContinue); }
	| BREAK SEMI_COLON              { $$ = ast_new(AstBreak); }
	| RETURN SEMI_COLON             { $$ = ast_new(AstReturn); }
	| RETURN expression SEMI_COLON  { $$ = ast_new(AstReturn); ast_add_child($$, $2); }
	;

statement
    : assignment SEMI_COLON     { $$ = $1; }
    | definition SEMI_COLON     { $$ = $1; }
    | func_call SEMI_COLON      { $$ = $1; }
    | if_statement              { $$ = $1; }
    | for_statement             { $$ = $1; }
    | while_statement           { $$ = $1; }
    | jump_statement            { $$ = $1; }
    | scope                     { $$ = $1; }
    | ALLOC identifier expression SEMI_COLON    { $$ = ast_new(AstAlloc); ast_add_child($$, $2); ast_add_child($$, $3); }
    | ASM STRING SEMI_COLON         { $$ = ast_new(AstAsm);  $$->value.string = strdup($2->value.string); ast_free($2, 0); }
    | FREE expression SEMI_COLON    { $$ = ast_new(AstFree); ast_add_child($$, $2); }
    | INCLUDE STRING                { $$ = ast_new(AstInclude); $$->value.string = strdup($2->value.string); ast_free($2, 0); }
    | INPUT identifier SEMI_COLON   { $$ = ast_new(AstInput); ast_add_child($$, $2); }
    | PRINT expression SEMI_COLON   { $$ = ast_new(AstPrint); ast_add_child($$, $2); }
    ;

scope_inner
    : { $$ = ast_new(AstScope); }
    | scope_inner statement { ast_add_child($1, $2); if ($2->src_line == 0) { $2->src_line = module_get_src_line(mod, yyget_lineno(scanner)); } }
    ;

scope 
    : BRACE_L scope_inner BRACE_R { $$ = $2; }
    ;

argument_list
    : expression { $$ = ast_new(AstArgumentList); ast_add_child($$, $1); }
    | argument_list COMMA expression    { ast_add_child($1, $3); }
    ;

func_call
    : identifier BRACKET_L BRACKET_R    { $$ = ast_new(AstFunctionCall); ast_add_child($$, $1); ast_add_child($$, ast_new(AstArgumentList)); }
    | identifier BRACKET_L argument_list BRACKET_R  { $$ = ast_new(AstFunctionCall); ast_add_child($$, $1); ast_add_child($$, $3); }
    ;

definition_list
    : definition { $$ = ast_new(AstDefinitionList); ast_add_child($$, $1); }
    | definition_list COMMA definition  { ast_add_child($1, $3); }
    ;

func
    : FUNC { $$ = ast_new(AstFunction); $$->src_line = module_get_src_line(mod, yyget_lineno(scanner)); } 
    ; 

function
    : func definition scope { $$ = $1; ast_add_child($$, $2); ast_add_child($$, ast_new(AstDefinitionList)); ast_add_child($$, $3); }
    | func definition BRACKET_L definition_list BRACKET_R scope { $$ = $1; ast_add_child($$, $2); ast_add_child($$, $4); ast_add_child($$, $6); }
    ;

%%

#include <stdio.h>

void yyerror(yyscan_t* scanner_, module* mod, const char* msg)
{
    int linenno = yyget_lineno(scanner_);
    char* src_line = module_get_src_line(mod, linenno);

    if (src_line != 0)
    {
        fprintf(stderr, "%s\n%s\n", src_line, msg);
        free(src_line);
    }
    else
    {
        fprintf(stderr, "line %d: %s\n", linenno, msg);
    }
}