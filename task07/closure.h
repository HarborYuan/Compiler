
typedef struct Closure {
  AST *ast;
  int index;
  struct Closure_list {
    struct Closure *clos;
    struct Closure_list *next;
  } *env;
} CLOSURE;

typedef struct Closure_list CLOSURE_LIST;

typedef struct Cbn_env {
  CLOSURE *cbn_env;  /* ast + closure */ 
  CLOSURE_LIST *stack;
} CBN_ENV;

 
/* */
CLOSURE *make_clos(AST *, CLOSURE_LIST *, int);
void print_closure(CLOSURE *);
CLOSURE *eval_cbv(CLOSURE *); 
void  init_eval();
CLOSURE *make_clos(AST *tree, CLOSURE_LIST *env, int);
CLOSURE_LIST *make_list(CLOSURE *clos, CLOSURE_LIST *env);
AST *clone_tree(AST * source);
CLOSURE_LIST *clone_list(CLOSURE_LIST *source);
CLOSURE *clone_clos(CLOSURE *source);
void free_list(CLOSURE_LIST *list);
void free_clos(CLOSURE *clos);
extern CLOSURE* global_eval_env[];


CBN_ENV *make_cbn_env(CLOSURE *clos, CLOSURE_LIST *stack);
CBN_ENV *eval_cbn(CBN_ENV *clos);
CBN_ENV *init_eval_env_cbv(void);
void free_cbn_env(CBN_ENV *clos);
extern int step; /* define in type.c */
