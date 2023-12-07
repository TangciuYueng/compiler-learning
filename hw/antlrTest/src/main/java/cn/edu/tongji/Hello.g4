grammar Hello;      // 定义一个名为Hello的语法，名字与文件名一致
prule : 'hello' ID;
ID : 'a'..'z' | [0-9]+;

//r : 'Hello' ID      // 定义一个r规则，匹配一个关键字Hello和紧随其后的标识符ID
//  | 'hello' ID      // "|"是备选分支的分隔符
//  ;
//
//ID : [a-z]+;        // 定义ID标识符，由小写字符组成

WS : [ \t\r\n]+ -> skip;    // 忽略空格、Tab、换行以及\r （Windows）
