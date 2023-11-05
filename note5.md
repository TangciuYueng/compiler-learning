# 自下而上分析
发现自上而下语法要求太高
我们自下而上规约

移进-规约法：使用一个符号栈，把输入符号逐一移进栈，当**栈顶形成某个产生式右部**时，则将栈顶的这一部分规约为该**产生式的左部符号**

![](./ref/note5-1.PNG)

## 基本问题
- 如何找出或确定**可规约串**
- 对找出的可规约串替换为哪一个非终结符号

## 规范规约
令$G$是一个文法，$S$是文法的开始符号，**若$\alpha \beta \sigma$**是文法$G$的一个**句型**，如果有
$S \Rightarrow^{*} \alpha A \sigma$且$A \Rightarrow^{+} \beta$
则称$\beta$是句型$\alpha \beta \sigma$相对于非终结符$A$的**短语**
特别地，若$A \Rightarrow \beta$，则称$\beta$是句型$\alpha \beta \sigma$关于产生式$A \rightarrow \beta$的直接短语
一个句型的最左直接短语成为**句柄**

短语、句柄是**相对于句型的**

**例子**
$S \rightarrow aAcBe$
$A \rightarrow b$
$A \rightarrow Ab$
$B \rightarrow d$
给出句型$aAbcde$的短语、直接短语和句柄
$S \Rightarrow aAcBe \Rightarrow aAcde \Rightarrow aAbcde$
短语：$d, Ab, aAbcde$
直接短语：$d, Ab$
句柄：$Ab$在最左边的

## 算法优先分析方法
比较简单，但是不规范

## LR分析方法
L表示从左向右扫描输入串，R表示构造最右推导的逆。LR分析法是严格的**规范规约**
不足在于**手工构造分析程序工作量相当大**(YACC即为一个语法分析程序的自动生成器)

### 原理
在移进-规约过程中寻找句柄
- 历史：在分析栈中已移进和规约的符号串
- 展望：根据当前使用的产生式推测未来可能遇到的输入符号串(LR(1)就预测一个，LR(k)就预测k个)
- 现实：当前输入符号


### LR分析其模型

### 分析表

s为shift进栈
r为reduce规约
