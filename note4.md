# 语法分析——自上而下
- 从文法的开始符号出发，反复使用各种产生式，寻找与输入符号匹配的最左推导
- 从⽂法的开始符号出发，向下推导，推出句⼦
- 对任何的输入串(单词符号)，试图⽤**⼀切可能的办法**, 从⽂法的开始符号出发，⾃上⽽下地为输入串建⽴⼀棵语法树，即为输入串寻找⼀个最左推导，本质上是一个个试探，效率低代价高

## 问题
- 如果含有**左递归**就会无限循环(因为最左推导)
- 虚假匹配问题
- 回溯引起时间空间大量消耗
- 不成功的时候不知道出错的确切位置
- 
## 分类
### LL(1)分析法
从左(Left)到右扫描输入串，构造最左(Leftmost)推导，分析时每步向前看⼀个(1)字符
#### 消除左递归
[csdnRef](https://blog.csdn.net/qq2071114140/article/details/102787831)
左递归文法
- 直接递归$A \rightarrow A\beta$
- 间接递归$A \rightarrow B\beta$和$B \rightarrow A\alpha$

如果一个文法是左递归时，就i不能采用自顶向下分析法

我们改写
$P \rightarrow P\alpha|\beta$ ($\alpha \neq \epsilon$, $\beta$不以$P$开头)

转换！
$P \rightarrow \beta P^{'}$
$P^{'} \rightarrow \alpha P^{'}|\epsilon$
把递归弄到右边，右边的递归进入点$P$去掉，先取后面的跟上新的递归进入点，然后取写一个新的递归和$\epsilon$

**例如**
$E \rightarrow E + T | T$
转换！
$E \rightarrow TE^{'}$
$E^{'} \rightarrow +T$

如果是间接左递归，先排序，但顺序不重要，生成一个直接递归
$S \rightarrow Qc|c$
$Q \rightarrow Rb|b$
$R \rightarrow Sa|a$

变成直接递归，变一个就好
$S \rightarrow Qc|c$
$Q \rightarrow Rb|b$

**这里注意**
$S$首先可以代入$Qc$和$c$
$R \rightarrow Qca|ca|a$
$Q$然后可以代入$Rb$和$b$
$R \rightarrow Rbca|bca|ca|a$

然后消除左递归
这里相当于$\beta$有三个
$R \rightarrow bcaR^{'}|caR^{'}|aR^{'}$
$R^{'} \rightarrow bcaR^{'}|\epsilon$

$A \rightarrow aB$
$A \rightarrow Bb$
$B \rightarrow Ac$
$B \rightarrow d$

先转换为直接递归，注意这里只有两个因此可以合并为一个
$A \rightarrow aAc|ad|Acb|db$
然后只用管$A$在左边的
$A \rightarrow (aAc|ad|db)A^{'}$
$A^{'} \rightarrow cbA^{'}|\epsilon$

### 递归下降分析法

### 预测分析法