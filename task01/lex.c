/*lex.c	 XL������ */


#include "lex.h"
#include <stdio.h>
#include <ctype.h>

char       *yytext   = "";  /* ��ǰ����,ע��������ֱ��ָ��
			       �л�����input_buffer,��˲�����'\0'��β,
			       ���ʹ��ʱҪС��, ���ֵΪ0, ��ʾ������Ϊ��,
			       ��Ҫ���¶��� */
int        yyleng    = 0;   /* ���εĳ���	 */
int        yylineno  = 0;   /* ������к�	*/

lex()
{
  static char input_buffer[128];
  char        *current;
  
  current = yytext + yyleng;  	/* �����Զ����Ĵ��� */

  while( 1 ) {                  /* ����һ������     */
    while( !*current ) {
      /* �����ǰ�������Ѷ���,���´Ӽ��̶����µ�һ��.
	 ���������ո� 
      */
      
      current = input_buffer;
      /* �����������,���� EOI */
      if( !fgets( input_buffer, 127, stdin ) ) {
	*current = '\0' ;
	return EOI;
      }
      
      ++yylineno;
      
      while( isspace(*current) )
	++current;
    }

    for( ; *current ; ++current ) {
      /* Get the next token */
      
      yytext = current;
      yyleng = 1;
      
      /* ���ز�ͬ�Ĵʻ���� */
      switch ( *current ) {
        case ';': return SEMI  ;
        case '+': return PLUS  ;
	case '-': return MINUS ;
	case '/': return DIVISION;
        case '*': return TIMES ;
        case '(': return LP    ;
        case ')': return RP    ;

        case '\n':
        case '\t':
        case ' ' : break;

        default:
	  if( !isalnum(*current) )
	    fprintf(stderr, "Ignoring illegal input <%c>\n", *current);
	  else {
	    while( isalnum(*current) )
	      ++current;
	    
	    yyleng = current - yytext;
	    return NUM_OR_ID;
	  }

	  break;
      }
    }
  }
}

static int Lookahead = -1;      /* ��ǰ�鿴�Ĵʻ�,���ֵΪ-1
				   ��ʾ��һ�ε���match����ʱ
				   ����Ҫ��ȡһ���ʻ� */

int match( int token )
{
  /* �ж�token�Ƿ�͵�ǰ��ǰ�鿴�Ĵʻ���ͬ. */
  
  if( Lookahead == -1 )
    Lookahead = lex();
  
  return token == Lookahead;
}

void advance()
{
  /* ��ǰ��һ���ʻ� */
  Lookahead = lex();
}
