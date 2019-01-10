
typedef enum {NUM=1, GET=2, BRANCH=3, PUT=4, APPLY=5,
              RETURN=6, ADD=7, SUB=8, MULT=9, DIV=10, EQ=11, LT=12, TAIL=13} INSTRUCT;

typedef struct Instruction {
  INSTRUCT instruct;
  int value;
  struct Instruction *abs;
  struct Instruction *next;
} INSTRUCTION;

typedef enum {CONSTANT=1, CLOS=2, ENV=3} Object_kind;

typedef struct Object {
  Object_kind kind;
  int value;
  INSTRUCTION * instr_list;
  struct Object * env;
  struct Object * next; 
} OBJECT;


typedef struct State {
  OBJECT *reg;
  INSTRUCTION *pc;
  OBJECT *stack;
} STATE;

OBJECT *make_object(Object_kind kind, int value, 
		    OBJECT *env, OBJECT *next, INSTRUCTION * instr);

OBJECT *get_n_th_env(OBJECT *env, int n);

INSTRUCTION *make_instruction(INSTRUCT instruct, int value, 
			      INSTRUCTION *abs, INSTRUCTION * next);
extern OBJECT *global_exec_env[MAX_ENV];

void init_exec_env();
INSTRUCTION *compile(AST * t);
INSTRUCTION *tail(AST *t);
OBJECT *execution(INSTRUCTION *code);
