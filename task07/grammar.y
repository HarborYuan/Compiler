%{
/* the grammar of lambda expression */
#include "tree.h"
#include "closure.h"
#include "type.h"
#include "code.h"
void yyerror (char const *);
/************************************/

char *name_env[MAX_ENV] = {"+", "-", "*", "/", "=", "<"};

AST *ast_env[MAX_ENV];
int current = INIT_POS;
#define YYSTYPE AST *
FILE *texfile;
 FILE *eval_tree;
int is_decl = 0;
CLOSURE_LIST *init_clos;
CBN_ENV *init_env_cbn; 
extern FILE * yyin;

static void (* set_method)(char *, AST *);
static void (* call_method)(AST *);
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
  set_method((char *) $3 -> lchild, $5);
  }
|  expr ';' {
  call_method($1);
  }
;


expr : INT {$$ = $1;}

| ID {
   $1->value = find_deepth((char *)$1->lchild);
   $$ = $1;
 }

| IF expr THEN expr ELSE expr FI { $$ = make_cond($2,$4,$6); }

| '(' expr ')' { $$ = $2; }

| '@' ID  { name_env[current++] = (char *)$2->lchild;} '.'  expr %prec THEN {
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

void set_call_by_name(char *name, AST *ast)
{
  CLOSURE *clos;
  Type_ptr type = typing(NULL, ast, current);

  name_env[current] = (char *) name;
  global_type_env [current] = storetype(type);
  ast_env[current] = ast;
  init_clos = make_list(clos, init_clos);
  printf("%s is defined: \n", name);
  printtype(type);
  new_env();
  clos = (eval_cbn(make_cbn_env(make_clos(ast, NULL,  current),NULL))) -> cbn_env;
  printf("\n");
  if( yyin == stdin) {
    print_closure(clos);
    printf("\n");
    printf("please input a lambda term with \";\":\n");
  }
  global_eval_env[current] = clos;
  current++;
  if (current == MAX_ENV) {
    printf("%d, buffer exceeds!", current);
    exit(1);
  }
}

void set_call_by_value(char *name, AST *ast)
{
  CLOSURE *clos = make_clos(clone_tree(ast), NULL, current);
  Type_ptr type = typing(NULL, ast, current);
  OBJECT *exe_result;
  ast_env[current] = ast;
  global_type_env [current] = storetype(type);
  name_env[current] = name;

  printf("%s is defined: \n", name);
  printtype(type);
  printf("\n");
  if( yyin == stdin) {
    print_closure(clos);
    printf("\nplease input a lambda term with \";\":\n");
  } 
 
  exe_result = execution(compile(ast));
  if (exe_result -> kind == CLOS)
    exe_result -> env -> value = current;
  global_exec_env[current] = exe_result;

  new_env();
  /* if (strcmp(name, "fact") == 0) { */
  /*   printf("looop\n"); */
  /*   CLOSURE_LIST *loop =  make_list(eval_cbv(clos), NULL); */
  /*   loop -> next = loop; */
  /*   init_eval_env[current] = loop;  */
  /* } */
  /* else */
  global_eval_env[current] = eval_cbv(clos);
  current++; 
  if (current == MAX_ENV) {
    printf("%d, buffer exceeds!", current);
    exit(1);
  }
}

void call_by_name(AST *ast)
{
  CLOSURE *clos = make_clos(ast, NULL, current);
  Type_ptr type = typing(NULL, ast, current);
  CBN_ENV *cbn_clos;
  print_expression(ast, stdout);
  printf(" |== ");
  printtype (type);
  printf("\n"); 
  new_env();
  printtree(ast);
  print_closure(clos);
  printf("\n");
  cbn_clos = (eval_cbn(make_cbn_env(clos, NULL)));
  print_closure(cbn_clos -> cbn_env);
  free_cbn_env(cbn_clos);
  printf("\nstep = %d\nplease input a lambda term with \";\":\n", step);
  step = 0;
}

void call_by_value(AST * ast)
{
  CLOSURE *clos = make_clos(ast, NULL, current);
  Type_ptr type = typing(NULL, ast, current);
  print_expression(ast, stdout);
  printf(" |== ");
  printtype (type);
  printf("\n"); 
  new_env();
  printtree(ast);
  execution(compile(ast));
  print_closure(clos);
  clos = eval_cbv(clos);
  printf(" => ");
  print_closure(clos);
  free_clos(clos);
  printf("\nstep = %d\nplease input a lambda term with \";\":\n",step);
  step = 0;
}

int main(int argc, char ** argv)
{
  set_method = set_call_by_name;
  call_method = call_by_name;
  while (argc > 1) {
    argc --; ++argv;
    if (strcmp(argv[0], "-v") == 0) {
      set_method = set_call_by_value;
      call_method = call_by_value;
      break;
    }      
  }

  if (call_method == call_by_value)
    printf("Call by VALUE mode!!!\n");
  else
    printf("Call by NAME mode!!!\n");
    
  printf("please input a lambda term with \";\":\n");
  
  /* please uncomment the following lines, if you want
     precharge the library */
  /* yyin = fopen("library.txt", "r"); */
  /* if (yyin == NULL) { */
  /*   printf("predefined library file can be opened!\n"); */
  /* } */
  
  init_eval();
  init_type_env();
  init_exec_env();

  texfile = fopen("expr.tex", "w");
  fprintf(texfile, "\\exptree{\\Tree\n");
  yyparse ();
  fprintf(texfile, "}\n");
  fclose(texfile);
  return 0;
}
 
