2016级弘毅班编译原理课程设计第2次编程作业

本次作业是编写RE直接转换为DFA程序(见 Dragon Book PP 173 Ch.3.9)的第一部分.
它将完成对输入正规表达式的识别，并生成抽象语法树结构，以便后续转换程序使用。

语法分析采用递归下降分析法. 

设RE的无二义文法文法定义如下：

reg -> reg '|' term | term
term -> term kleene | kleene
kleene -> kleene * | fac
fac -> alpha | epsilon|  '(' reg ')'
alpha -> 'a' - 'z' | 'A' - 'Z' | '0' - '9'
epsilon -> '!'

注：用ASCII的感叹号'!'表示正规表达式中的空串.

消除左递归后的等价文法如下：

reg -> term  reg'
reg' -> '|' term reg' | epsilon
term -> kleene term'
term' -> kleene term' | epsilon
kleene -> fac kleene' 
kleene' -> * kleene' | epsilon
fac -> alpha | epsilon| '(' reg ')'
alpha -> 'a' - 'z' | 'A' - 'Z' | '0' - '9'
epsilon -> '!'

抽象语法树(Abstract Syntax Tree, AST)是简化后的语法树. 
即线性输入的正则表达式所对应的二维树结构. 如

(a|b)*abb的AST为：

                     Seq
                      |
                   +------+
                   |      |
                  Seq     $(end of input)
                   |
	        +-----+
                |     | 
               Seq    b
                |
             +-----+
             |     | 
            Seq    b
             |
          +-----+   
          |     |
         Star   a
          |
          Or
          |
       +-----+
       |     |
       a     b

为此，定义AST数据类型为（见ast.h）：
typedef enum { Star = 4, Seq = 3, Or = 2, 
               Alpha = 5, Epsilon = 6} Kind;

typedef struct ast {
  Kind op;
  struct ast *lchild, *rchild;
  char val;
  int pos; /* the position of character */
  ATT attribute; /* the attribute for DFA generation */
  int index;
}  AST;

typedef AST * AST_PTR;

对AST类型的操作见ast.h及ast.c. 其中函数print_ast(AST *)
完成生成好的语法树转换为graphviz(https://www.graphviz.org/)
的输入文件格式，并输出到ast.gv中.
通过"dot -Tpdf ast.gv -o ast.pdf"(linux)
或Windows的gvedit即可以图形方式输出该树结构.

现已给出：
set.h (后续转换dfa使用)
ast.h
ast.c
main.c
unixmake.mak
tcmake.mak
reg2dfa1.exe (样本执行文件)

要求完成词法及语法分析模块parser.c，使得程序在正确完成语法分析的
同时生成输入正则表达式的AST. 

其中递归下降分析函数的原型如下所示：

AST_PTR reg();
AST_PTR reg1(AST_PTR term_left); /* reg' in grammar */
AST_PTR term ();
AST_PTR term1(AST_PTR kleene_left); /* term' in grammar */
AST_PTR kleene();
AST_PTR kleene1( AST_PTR fac_left); /* kleene' in grammar */
AST_PTR fac();

思考：
编写一个对正则表达式的AST遍历的函数，要求输出该正则式对应的
最简式(即括号最少). 如：
(((a(bc)(d*))|e)|f) 的最简式为：abcd*|e|f



请将完成后的parser.c程序(不要其他的文件，也不要打包压缩)
mailto: hanfei.wang@gmail.com
邮件主题请标明: 学号(2)
注意：学号和(2)一定是ASCII码. 其中3表示第三次编程作业. 

hfwang

2018.09.25
