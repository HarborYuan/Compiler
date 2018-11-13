
/************************************************************/
/*      copyright hanfei.wang@gmail.com                     */
/*             2018.09.20                                   */
/************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <malloc.h>
#include "set.h"
typedef struct att {
  int nullable;
  SET * firstpos;
  SET * lastpos;
  SET * followpos;
} ATT;

#define MAX_STATES 128
/* maximum states */

typedef enum { Star = 4, Seq = 3, Or = 2, 
	       Alpha = 5, Epsilon = 6} Kind;
/* in order of increasing preference */

typedef struct ast {
  Kind op;
  struct ast *lchild, *rchild;
  char val;
  int pos; /* the position of character */
  ATT attribute; /* the attribute for DFA generation */
  int index;
}  AST;

typedef AST * AST_PTR;

AST_PTR mkOpNode(Kind op, AST_PTR tree1, AST_PTR tree2);

AST_PTR mkLeaf(char c, int pos);

AST_PTR mkEpsilon(void);

void free_node(AST_PTR tree);
  /* free memory of the tree */

void print_tree(AST_PTR tree);
void print_ast (AST_PTR tree);
void graphviz( AST_PTR tree );

AST_PTR start(void);

/* defined in dfa.c */
int  get_nullable ( AST_PTR tree);
SET *get_firstpos (AST_PTR tree);
SET *get_lastpos (AST_PTR tree );
SET *get_followpos (AST_PTR tree );
void calcul_attribute(AST_PTR tree);

typedef  struct follow_index {
  char alphabet;
  SET * follow;
} FOLLOW_INDEX; 

/* a mapping from position to its corresponding alphabet and
   followposition */

extern FOLLOW_INDEX cindex[];
 
