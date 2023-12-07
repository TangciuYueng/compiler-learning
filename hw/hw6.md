# 代码
```
while A < C and B < D do
    if A = 1 then
        C := C + 1
    else
        while A <= D do
            A := A + 2
```
# 产生式
1. S $\rightarrow$ if B then M1 S1 N else M2 S2
2. N $\rightarrow \varepsilon$
3. M $\rightarrow \varepsilon$
4. S $\rightarrow$ M1 B do M2 S1
5. S $\rightarrow$ A
6. A $\rightarrow$ id := E
7. E $\rightarrow$ E + E
8. E $\rightarrow$ id/const
9.  B $\rightarrow$ id/const rop id/const
10. B $\rightarrow$ B and B