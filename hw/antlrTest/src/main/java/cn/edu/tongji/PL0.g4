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
// <分程序> -> [<常量说明>][<变量说明>]<语句>
subprogram : constantDeclaration? variableDeclaration? statement ;
// <常量说明> -> CONST <常量定义> {,<常量定义>};
constantDeclaration : CONST constantDefinition (COMMA constantDefinition)* SEMI ;
// <常量定义> -> <标识符>:=<无符号整数>
constantDefinition : identifier ASSIGN unsignedInteger ;
// <无符号整数> -> <数字>{<数字>}
unsignedInteger : NUM ;
// <变量说明> -> VAR<标识符>{,<标识符>};
variableDeclaration : VAR identifier (COMMA identifier)* SEMI ;
// <复合语句> -> BEGIN<语句>{;<语句>}END
compoundStatement : BEGIN statement (SEMI statement)* END ;
// <语句> -> <赋值语句> | <条件语句 > | <循环语句> | <复合语句> | <空语句>
statement : assignmentStatement | conditionStatement | loopStatement | compoundStatement | emptyStatement ;
// <赋值语句> -> <标识符>:=<表达式>
assignmentStatement : identifier ASSIGN expression ;
// <表达式> -> [+|-]<项> | <表达式> <加法运算符> <项>
expression : (PLUS | MINUS)? term | expression (PLUS | MINUS) term;
// <项> -> <因子>|<项><乘法运算符><因子>
term : factor | term (TIMES | DIV) factor ;
// <因子> -> <标识符>|<无符号整数>|(表达式)
factor : identifier | unsignedInteger | LPAREN expression RPAREN ;
// <条件语句> -> IF <条件> THEN <语句>
conditionStatement : IF condition THEN m statement ;
// <循环语句> -> WHILE <条件> DO <语句>
loopStatement : WHILE m condition DO m statement ;
// <条件> -> <表达式> <关系运算符> <表达式>
condition : expression relationalOperator expression ;
// <关系运算符> -> = | <> | < | <= | > | >=
relationalOperator : EQUAL | NEQ | LT | LE | GT | GE ;
// <空语句>
emptyStatement : ;
// <标识符>
identifier : ID ;
// 记录地址
m : ;
