
2016����������ԭ��γ���Ƶ�һ�α����ҵ
			
XL���Ա�����

XL���Եĵ���:

EOI : �������
SEMI: ƥ�䵥���ַ�';'
PLUS: ƥ�䵥���ַ�'+'
TIMES: ƥ�䵥���ַ�'*'
LP: ƥ�䵥���ַ�'('
RP: ƥ�䵥���ַ�')'
NUM_OR_ID: ƥ��ʮ�����������ʶ��

XL���Եĵݹ���ʽ����:

statments ->  expression SEMI EOI
	| expression SEMI statments

expression -> term expression'

expression' -> PLUS term expression'
	|    epsilon   /* �մ� */

term ->	factor term'

term' -> TIMES factor term'
	|    epsilon	/* �մ� */

factor-> NUM_OR_ID
	| LP expression RP

����ģ��

lex.h		�ʻ�궨��ͷ�ļ�
lex.c		�ʷ�����ģ��
plain.c		�﷨����ģ��(�����������)
improved.c	plain.c�ĸĽ�, ��ͨ��legal_lookahead������ expresssion 
		��expression'; term��term'��Ϊһ����������,��plain.c
		�ķ�������
name.c		��ʱ�������亯��
affix.c 	�﷨����ģ��(�����������, ת��Ϊǰ׺���ʽ)
retval.c	�﷨����ģ��(�����������, �����м�����)
retsuff.c	�﷨����ģ��(ͬʱ�����м����Ժ�ǰ׺���ʽ)
retinf.c        ǰ׺���ʽ�﷨����ģ�飬δ��ɣ�
retinf.exe	����ִ���ļ���
main.c		������
plain.prj	�����ļ�,��lex.h, lex.c, plain.c��main.c���
improved.prj	�����ļ�,��lex.h, lex.c, improved.c��main.c���
affix.prj	�����ļ�,��lex.h, lex.c, name.c, affix.c��main.c���
retval.prj	�����ļ�,��lex.h, lex.c, name.c, retval.c��main.c���

unixmake.mak	linux Makefile file: ����������
                $ make -f unixmake.mak 
		�������е�ִ���ļ���

tcmake.mak	Turbo C Makefile(�����������²���!):
		1/ C> path=c:\tc;%path%
		   ����c:\tcΪturbo C��ִ���ļ�·������
		2/ cd ���ļ������ڵ�·����
		3/ ���� tcmake.mak �е�TCHOME·��Ϊ��ϵͳ�е�tc·����
		4/ C> make -ftcmake.mak 
		   �������е�ִ���ļ���


��ҵ:

1) ѧϰ�������Makefile(����make.pdf)��������ִ��gcc(����gcc.pdf).
   �����Ķ�������������г���, ����make����plain, improved, affix 
   ��retvalִ���ļ�(TC��GCC����)��

2) ���retinf.c������ǰ׺���ʽ���ķ����£�

   statements -> expression SEMI  |  expression SEMI statements
   
   expression -> PLUS expression expression
               |  MINUS expression expression
               |  TIMES expression expression
               |  DIVISION expression expression
	       |  NUM_OR_ID

   ����ݹ���ú��� expression()�ķ���ֵ����Ϊ��
  
   struct YYLVAL {
     char * val;  /* ��¼���ʽ�м���ʱ���� */
     char * expr; /* ��¼���ʽ��׺ʽ */
   };

   typedef struct YYLVAL Yylval;
 
  �����retinf.c�еĺ���
  void    *statements     ( void );
  Yylval    *expression ( void );

  ʹ�������������ɷ���ǰ׺���ʽ������м�����ͬʱ��Ҳ�ܹ���ǰ׺
  ���ʽ����Ϊ��׺���ʽ, ��Ҫ��������׺���ʽ�о������������š�
  �磺 ����"+ a * b c;"ʱ��Ӧ�����׺ʽΪ" a + b * c", ������"a + (b * c)"��
  "(a) + (b * c)"��(�μ���������retinf.exe��ִ�����)��
  
  ע�⣺���� expression()�ȷ��ص���ָ�룬��ָ������
  һ�������Ǿֲ�����������������ʱ�����л����ͷţ�
  ָ��ú����ֲ������Ĵ洢�ռ佫�����ã�������������
  �Ľ��(Dangling Reference), ������ȫ�ֱ�����malloc()
  ��̬����Ĵ洢�ռ䡣
 
  �����: "* + a b * c d;"��������.

3) XL���Է������Ľ�ϴ�������ȼ�, ��retsuff.exe��������ʽ: "1 + 2 + 3;", 
   �Ƚ��� 1 + 2 ������, �����Ƚ��� 2 + 3 ������. 
   ����"1+2*3;"������"2*3"����������"1+2".

4) ˼���� ����ǰ׺���ʽ��
   + a + b + + c d + e f;
   ����retinf.exe���������
    t0 = a
    t1 = b
    t2 = c
    t3 = d
    t2 += t3
    t3 = e
    t4 = f
    t3 += t4
    t2 += t3
    t1 += t2
    t0 += t1
   ����Ҫt0, t2, ..., t4����Ĵ�����ɱ��ʽ�ļ��㡣��������޸ļ���������£�
    t0 = c
    t1 = d
    t0 += t1
    t1 = e
    t2 = f
    t1 += t2
    t0 += t1
    t1 = b
    t0 += t1
    t1 = a
    t0 += t1
    ��ֻ�������Ĵ�������˼�����ʵ�������㷨��
    (��ο�: ���������㷨(https://en.wikipedia.org/wiki/Strahler_number))

    ������޸���ĳ���ʹ����������м����ʹ�õļĴ������١�

    ������޸ļ���Ľ�ϴ���Ϊ��+ + + + + a b c d e f, 
    ����������Ĵ���......  
    (hard, ��ο�: �Ĵ����ķ���ʵ����ͬ����ͼ�۵���ɫ����(NP-complete)
    (https://en.wikipedia.org/wiki/Register_allocation)
    (���飺P.556))

5) �뽫��ɺ��retinf.c����(��Ҫ�������ļ���Ҳ��Ҫ���ѹ��)
   mailto: hanfei.wang@gmail.com
   �ʼ����������: ѧ��(1)
   ע�⣺ѧ�ź�(1)һ����ASCII�롣����1��ʾ��һ�α����ҵ��

hfwang

2018.09.04




	
