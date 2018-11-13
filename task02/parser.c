
/************************************************************/
/*      copyright hanfei.wang@gmail.com                     */
/*             2018.09.20                                   */
/************************************************************/

/*
	File: parser.c
	Author: Yuan Haobo
	Contact: yuanhaobo@whu.edu.cn
	License: Copyright (c) 2018 Yuan Haobo
	Last Modified: 2018-11-13
*/

#include <ctype.h>
#include <stdlib.h>
#include "ast.h"

/* reg -> term  reg'
   reg' -> '|' term reg' | epsilon
   term -> kleene term'
   term' -> kleene term' | epsilon
   kleene -> fac kleene'
   kleene' -> * kleene' | epsilon
   fac -> alpha | '(' reg ')'
*/
static char input_buffer[MAX_STATES] = "\0";

FOLLOW_INDEX cindex[MAX_STATES] = {0};
/*  pos index to the corresponding character and
    follow set, for dfa transformation only */

static char *current = input_buffer;
/* our match - advance in XL parser is corresponding
   here (* current = ? ) - next_token() */

static int pos = 0; /* for dfa only */

void next_token(void)
{
  if (!*current)
  {
    current = input_buffer;
    if (!fgets(input_buffer, MAX_STATES - 1, stdin))
    {
      *current = '\0';
      return;
    }
  }
  else
    current++;

  while (isspace(*current))
    current++;
}

AST_PTR reg();
AST_PTR reg1(AST_PTR term_left);
AST_PTR term();
AST_PTR term1(AST_PTR kleene_left);
AST_PTR kleene();
AST_PTR kleene1(AST_PTR fac_left);
AST_PTR fac();
///**********************【思考题】去掉多余括号的函数********************///
void simplify_print(AST_PTR tree);

AST_PTR start()
{
  AST_PTR leaf, root = reg();
  pos++;
  leaf = mkLeaf('$', pos);
  root = mkOpNode(Seq, root, leaf);
  simplify_print(root);

  if (*current != '\0')
    printf("the parser finished at %c, before the end of RE\n", *current);
  return root;
}

/* reg -> term  reg' */
AST_PTR reg()
{
  AST_PTR left;
  left = term();
  return reg1(left);
}

/*  reg' -> '|' term reg' | epsilon */
AST_PTR reg1(AST_PTR term_left)
{
  if (*current == '|')
  {
    next_token();
    AST_PTR right = term();
    AST_PTR mid = mkOpNode(Or, term_left, right);
    return reg1(mid);
  }
  return term_left;
}

/*  term -> kleene term' */
AST_PTR term()
{
  AST_PTR left;
  left = kleene();
  return term1(left);
}

/*  term' -> kleene term' | epsilon */
AST_PTR term1(AST_PTR kleene_left)
{
  if (*current == '(' || isalpha(*current) || *current == '!')
  {
    AST_PTR left = kleene();
    AST_PTR mid = mkOpNode(Seq, kleene_left, left);
    AST_PTR right = term1(mid);
    return right;
  }
  return kleene_left;
}

/*  kleene -> fac kleene' */
AST_PTR kleene()
{
  AST_PTR left;
  left = fac();
  return kleene1(left);
}

/*   kleene' -> * kleene' | epsilon */
AST_PTR kleene1(AST_PTR fac_left)
{
  if (*current == '*')
  {
    next_token();
    AST_PTR left = mkOpNode(Star, fac_left, NULL);
    AST_PTR mid = kleene1(left);
    return mid;
  }
  return fac_left;
}

/*   fac -> alpha | '(' reg ')' */
AST_PTR fac()
{
  if (*current == '(')
  {
    next_token();
    AST_PTR thisreg = reg();
    next_token();
    return thisreg;
  }
  else if (*current == '!')
  {
    next_token();
    return mkEpsilon();
  }
  else if (isalpha(*current))
  {
    AST_PTR tmp = mkLeaf(*current, pos++);
    next_token();
    return tmp;
  }
}

void simplify_print(AST_PTR tree)
{
}
