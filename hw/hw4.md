考虑下面文法$G_1$: 
$S \rightarrow a | \wedge | (T)$
$T \rightarrow T,S | S$

- 消去$G_1$的左递归。然后，对每个非终结符，写出不带回溯的递归子程序
- 经改写后的文法是否是LL(1)的？给出它的预测分析表

## 消除左递归
$S \rightarrow a | \wedge | (T)$
$T \rightarrow ST'$
$T'\rightarrow ,ST' | \varepsilon$

## 不带回溯的递归子程序
### P(S)
```
IF ch = "a" THEN 
    read(ch);
ELSE IF ch = "∧" THEN 
    read(ch);
ELSE IF ch = "(" THEN
    BEGIN
        read(ch);
        P(T);
        IF ch = ")" THEN
            read(ch);
        ELSE ERROR
    END
ELSE ERROR;
```
### P(T)
```
BEGIN
    P(S);
    P(T');
END;
```
### P(T')
```
IF ch = "," THEN
    BEGIN
        read(ch);
        P(S);
        P(T');
    END
ELSE IF ch = ")" THEN
    RETURN;
ELSE ERROR;
```
## 求出FIRST与FOLLOW
$FIRST(S) = \{a, \wedge, ( \}$
$FIRST(T) = \{a, \wedge, ( \}$
$FIRST(T') = \{,, \varepsilon \}$

$FOLLOW(S) = \{\#\} + FIRST(T') - \{ \varepsilon \} + FOLLOW(T) + FOLLOW(T') = \{ \#, ,, ) \}$
$FOLLOW(T) = \{ ) \}$
$FOLLOW(T') = FOLLOW(T) = \{ ) \}$

## 检查是否满足LL(1)
- 已经消除左递归
- 检查$FIRST$
  - 对于$S \rightarrow a | \wedge | (T)$
    - $FIRST(a) = \{a\}, FIRST(\wedge) = \{\wedge\}, FIRST((T)) = \{(\}$
    - 因此上述两两不相交
  - 对于$T' \rightarrow ,ST'|\varepsilon$所右部同样两两不相交
- 检查$FOLLOW$
  - 对于$\varepsilon \in FIRST(T')$因此检查$FIRST(T') \cap FOLLOW(T') = \varnothing$

因此满足LL(1)文法

## 预测分析表
根据每个非终结符的首符集填入相应产生式
注意$T'$可以推出$\varepsilon$，因此需要看到$T'$的后继符集填入$T' \rightarrow \varepsilon$
|&nbsp;|$a$|$\wedge$|$($|$)$|$,$|$\#$|
|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
|$S$|$S \rightarrow a$|$S \rightarrow \wedge$|$S \rightarrow (T)$|&nbsp;|&nbsp;|&nbsp;|
|$T$|$T \rightarrow ST'$|$T \rightarrow ST'$|$T \rightarrow ST'$|&nbsp;|&nbsp;|&nbsp;|
|$T'$|&nbsp;|&nbsp;|&nbsp;|$T' \rightarrow \varepsilon$|$T' \rightarrow ,ST'$|&nbsp;|