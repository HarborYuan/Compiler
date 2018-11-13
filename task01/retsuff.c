/* retval.c  	XL������*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lex.h"

char err_id[] = "error";
char * midexp;

struct YYLVAL {
  char * val;  /* ��¼���ʽ�м���ʱ���� */
  char * expr; /* ��¼���ʽǰ׺ʽ */
};

typedef struct YYLVAL Yylval;

Yylval    *factor     ( void );
Yylval    *term       ( void );
Yylval    *expression ( void );

char *newname( void ); /* ��name.c�ж��� */

extern void freename( char *name );

void statements ( void )
{
  /*  statements -> expression SEMI  |  expression SEMI statements  */
  
  Yylval *temp;
  printf("Please input an infix expression and ending with \";\"\n");
  while( !match(EOI) )    {
    temp = expression();
    printf("the affix expression is %s\n", temp -> expr);
    freename(temp -> val);
    free(temp -> expr);
    free(temp);    
    if( match( SEMI ) ){
      advance();
      printf("Please input an infix expression and ending with \";\"\n");
    }
    else
      fprintf( stderr, "%d: Inserting missing semicolon\n", yylineno );
  }
}

Yylval    *expression()
{
  /* expression -> term expression'
     expression' -> PLUS term expression' 
                 |  MINUS term expression'
                 |  epsilon           */
  
  Yylval  *temp, *temp2;
  
  char *tmpmid;  /* ��¼ǰ׺���ʽ */
  char *tmpmid1;
  
  temp = term();
  tmpmid = temp->expr;
  
  while( match( PLUS )|| match(MINUS) ) {
    char op = yytext[0];
    advance();
    temp2 = term();
    printf("    %s %c= %s\n", temp ->val, op, temp2 ->val );
	/* generate code by side effects */

    freename( temp2 ->val );
      
    /* ���Ѿ��Ǳ�ľ��� term PLUS term2 �������Ӧ��ǰ׺ʽ
       ��ǰ׺ʽΪ��  "+" + term->expr + " " + term2->expr  */

    /* ���Ѿ��Ǳ�ľ��� term MINUS term2 �������Ӧ��ǰ׺ʽ
       ��ǰ׺ʽΪ��  "-" + term->expr + " " + term2->expr  */
  
  
    tmpmid1 = (char *) malloc(strlen(temp2 -> expr) + strlen(tmpmid) + 4);
    sprintf (tmpmid1, "%c %s %s", op, tmpmid, temp2 -> expr);

    free(tmpmid);  /* һ��Ҫ�ͷ� */

    tmpmid = tmpmid1;
    free(temp2->expr);
    free(temp2); 

  }
  temp->expr = tmpmid; /* ���õ�һ��term()�Ľṹ��Ϊ�µķ��� */
  
  return temp;
}

Yylval    *term( void)
{
  Yylval  *temp, *temp2 ;
  char  *tmpmid, *tmpmid1;
  temp = factor();
  tmpmid = temp->expr;
  while( match( TIMES ) || match( DIVISION ) )    {
    char op = yytext[0];
    advance();
    temp2 = factor();
    printf("    %s %c= %s\n", temp->val, op, temp2->val );
	/* generate code by side effects */

    freename( temp2->val );

    tmpmid1 = (char *) malloc(strlen(temp2->expr) + strlen(tmpmid) + 4);
    sprintf ( tmpmid1, "%c %s %s", op, tmpmid, temp2 -> expr);   
    free(tmpmid);
    tmpmid = tmpmid1;
    free(temp2->expr);
    free(temp2);
  }
  
  temp->expr = tmpmid;
  return temp;
}

Yylval    *factor( void )
{
  Yylval *temp;
  char * tmpvar, *tmpexpr;
  if( match(NUM_OR_ID) )    {
    tmpvar = newname();
    tmpexpr = (char *) malloc(yyleng + 1);

    strncpy(tmpexpr, yytext, yyleng);
    tmpexpr[yyleng] = 0; 

    printf("    %s = %s\n", tmpvar, tmpexpr );
	/* generate code by side effects */
    
    temp = (Yylval *) malloc(sizeof(Yylval)); 
      /* һ��Ҫ��̬����
	 ָ��ֲ�������ָ����Բ����Ƿ���ֵ */
    temp->val = tmpvar;
    temp->expr = tmpexpr;
    advance();

  } else
    if( match(LP) ) {
      advance();
      temp = expression();
      if( match(RP) ){
	advance();
      } else
	fprintf(stderr, "%d: Mismatched parenthesis\n", yylineno );
    }
    else {
      char *s ;
      advance();
      s = (char *) malloc(10);
      strcpy(s,"error_id");
      fprintf( stderr, "%d: Number or identifier expected\n", yylineno );
      temp = (Yylval *) malloc( sizeof(Yylval));
      temp->val =  newname();
      temp->expr = s;
    }
  
  return temp;
}
