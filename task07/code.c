#include "tree.h"
#include "code.h"

INSTRUCTION *make_instruction(INSTRUCT instruct, int value,
                              INSTRUCTION *abs, INSTRUCTION *next) {
  INSTRUCTION *
      new = (INSTRUCTION *) smalloc(sizeof(INSTRUCTION));
  new -> instruct = instruct;
  new -> value = value;
  new -> abs = abs;
  new -> next = next;
  return new;
}

OBJECT *make_object(Object_kind kind, int value,
                    OBJECT *env, OBJECT *next, INSTRUCTION *instr) {
  OBJECT *
      new = (OBJECT *) smalloc(sizeof(OBJECT));
  new -> kind = kind;
  new -> value = value;
  new -> env = env;
  new -> instr_list = instr;
  new -> next = next;
  return new;
}

void free_instruction(INSTRUCTION *);

void free_object(OBJECT *obj) {
  if (obj == NULL) return;

  free_object(obj->env);
  free_object(obj->next);
  free_instruction(obj->instr_list);
  sfree(obj);
  return;
}

INSTRUCTION *clone_instruction(INSTRUCTION *source) {
  if (source == NULL) return NULL;
  return make_instruction(source->instruct,
                          source->value,
                          clone_instruction(source->abs),
                          clone_instruction(source->next));
}

void free_instruction(INSTRUCTION *instr) {
  if (instr == NULL) return;
  free_instruction(instr->next);
  free_instruction(instr->abs);
  sfree(instr);
  return;
}

void print_instruction(INSTRUCTION *instr) {
  if (instr == NULL) return;
  switch (instr->instruct) {
    case NUM:printf("NUM %d", instr->value);
      break;
    case GET:printf("GET:%d", instr->value);
      break;
    case BRANCH:printf("BRA");
      break;
    case PUT:printf("PUT[");
      print_instruction(instr->abs);
      printf("]");
      break;
    case APPLY:printf("APP");
      break;
    case TAIL:printf("TAIL");
      break;
    case RETURN:printf("RET");
      break;
    case ADD:printf("ADD");
      break;
    case SUB:printf("SUB");
      break;
    case MULT:printf("MUL");
      break;
    case DIV:printf("DIV");
      break;
    case EQ:printf("EQ");
      break;
    case LT:printf("LT");
  }
  if (instr->next != NULL) {
    printf(";");
    print_instruction(instr->next);
  }
  return;
}

void print_instruction_list(INSTRUCTION *instr) {
  printf("[");
  print_instruction(instr);
  printf("]");
  return;
}

OBJECT *clone_object(OBJECT *source) {
  if (source == NULL) return NULL;
  switch (source->kind) {
    case CONSTANT:
      return make_object(CONSTANT, source->value, NULL,
                         clone_object(source->next), NULL);
    case ENV:
      return make_object(ENV, source->value,
                         clone_object(source->env),
                         clone_object(source->next), NULL);
    case CLOS: {
      return make_object(CLOS, 0,
                         clone_object(source->env),
                         clone_object(source->next),
                         clone_instruction(source->instr_list));
    }
  }
}

OBJECT *clone_single_object(OBJECT *source) {
  if (source == NULL) return NULL;
  switch (source->kind) {
    case CONSTANT:
      return make_object(CONSTANT, source->value, NULL,
                         NULL, NULL);
    case ENV:
      return make_object(ENV, source->value,
                         clone_object(source->env),
                         NULL, NULL);
    case CLOS: {
      return make_object(CLOS, 0,
                         clone_object(source->env),
                         NULL,
                         clone_instruction(source->instr_list));
    }
  }
}

void print_object_list(OBJECT *);

void print_object(OBJECT *source) {
  if (source == NULL) return;
  switch (source->kind) {
    case CONSTANT:printf("%d", source->value);
      break;
    case ENV:printf("ENV");
      print_object_list(source->env);
      break;
    case CLOS:printf("CLOS(");
      print_instruction_list(source->instr_list);
      printf(",");
      print_object_list(source->env);
      printf(")");
  }
  if (source->next != NULL) {
    printf(";");
    print_object(source->next);
  }
  return;
}

void print_object_list(OBJECT *obj) {
  printf("[");
  print_object(obj);
  printf("]");
  return;
}

INSTRUCTION *concate(INSTRUCTION *code1, INSTRUCTION *code2) {
  INSTRUCTION *cursor = code1;
  if (code1 == NULL) return code2;
  while (cursor->next != NULL)
    cursor = cursor->next;
  cursor->next = code2;
  return code1;
}

OBJECT *get_n_th_env12(OBJECT *env, int n) {
  int i = 0;
  OBJECT *cursor = env;
  int index = env->value;
  while (i != n - 1 && cursor != NULL) {
    cursor = cursor->next;
    i++;
  }
  if (i == n - 1 && cursor != NULL)
    return cursor;

  printf("wrong access of closure env\n");
  exit(1);
}

OBJECT *get_n_th_env(OBJECT *env, int n) {
  int i = 0;
  OBJECT *cursor = env->env;
  int index = env->value;
  while (i != n - 1 && cursor != NULL) {
    cursor = cursor->next;
    i++;
  }
  if (i == n - 1 && cursor != NULL)
    return clone_single_object(cursor);

  if (index - (n - i) >= 0)
    return clone_single_object(global_exec_env[index - n + i]);

  printf("wrong access of closure env\n");
  exit(1);
}

int primitive_op(int x, int y, INSTRUCT op) {
  switch (op) {
    case ADD :return y + x;
    case SUB :return y - x;
    case MULT :return y * x;
    case DIV :return y / x;
    case EQ :return y == x;
    case LT :return y < x;
  }
}

STATE *step_exe(STATE *state) {
  /* todo */
}

INSTRUCTION *compile(AST *t) {
  INSTRUCTION *result;
  switch (t->kind) {
    case CONST: {
      result = make_instruction(NUM, t->value, NULL, NULL);
      return result;
    }
    case VAR: {
      result = make_instruction(GET, t->value, NULL, NULL);
      return result;
    }
    case ABS: {
      INSTRUCTION *right = compile(t->rchild);
      INSTRUCTION *ret = make_instruction(RETURN, 0, NULL, NULL);
      result = make_instruction(PUT, 0, concate(right, ret), NULL);
      return result;
    }
    case COND: {
      /* todo */
    }
  }
}

INSTRUCTION *basic_op(INSTRUCT op) {
  /* [ABS [OP; RETURN]; RETURN]*/
  INSTRUCTION *i_op = make_instruction(op, 0, NULL,
                                       make_instruction(RETURN, 0, NULL, NULL)),
      *abs = make_instruction(PUT, 0, i_op,
                              make_instruction(RETURN, 0, NULL, NULL));
  return abs;
}

OBJECT *global_exec_env[MAX_ENV];

void init_exec_env() {
  int i;
  INSTRUCT op;
  for (i = 0; i < INIT_POS; i++) {
    global_exec_env[i] = make_object(CLOS, 0,
                                     make_object(ENV, 0, NULL, NULL, NULL),
                                     NULL, basic_op(i + ADD));
  }
}

OBJECT *execution(INSTRUCTION *code) {
  STATE *state = (STATE *) smalloc(sizeof(STATE));
  OBJECT *result;
  state->pc = code;
  state->reg = make_object(ENV, current, NULL, NULL, NULL);
  state->stack = NULL;
  while (state->pc != NULL)
    state = step_exe(state);
  if (state->stack != NULL)
    printf("kind: %d, value %d\n", state->stack->kind,
           state->stack->value);
  printf("result stack is: ");
  print_object_list(state->stack);
  printf("\n");
  sfree(state->stack->next);
  state->stack->next = NULL;
  free_instruction(state->pc);
  free_object(state->reg);
  result = state->stack;
  sfree(state);
  return result;
}
