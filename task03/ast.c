
/************************************************************/
/*      copyright hanfei.wang@gmail.com                     */
/*             2012.10.23                                   */
/************************************************************/

#include "include/ast.h"
#include <stdlib.h>
#include "include/set.h"

void free_node(AST_PTR tree);

AST_PTR mkOpNode(Kind op, AST_PTR tree1, AST_PTR tree2) {
  AST_PTR tree_tmp;

  tree_tmp = (AST_PTR) malloc(sizeof(AST));
  tree_tmp->op = op;
  tree_tmp->lchild = tree1;
  tree_tmp->rchild = tree2;
  return tree_tmp;
}

AST_PTR mkLeaf(char c, int pos) {
  AST_PTR tree_tmp;

  tree_tmp = (AST_PTR) malloc(sizeof(AST));
  tree_tmp->op = Alpha;
  tree_tmp->val = c;
  tree_tmp->pos = pos;

  tree_tmp->lchild = NULL;
  tree_tmp->rchild = NULL;
  return tree_tmp;
}

AST_PTR mkEpsilon(void) {
  AST_PTR tree_tmp;
  tree_tmp = (AST_PTR) malloc(sizeof(AST));
  tree_tmp->op = Epsilon;
  tree_tmp->lchild = NULL;
  tree_tmp->rchild = NULL;
  return tree_tmp;
}

void print_tree(AST_PTR tree) {
  if (tree == NULL) {
    printf("attempt print empty tree!\n");
    return;
  }

  switch (tree->op) {
    case Star :print_tree(tree->lchild);
      printf("%s", "*");
      return;
    case Seq : printf("%c", '(');
      print_tree(tree->lchild);
      print_tree(tree->rchild);
      printf("%c", ')');
      return;
    case Or : printf("%c", '(');
      print_tree(tree->lchild);
      printf("%c", '|');
      print_tree(tree->rchild);
      printf("%c", ')');
      return;
    case Alpha :printf("%c", tree->val);
      return;
    case Epsilon :printf("!");
      return;
  }
}

static FILE *outfile;
static int label_count = 0;

void graphviz_aux(AST_PTR);

void graphviz(AST_PTR tree) {
  label_count = 0;
  if ((outfile = fopen("ast_tree.gv", "w")) == NULL) {
    printf("coudn't create output file!\n");
    exit(1);
  }

  if (tree == NULL) {
    printf("attempt output empty tree!\n");
    exit(1);
  }
  tree->index = 0;
  fprintf(outfile, "digraph  G {\n");
  graphviz_aux(tree);
  fprintf(outfile, "}\n");
  fclose(outfile);
}

#define OUTFORMAT "node_%d [shape=record,label=\"%s|{"

void print_node(AST_PTR tree) {
  char *tmp, char_to_array[10] = {0};
  switch (tree->op) {
    case Star :tmp = "*";
      break;
    case Seq :tmp = ".";
      break;
    case Or :tmp = "\\|";
      break;
    case Alpha :sprintf(char_to_array, "{%c|pos|%d}", tree->val, tree->pos);
      tmp = char_to_array;
      break;
    case Epsilon :tmp = "&epsilon;";
  }

  fprintf(outfile, OUTFORMAT, tree->index, tmp);
  fprintf(outfile, "{null|%s}|{first|",
          get_nullable(tree) ? "true" : "false");
  pset(get_firstpos(tree), (pset_t) fprintf, outfile);
  fprintf(outfile, "}|{last|");
  pset(get_lastpos(tree), (pset_t) fprintf, outfile);
  fprintf(outfile, "}|{follow|");
  pset(get_followpos(tree), (pset_t) fprintf, outfile);
  fprintf(outfile, "}}\"]\n");
  return;
}

void graphviz_aux(AST_PTR tree) {
  switch (tree->op) {
    case Star :tree->lchild->index = ++label_count;

      print_node(tree);
      fprintf(outfile, "node_%d  -> node_%d;\n",
              tree->index, tree->lchild->index);
      graphviz_aux(tree->lchild);
      return;
    case Seq :tree->lchild->index = ++label_count;
      tree->rchild->index = ++label_count;
      print_node(tree);
      fprintf(outfile, "node_%d  -> node_%d;\n",
              tree->index, tree->lchild->index);
      fprintf(outfile, "node_%d  -> node_%d;\n",
              tree->index, tree->rchild->index);
      graphviz_aux(tree->lchild);
      graphviz_aux(tree->rchild);
      return;
    case Or :tree->lchild->index = ++label_count;
      tree->rchild->index = ++label_count;
      print_node(tree);
      fprintf(outfile, "node_%d  -> node_%d;\n",
              tree->index, tree->lchild->index);
      fprintf(outfile, "node_%d  -> node_%d;\n",
              tree->index, tree->rchild->index);
      graphviz_aux(tree->lchild);
      graphviz_aux(tree->rchild);
      return;
    case Alpha :
    case Epsilon:print_node(tree);
      return;

  }
}

void print_ast_aux(AST_PTR);

void print_ast(AST_PTR tree) {

  if ((outfile = fopen("ast.gv", "w")) == NULL) {
    printf("coudn't create output file!\n");
    exit(1);
  }

  if (tree == NULL) {
    printf("attempt output empty tree!\n");
    exit(1);
  }
  tree->index = 0;
  fprintf(outfile, "digraph  G {\n");
  print_ast_aux(tree);
  fprintf(outfile, "}\n");
  fclose(outfile);
}

void print_ast_aux(AST_PTR tree) {
  switch (tree->op) {
    case Star :fprintf(outfile, "node_%d[label=\"*\"];\n", tree->index);
      tree->lchild->index = ++label_count;
      fprintf(outfile, "node_%d  -> node_%d;\n",
              tree->index, tree->lchild->index);
      print_ast_aux(tree->lchild);
      return;
    case Seq :fprintf(outfile, "node_%d[label=\".\"];\n", tree->index);
      tree->lchild->index = ++label_count;
      tree->rchild->index = ++label_count;
      fprintf(outfile, "node_%d  -> node_%d;\n",
              tree->index, tree->lchild->index);
      fprintf(outfile, "node_%d  -> node_%d;\n",
              tree->index, tree->rchild->index);
      print_ast_aux(tree->lchild);
      print_ast_aux(tree->rchild);
      return;
    case Or :fprintf(outfile, "node_%d[label=\"|\"];\n", tree->index);
      tree->lchild->index = ++label_count;
      tree->rchild->index = ++label_count;
      fprintf(outfile, "node_%d  -> node_%d;\n",
              tree->index, tree->lchild->index);
      fprintf(outfile, "node_%d  -> node_%d;\n",
              tree->index, tree->rchild->index);
      print_ast_aux(tree->lchild);
      print_ast_aux(tree->rchild);
      return;
    case Alpha :fprintf(outfile, "node_%d[label=\"%c\"];\n", tree->index, tree->val);
      return;
    case Epsilon:fprintf(outfile, "node_%d[label=\"&epsilon;\"];\n", tree->index);
      return;
  }
}
