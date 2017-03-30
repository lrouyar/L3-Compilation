%{
	#include <stdio.h>
	#include <string.h>
	#include "include/Function.h"
	#include "include/Variable.h"
	#include "include/Env.h"

	int yyerror(char *s);
	int yylex();
	int yylineno;
%}

	%union {
		char* identity;
		int constant;
        struct Type* type;
        struct Variable* variable;
        struct Env* env;
        struct FuncDisclaimer* funcDisc;
        struct Func* function;
	}

	%token<identity>V
	%token<constant>I
	%token NFon NPro NewAr Sk T_ar T_boo T_int Def Dep Af True False Se If Th El Var Wh Do Pl Mo Mu And Or Not Lt Eq OPar CPar OBracket CBracket OBrace CBrace Comma Colon SpaceTab
	%start MP


  %type<type> TP
  %type<variable> Argt
  %type<env> L_vart L_argt L_argtnn
  %type<funcDisc> D_entf D_entp
  %type<function> D


  %left Se
	%left Pl Mo
	%left Mu

%%


MP: L_vart LD C {}

E: E Pl E {}
  | E Mo E {}
  | E Mu E {}
  | E Or E {}
  | E Lt E {}
  | E Eq E {}
  | E And E {}
  | Not E {}
  | OPar E CPar {}
  | I {}
  | V {}
  | True {}
  | False {}
  | V OPar L_args CPar {}
  | NewAr TP OBracket E CBracket {}
  | Et {}

Et: V OBracket E CBracket {}
  | Et OBracket E CBracket {}

C: C Se C {}
  | Et Af E {}
  | V Af E {}
  | Sk {}
  | OBrace C CBrace {}
  | If E Th C El C {}
  | Wh E Do C {}
  | V OPar L_args CPar {}

L_args: %empty {}
  | L_argsnn {}

L_argsnn: E {}
  | E Comma L_argsnn {}

L_argt: %empty { $$ = VarDisclaimList_init();}
  | L_argtnn { $$ = $1;}

L_argtnn: Argt { $$ = Env_init();}
  | L_argtnn Comma Argt { $$ = $1; VarDisclaimList_append($$, $3); }

Argt: V Colon TP { $$ = VarDisclaim_init($1, $3); }

TP: T_boo { $$ = Type_init(BOOL, 0); }
  | T_int  { $$ = Type_init(INT, 0); }
  | T_ar TP { $$ = Type_init(ARRAY, $2); }

L_vart: %empty {  $$ = VarDisclaimList_init(); }
  | L_vartnn { $$ = $1;}

L_vartnn: Var Argt { $$ = VarDisclaimList_init(); VarDisclaimList_append($$, $2); }
  | L_vartnn Comma Var Argt { $$ = $1; VarDisclaimList_append($$, $4); }

D_entp: Dep NPro OPar L_argt CPar {}

D_entf: Def NFon OPar L_argt CPar Colon TP {}

D: D_entp L_vart C {}
  | D_entf L_vart C {}

LD: %empty {}
  | LD D {}

%%

int main()
{
	yyparse();
}


int yyerror(char *s)
{
 fprintf(stderr, "error: %s near %d\n", s, yylineno);
}
