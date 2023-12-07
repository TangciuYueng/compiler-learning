grammar PL0;

// 关键字
PROGRAM : 'PROGRAM';
BEGIN : 'BEGIN';
END : 'END';
CONST : 'CONST';
VAR : 'VAR';
WHILE : 'WHILE';
DO : 'DO';
IF : 'IF';
THEN : 'THEN';

// 标识符
// <标识符>→<字母>{<字母> | <数字>}
ID : [a-z] [a-z0-9]*;
// 整数
NUM : [0-9]+;

// 算符 界符
PLUS : '+';
MINUS : '-';
TIMES : '*';
DIV : '/';
ASSIGN : ':=';
EQUAL : '=';
NEQ : '<>';
LT : '<';
GT : '>';
LE : '<=';
GE : '>=';
LPAREN : '(';
RPAREN : ')';
SEMI : ';';
COMMA : ',';

// 忽略空白符
WS : [ \t\r\n]+ -> skip;

// <程序> -> <程序首部> <分程序>
// <程序首部> -> PROGRAM <标识符>
program : PROGRAM ID subprogram ;
// <分程序> -> [<常量说明>][<变量说明>]<语句部分>
subprogram : constant_declaration? variable_declaration? statement_part ;
// <常量说明> -> CONST <常量定义> {,<常量定义>}
constant_declaration : CONST constant_definition (COMMA constant_definition)* ;
// <常量定义> -> <标识符>=<无符号整数>
constant_definition : identifier EQUAL unsigned_integer ;
// <无符号整数> -> <数字>{,<数字>}
unsigned_integer : NUM (COMMA NUM)* ;
// <变量说明> -> VAR<标识符>{,<标识符>}
variable_declaration : VAR identifier (COMMA identifier)* ;
// <语句部分> -> <语句>|<复合语句>
statement_part : statement | compound_statement ;
// <复合语句> -> BEGIN<语句>{;<语句>}END
compound_statement : BEGIN statement (SEMI statement)* END ;
// <语句> -> <赋值语句> | <条件语句 > | <循环语句> | <复合语句> | <空语句>
statement : assignment_statement | condition_statement | loop_statement | compound_statement | empty_statement ;
// <赋值语句> -> <标识符>:=<表达式>
assignment_statement : identifier ASSIGN expression ;
// <表达式> -> [+|-]<项> | <表达式> <加法运算符> <项>
expression : (PLUS | MINUS)? term | expression (PLUS | MINUS) term;
// <项> -> <因子>|<项><乘法运算符><因子>
term : factor | term (TIMES | DIV) factor ;
// <因子> -> <标识符>|<常量>|(表达式)!!!!!!! problem 1
factor : identifier | LPAREN expression RPAREN ;
// <条件语句> -> IF <条件> THEN <语句>
condition_statement : IF condition THEN statement ;
// <循环语句> -> WHILE <条件> DO <语句>
loop_statement : WHILE condition DO statement ;
// <条件> -> <表达式> <关系运算符> <表达式>
condition : expression relational_operator expression ;
// <关系运算符> -> = | <> | < | <= | > | >=
relational_operator : EQUAL | NEQ | LT | LE | GT | GE ;
// <空语句>
empty_statement : ;
// <标识符>
identifier : ID ;

// !!!!!! 复合语句有问题 problem2
