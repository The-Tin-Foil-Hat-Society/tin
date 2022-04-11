%{
#include <stdio.h>
#include "ast.h"
#include "parser.tab.h"
%}

%option reentrant bison-bridge noyywrap
%option warn nodefault yylineno

/* match string literals */
string      \"[^\"\n]*\"
/* match identifiers */
identifier  [a-zA-Z_][a-zA-Z0-9_]*
/* match floats */
float       [-]?[0-9]+\.[0-9]+
/* match integers */
integer     [-]?[0-9]+
/* match multiline comments */
comment		[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]

%%
{comment}				;					/* discard comments */
"//".*                  ;           

"i8"                    { return I8;    }   /* match data types */
"u8"                    { return U8;    }
"i16"                   { return I16;   }
"u16"                   { return U16;   }
"i32"                   { return I32;   }
"u32"                   { return U32;   }
"i64"                   { return I64;   }
"u64"                   { return U64;   }
"f32"                   { return F32;   }
"f64"                   { return F64;   }
"bool"                  { return BOOL;  }
"void"                  { return VOID;  }
"ptr"                   { return PTR;   }
"ref"                   { return REF;   }

"false"                 { 
    ast_node* node = ast_new(AstBoolLit);
    node->value.boolean = false;
    *yylval = node;
    return BOOL_LIT;
}
"true"                  { 
    ast_node* node = ast_new(AstBoolLit);
    node->value.boolean = true;
    *yylval = node;
    return BOOL_LIT;
}

"alloc"                 { return ALLOC;   }   /* match keywords */
"asm"                   { return ASM;     }
"break"                 { return BREAK;   }
"continue"              { return CONT;    }
"for"                   { return FOR;     }
"free"                  { return FREE;    }
"fn"                    { return FUNC;    }
"if"                    { return IF;      }
"else"                  { return ELSE;    }
"include"               { return INCLUDE; }
"input"                 { return INPUT;   }
"print"                 { return PRINT;   }
"return"                { return RETURN;  }
"while"                 { return WHILE;   }

"="                     { return IS;  }   /* match operators */
"+"                     { return ADD; }
"-"                     { return SUB; }
"*"                     { return MUL; }
"/"                     { return DIV; }
"**"                    { return POW; }
"%"                     { return MOD; }
"<<<"                   { return ROTL; }
">>>"                   { return ROTR; }
"<<"                    { return SHIFTL; }
">>"                    { return SHIFTR; }
"<"                     { return LT;  }
">"                     { return GT;  }
"<="                    { return LE;  }
">="                    { return GE;  }
"=="                    { return EQ;  }
"!="                    { return NE;  }
"&&"                    { return AND; }
"!"                     { return NOT; }
"||"                    { return OR;  }
"&"                     { return BAND; }
"|"                     { return BOR;  }
"^"                     { return BXOR; }

";"                     { return SEMI_COLON;    }
"::"                    { return DOUBLE_COLON;  }
":"                     { return COLON;         }
","                     { return COMMA;         }
"("                     { return BRACKET_L;     }
")"                     { return BRACKET_R;     }
"{"                     { return BRACE_L;       }
"}"                     { return BRACE_R;       }
"["                     { return SQUARE_BRACKET_L; }
"]"                     { return SQUARE_BRACKET_R; }

{string} { 
    ast_node* node = ast_new(AstStringLit);
    node->value.string = strdup(yytext + 1);
    node->value.string[strlen(node->value.string)-1] = '\0'; /* strip quotes at start and end */
    *yylval = node;
    return STRING;
}   /* match before identifier, otherwise the string inside the quotes will be matched as an identifier*/
{identifier} {
    ast_node* node = ast_new(AstIdentifier);
    node->value.string = strdup(yytext);
    *yylval = node;
    return IDENTIFIER;
}
{float} {
    ast_node* node = ast_new(AstFloatLit);
    ast_node* data_type_node = ast_new(AstDataType);

    node->value.floating = strtod(yytext, 0);
    data_type_node->value.dtype = data_type_new("f64");

    data_type_node->value.dtype->pointer_level = 0;
    ast_add_child(node, data_type_node);

    *yylval = node;
    return FLOAT;
}
{integer} {
    ast_node* node = ast_new(AstIntegerLit);
    ast_node* data_type_node = ast_new(AstDataType);

    bool _signed = yytext[0] == '-';
    if (_signed)
    {
        node->value.integer = strtoll(yytext, 0, 10);
        data_type_node->value.dtype = data_type_new("i64");
    }
    else
    {
        node->value.integer = strtoull(yytext, 0, 10);
        data_type_node->value.dtype = data_type_new("u64");
    }

    data_type_node->value.dtype->pointer_level = 0;
    ast_add_child(node, data_type_node);

    *yylval = node;
    return INTEGER;
}


<<EOF>>                 		{ return EOF;   }   /* consume end-of-file */
[ \t\n]+                		;                   /* discard whitespace */
.                       		;                   /* discard bad characters */

%%
