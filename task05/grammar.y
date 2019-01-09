/***********************************************/
/*      File: grammar.y                        */
/*      Author: Yuan Haobo                     */
/*      Contact: yuanhaobo@whu.edu.cn          */
/*      License: Copyright (c) 2018 Yuan Haobo */
/***********************************************/
%{
/* the grammar of lambda expression */
#include "tree.h"
#include "type.h"
  
void yyerror (char const *);
/************************************/

char *name_env[MAX_ENV] = {"+", "-", "*", "/", "=", "<"};

AST *ast_env[MAX_ENV];

int current = INIT_POS;

#define YYSTYPE AST *
FILE *texfile;
int is_decl = 0;
%}
%nonassoc '.'
%left THEN ELSE
%left INT LET ID IF FI '(' '@'
%left CONCAT
%%

lines : decl 

| lines decl 
;
 
decl : LET {is_decl = 1;} ID '=' expr ';' {
  Type_ptr type = typing(NULL, $5, current);
  ast_env[current] = ($5);
  global_type_env [current] = storetype(type);
  name_env[current++] = (char *) $3 -> lchild;

  printf("%s is defined: ", (char *) $3 -> lchild);
  printtype(type);
  printf("\n");
  if( yyin == stdin) {
    printf("\nplease input a lambda term with \";\":\n");
  } 
  new_env();
  if (current == MAX_ENV) {
    printf("buffer exceeds!");
    exit(1);
  }
  }

|  expr ';' {
  printtree($1);
  Type_ptr type = typing(NULL, $1, current);
  print_expression($1, stdout);
  printf(" |== ");
  printtype ( type );
  
  printf("\n");
  new_env();
  free_ast($1);
  printf("please input a lambda term with \";\":\n");
  }
;

expr : INT {$$ = $1;}

| ID {
  $1->value = find_deepth((char *)$1->lchild);
  $$ = $1;
 }

| IF expr THEN expr ELSE expr FI { $$ = make_cond($2,$4,$6); }

| '(' expr ')' { $$ = $2; }

| '@' ID  { name_env[current++] = (char *)$2->lchild; } '.'  expr %prec THEN {
  $$ = make_abs((char *)$2->lchild, $5);
  current--;
  }

| expr expr %prec CONCAT   {$$ = make_app($1, $2);}
;

%%

void yyerror ( char const *s)
{
  printf ("%s!\n", s);
}

extern FILE * yyin;
int main ()
{
  /* yyin = fopen("library.txt", "r"); */
  /* if (yyin == NULL) { */
  /*   printf("predefined library file can be opened!\n"); */
  /* } */
  if ((texfile = fopen("expr.tex", "w")) == NULL) exit(1);

  printf("please input a lambda term with \";\":\n");  
  init_type_env();
  new_env();
  yyparse ();
  fclose(texfile);
  return 0;
}
