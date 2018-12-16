
/************************************************************/
/*      copyright hanfei.wang@gmail.com                     */
/*             2012.02.24                                  */
/************************************************************/

#include <ctype.h>
#include <stdlib.h>
#include "include/ast.h"

void next_token();

int main(void) {
  AST_PTR ast;
  printf("Please input a regular expression\n");
  printf("the Epsilon is represented by exlamation mark '!':\n");
  next_token();

  ast = start();

  print_tree(ast);
  print_ast(ast);
  printf("\n");

  calcul_attribute(ast);
  graphviz(ast);

  return 0;
}
