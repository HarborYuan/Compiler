/************************************************************/
/*      copyright hanfei.wang@gmail.com                     */
/*             2018.10.23                                   */
/************************************************************/

/***********************************************/
/*      File: dfa.c                            */
/*      Author: Yuan Haobo                     */
/*      Contact: yuanhaobo@whu.edu.cn          */
/*      License: Copyright (c) 2018 Yuan Haobo */
/***********************************************/

#include "include/ast.h"

#define DEBUG 1

FOLLOW_INDEX cindex[MAX_STATES] = {0};
/*  pos index to the corresponding character and 
    follow set */

static int pos = 0;

void put_index(char c) {
  if (pos > MAX_STATES - 1) {
    printf("maximum buffer exceeded!\n");
    exit(1);
  }
  cindex[pos].alphabet = c;
  return;
}

char get_index(int i) {
  if (pos > MAX_STATES - 1) {
    printf("maximum buffer exceeded!\n");
    exit(1);
  }
  return cindex[i].alphabet;
}

int get_nullable(AST_PTR tree) {
  return (tree->attribute).nullable;
}

SET *get_firstpos(AST_PTR tree) {
  return (tree->attribute).firstpos;
}

SET *get_lastpos(AST_PTR tree) {
  return (tree->attribute).lastpos;
}

SET *get_followpos(AST_PTR tree) {
  return (tree->attribute).followpos;
}

void set_pos(AST_PTR tree) {
  if (tree == NULL) {
    printf("attempt manipulate an empty tree!\n");
    return;
  }

  switch (tree->op) {
    case Star :set_pos(tree->lchild);
      return;
    case Seq :
    case Or :set_pos(tree->lchild);
      set_pos(tree->rchild);
      return;
    case Alpha :pos++;
      tree->pos = pos;
      put_index(tree->val);   /* add end sign to AST */
      return;
    case Epsilon :return;
  }

}

void nullable(AST_PTR tree) {
  if (tree == NULL) {
    printf("attempt manipulate an empty tree!\n");
    return;
  }
  switch (tree->op) {
    case Star :nullable(tree->lchild);
      tree->attribute.nullable = 1;
      return;
    case Seq :nullable(tree->lchild);
      nullable(tree->rchild);
      tree->attribute.nullable = tree->lchild->attribute.nullable && tree->rchild->attribute.nullable;
      return;
    case Or :nullable(tree->lchild);
      nullable(tree->rchild);
      tree->attribute.nullable = tree->lchild->attribute.nullable || tree->rchild->attribute.nullable;
      return;
    case Alpha :tree->attribute.nullable = 0;
      return;
    case Epsilon :tree->attribute.nullable = 1;
      return;
  }
}

void firstpos(AST_PTR tree) {
  if (tree == NULL) {
    printf("attempt manipulate an empty tree!\n");
    return;
  }
  SET *tmp = newset();
  switch (tree->op) {
    case Star :firstpos(tree->lchild);
      UNION(tmp, tree->lchild->attribute.firstpos);
      tree->attribute.firstpos = tmp;
      return;
    case Seq :firstpos(tree->lchild);
      firstpos(tree->rchild);
      if (get_nullable(tree->lchild)) {
        UNION(tmp, tree->lchild->attribute.firstpos);
        UNION(tmp, tree->rchild->attribute.firstpos);
      } else {
        UNION(tmp, tree->lchild->attribute.firstpos);
      }
      tree->attribute.firstpos = tmp;
      return;
    case Or :firstpos(tree->lchild);
      firstpos(tree->rchild);
      UNION(tmp, tree->lchild->attribute.firstpos);
      UNION(tmp, tree->rchild->attribute.firstpos);
      tree->attribute.firstpos = tmp;
      return;
    case Alpha :ADD(tmp, tree->pos);
      tree->attribute.firstpos = tmp;
      return;
    case Epsilon :tree->attribute.firstpos = tmp;
      return;
  }
}

void lastpos(AST_PTR tree) {
  if (tree == NULL) {
    printf("attempt manipulate an empty tree!\n");
    return;
  }
  SET *tmp = newset();
  switch (tree->op) {
    case Star :lastpos(tree->lchild);
      UNION(tmp, tree->lchild->attribute.lastpos);
      tree->attribute.lastpos = tmp;
      return;
    case Seq :lastpos(tree->lchild);
      lastpos(tree->rchild);
      if (get_nullable(tree->rchild)) {
        UNION(tmp, tree->lchild->attribute.lastpos);
        UNION(tmp, tree->rchild->attribute.lastpos);
      } else {
        UNION(tmp, tree->rchild->attribute.lastpos);
      }
      tree->attribute.lastpos = tmp;
      return;
    case Or :lastpos(tree->lchild);
      lastpos(tree->rchild);
      UNION(tmp, tree->lchild->attribute.lastpos);
      UNION(tmp, tree->rchild->attribute.lastpos);
      tree->attribute.lastpos = tmp;
      return;
    case Alpha :ADD(tmp, tree->pos);
      tree->attribute.lastpos = tmp;
      return;
    case Epsilon :tree->attribute.lastpos = tmp;
      return;
  }
}

void followpos_aux(AST_PTR tree) {
  if (tree == NULL) {
    printf("attempt manipulate an empty tree!\n");
    return;
  }
  SET *tmp = newset();
  switch (tree->op) {
    case Star :UNION (tmp, tree->attribute.followpos);
      UNION (tmp, tree->lchild->attribute.firstpos);
      tree->lchild->attribute.followpos = tmp;
      followpos_aux(tree->lchild);
      return;
    case Seq:UNION(tmp, tree->attribute.followpos);
      tree->rchild->attribute.followpos = tmp;
      if (tree->rchild->attribute.nullable) {
        tmp = newset();
        UNION(tmp, tree->attribute.followpos);
        UNION(tmp, tree->rchild->attribute.firstpos);
        tree->lchild->attribute.followpos = tmp;
      } else {
        tmp = newset();
        UNION(tmp, tree->rchild->attribute.firstpos);
        tree->lchild->attribute.followpos = tmp;
      }
      followpos_aux(tree->lchild);
      followpos_aux(tree->rchild);
      return;
    case Or: UNION(tmp, tree->attribute.followpos);
      tree->lchild->attribute.followpos = tmp;
      tmp = newset();
      UNION(tmp, tree->attribute.followpos);
      tree->rchild->attribute.followpos = tmp;
      followpos_aux(tree->lchild);
      followpos_aux(tree->rchild);
      return;
    case Alpha:UNION (tmp, get_followpos(tree));
      cindex[tree->pos].follow = tmp;
      return;
    case Epsilon:tree->attribute.followpos = tmp;
      return;
  }
}

void followpos(AST_PTR tree) {
  SET *tmp = newset();
  /* add end mark $ to followpos of AST root */
  ADD (tmp, (tree->rchild->pos));
  (tree->attribute).followpos = tmp;
  (tree->lchild->attribute).followpos = tmp;
  (tree->rchild->attribute).followpos = newset();
  cindex[tree->rchild->pos].follow = newset();
  followpos_aux(tree->lchild);
  return;
}

void test_limit_of_max_state(int x) {
  if (x > MAX_STATES) {
    printf("maximum state exceeded! exit!\n");
    exit(1);
  }
}

static struct alpha_pos {
  char alphabet;
  SET *pos;
} alphabet_index[MAX_STATES] = {0};

/* a mapping from alphabet to its position set */
/* and the array index is the index of the alphabet */

static int alphabet_number = 0;

void get_alphabet_to_pos(AST_PTR tree) {
  int length = tree->rchild->pos, i, j;
  alphabet_number = -1;
  for (i = 1; i <= length; i++) {
    j = 0;
    while (j <= alphabet_number) {
      if (cindex[i].alphabet == alphabet_index[j].alphabet) {
        ADD (alphabet_index[j].pos, i);
        break;
      }
      j++;
    }
    if (j > alphabet_number) {
      SET *tmp = newset();
      alphabet_index[j].alphabet = cindex[i].alphabet;
      ADD(tmp, i);
      alphabet_index[j].pos = tmp;
      alphabet_number = j;
    }
  }
  if (DEBUG) {
    printf("the number of alphabet is %d!\n", alphabet_number);
    for (i = 0; i < alphabet_number; i++) {
      printf("%c has pos as : ", alphabet_index[i].alphabet);
      pset(alphabet_index[i].pos, (pset_t) fprintf, stdout);
      printf("\n");
    }
  }
  /* alphabet_index begins at  0 !!! */
}
void dfa(AST_PTR tree);
void minimize();

void calcul_attribute(AST_PTR tree) {
  set_pos(tree);
  nullable(tree);
  firstpos(tree);
  lastpos(tree);
  followpos(tree);
  get_alphabet_to_pos(tree);
  dfa(tree);
  minimize();
  return;
}

static struct States {
  int is_accept;
  SET *dfa_set;
  int *row;
} states[MAX_STATES] = {0};
static int next_state = 0;

void init_state(struct States *state, SET *dfa_set,
                int row_value, int length_of_row) {
  int i;
  state->is_accept = 0;
  state->dfa_set = dfa_set;
  state->row = (int *) malloc(length_of_row * sizeof(int));
  if (state->row == NULL) {
    printf("memory exhausted, exit!\n");
    exit(1);
  }
  for (i = 0; i < length_of_row; i++)
    (state->row)[i] = row_value;
}

void print_dfa(char *filename, struct States *states, int next_state);

void dfa(AST_PTR tree) {
  int i = 0;
  states[0].dfa_set = get_firstpos(tree);

  while (i <= next_state) {
    int *next_row, j;

    init_state(&states[i], states[i].dfa_set, 0, alphabet_number);

    if (DEBUG) printf("state %d: \n", i);
    next_row = states[i].row;

    if (MEMBER(states[i].dfa_set, tree->rchild->pos))
      states[i].is_accept = 1;

    for (j = 0; j < alphabet_number; j++) {
      /* for every character, calcul goto state */
      int k;
      char c = alphabet_index[j].alphabet;
      SET *posset = alphabet_index[j].pos;
      SET *gotoset = newset();
      next_member(NULL);
      while ((k = next_member(states[i].dfa_set)) >= 0) {
        if (cindex[k].alphabet == c)
          UNION (gotoset, cindex[k].follow);
      }
      if (DEBUG) pset(gotoset, (pset_t) fprintf, stdout);

      if (!IS_EMPTY(gotoset)) {
        int k;
        for (k = 0; k <= next_state; k++) {
          if (IS_EQUIVALENT(states[k].dfa_set, gotoset))
            break;
        }
        if (k > next_state) {
          if (DEBUG) printf(" ### k=%d\n", k);
          next_state++;

          test_limit_of_max_state(next_state);

          states[next_state].dfa_set = gotoset;
        }
        if (DEBUG) printf("%c, to state %d\n", c, k);
        *next_row++ = k;
      } else
        *next_row++ = -1;
    }
    i++;
  }
  print_dfa("dfa.gv", states, next_state);
}

void print_dfa(char *filename, struct States *states, int next_state)
/* generate graphviz dot format of DFA */
{
  FILE *dfafile;
  int i, j;
  if ((dfafile = fopen(filename, "w")) == NULL) {
    printf("coudn't create output file!\n");
    exit(1);
  }
  fprintf(dfafile, "digraph  G {\nrankdir=LR\n\
start[shape =none]\nstart ->node_0\n");
  for (i = 0; i <= next_state; i++) {
    int *p = states[i].row;
    fprintf(dfafile, "node_%d [label=\"state%d:\\n", i, i);
    pset(states[i].dfa_set, (pset_t) fprintf, dfafile);
    if (states[i].is_accept)
      fprintf(dfafile, "\", shape = doublecircle]\n");
    else
      fprintf(dfafile, "\"]\n");

    for (j = 0; j < alphabet_number; j++) {
      if (p[j] != -1) {
        fprintf(dfafile, "node_%d -> node_%d [label =\"%c\"]\n",
                i, p[j], alphabet_index[j].alphabet);
      }
    }
  }
  fprintf(dfafile, "}\n");
  fclose(dfafile);
}

int index_of_partition(int element, SET **set_list, int set_length) {
  int i;
  for (i = 0; i <= set_length; i++)
    if (MEMBER(set_list[i], element)) break;
  return i;
}

void minimize() {
  /* 请完成!!!  选做 */
}
