package cn.edu.tongji.PL0;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;

public class PL0VisitorImpl extends PL0BaseVisitor<String> {
    //指定中间代码文件的路径。
    private static final String FILE_PATH = "midCodes.txt";
    //常量和变量的关键字。
    private static final String CONST = "const";
    private static final String VAR = "var";

    //计数器，用于生成临时变量的编号。
    private int tempVarCounter = 0;

    //计数器，用于生成条件和跳转语句的标签。
    private int conditionCounter = 0;
    private int nextCounter = 0;
    //计数器，用于生成中间代码的编号。
    private int midCodeCounter = 100;

    //映射表，用于存储中间代码、真值链、假值链和下一条指令链。
    private final Map<Integer, String> midCodes = new HashMap<>();
    private final Map<Integer, List<Integer>> trueLists = new HashMap<>();
    private final Map<Integer, List<Integer>> falseLists = new HashMap<>();
    private final Map<Integer, List<Integer>> nextLists = new HashMap<>();

    //符号表，用于存储变量和常量的名称和值。
    private final Map<String, String> symbolTable = new HashMap<>();

    //生成一个新的临时变量名，格式为 t0、t1、t2 等。
    //生成一个新的中间代码、条件标签或跳转标签。
    private String newTempVar() {
        return "t" + tempVarCounter++;
    }
    private int getMidCodeCounter() {
        return midCodeCounter++;
    }
    private int getConditionCounter() {
        return conditionCounter++;
    }
    private int getNextCounter() {
        return nextCounter++;
    }

    // +|- 运算 控制 条件
    //这段代码是一个访问赋值语句的方法，它用于生成赋值语句的中间代码，并返回下一条指令的标签。
    @Override
    public String visitAssignmentStatement(PL0Parser.AssignmentStatementContext ctx) {
        //通过 visit(ctx.identifier()) 访问赋值语句中的标识符，获取标识符的值，并将其存储在 id 变量中。
        String id = visit(ctx.identifier());
        //通过 visit(ctx.expression()) 访问赋值语句中的表达式，获取表达式的值，并将其存储在 expr 变量中。
        String expr = visit(ctx.expression());
        //调用 getMidCodeCounter() 方法生成一个新的中间代码编号 cnt，并将 id + " := " + expr 存储在 midCodes 映射表中，表示赋值语句的中间代码。
        int cnt = getMidCodeCounter();
        midCodes.put(cnt, id + " := " + expr);
        //调用 getNextCounter() 方法生成一个新的跳转标签编号 next，并将一个新的空列表存储在 nextLists 映射表中，表示下一条指令的链。
        int next = getNextCounter();
        nextLists.put(next, new ArrayList<>());
        //返回 next 的字符串表示，即下一条指令的标签。
        return String.valueOf(next);
    }

    //一个访问常量定义的方法，它用于生成常量定义的中间代码
    @Override
    public String visitConstantDefinition(PL0Parser.ConstantDefinitionContext ctx) {
        // 获取常量定义中标识符的文本值
        String id = ctx.identifier().getText();
        // 检查符号表中是否已经存在该标识符
        if (symbolTable.containsKey(id)) {
            System.out.println(id + "已声明");
        } else {
            // 如果不存在，则将其添加到符号表中，值为 CONST
            symbolTable.put(id, CONST);
        }
        // 获取常量定义中无符号整数的文本值
        String uInt = ctx.unsignedInteger().getText();
        // 调用 getMidCodeCounter() 方法生成一个新的中间代码编号 cnt，
        // 并将 id + " := " + uInt 存储在 midCodes 映射表中，表示常量定义的中间代码
        int cnt = getMidCodeCounter();
        midCodes.put(cnt, id + " := " + uInt);
        // 返回 null
        return null;
    }


    //访问变量声明语句，并将变量添加到符号表中。
    @Override
    public String visitVariableDeclaration(PL0Parser.VariableDeclarationContext ctx) {
        // 遍历变量声明中的每个标识符
        for (PL0Parser.IdentifierContext identifierContext : ctx.identifier()) {
            // 获取标识符的文本值
            String id = identifierContext.getText();
            // 判断是否已经在符号表中
            if (symbolTable.containsKey(id)) {
                System.out.println(id + "已经声明");
            } else {
                // 如果不存在，则将其添加到符号表中，值为 VAR
                symbolTable.put(id, VAR);
            }
        }
        // 返回 null
        return null;
    }

//对表达式的访问，并生成相应的中间代码。
@Override
public String visitExpression(PL0Parser.ExpressionContext ctx) {
    var expr = ctx.expression();
    String op;
    var minus = ctx.MINUS();
    if (expr != null) {
        // 表达式形式为 expression (PLUS|MINUS) term
        // 判断符号
        op = (minus != null) ? "-" : "+";

        // 递归调用，访问左侧表达式
        String left = visit(expr);

        // 访问右侧term表达式
        String right = visit(ctx.term());

        // 生成新的临时变量名
        String res = newTempVar();

        // 生成中间代码：res = left op right
        midCodes.put(getMidCodeCounter(), res + " = " + left + " " + op + " " + right);

        // 返回临时变量名
        return res;
    } else {
        // 表达式形式为 (PLUS | MINUS)? term
        String term = visit(ctx.term());
        op = (minus != null) ? minus.getText() : (ctx.PLUS() != null ? ctx.PLUS().getText() : "");

        if (!op.isEmpty()) {
            // 存在符号操作，生成新的临时变量名
            String res = newTempVar();

            // 生成中间代码：res = op term
            midCodes.put(getMidCodeCounter(), res + " = " + op + " " + term);

            // 返回临时变量名
            return res;
        } else {
            // 没有符号操作，直接返回term
            return term;
        }
    }
}

//对项的访问，并生成相应的中间代码
    @Override
    public String visitTerm(PL0Parser.TermContext ctx) {
        var term = ctx.term();
        if (term != null) {
            // term (TIMES | DIV) factor
            String op = (ctx.TIMES() != null) ? "*" : "/";
            // 递归调用，访问左侧子项
            String left = visit(term);
            // 访问右侧因子
            String right = visit(ctx.factor());
            // 生成新的临时变量名
            String res = newTempVar();
            // 生成中间代码：res = left op right
            midCodes.put(getMidCodeCounter(), res + " = " + left + " " + op + " " + right);
            // 返回临时变量名
            return res;
        } else {
            // factor
            return visit(ctx.factor());
        }
    }

    //实现了对因子的访问，并生成相应的中间代码
    @Override
    public String visitFactor(PL0Parser.FactorContext ctx) {
        var expr = ctx.expression();
        if (expr != null) {
            // 去掉括号
            return visit(expr);
        }
        var uInt = ctx.unsignedInteger();
        if (uInt != null) {
            // 返回无符号整数的文本值
            return ctx.getText();
        }
        // 返回标识符的值
        return visit(ctx.identifier());
    }


//对条件语句的访问，并生成相应的中间代码
    @Override
    public String visitCondition(PL0Parser.ConditionContext ctx) {
        // 获取条件语句中的左表达式，并递归调用 visit 方法获取其值
        String left = visit(ctx.expression(0));
        // 获取条件语句中的右表达式，并递归调用 visit 方法获取其值
        String right = visit(ctx.expression(1));
        // 获取条件语句中的关系运算符，并获取其文本值
        String relop = ctx.relationalOperator().getText();

        // 创建两个空的列表，用于存储条件为真和条件为假时需要跳转到的中间代码行号
        List<Integer> trueList = new ArrayList<>();
        List<Integer> falseList = new ArrayList<>();

        // 获取当前中间代码计数器的值，并将其存储在 cnt1 变量中，作为条件为真时需要跳转到的中间代码行号
        int cnt1 = getMidCodeCounter();
        // 将 cnt1 添加到 trueList 中
        trueList.add(cnt1);
        // 获取当前中间代码计数器的值，并将其存储在 cnt2 变量中，作为条件为假时需要跳转到的中间代码行号
        int cnt2 = getMidCodeCounter();
        // 将 cnt2 添加到 falseList 中
        falseList.add(cnt2);

        // 获取当前条件计数器的值，并将其存储在 conditionCounter 变量中
        int conditionCounter = getConditionCounter();
        // 将 trueList 存储在 trueLists 中，以便在后续生成中间代码时使用
        trueLists.put(conditionCounter, trueList);
        // 将 falseList 存储在 falseLists 中，以便在后续生成中间代码时使用
        falseLists.put(conditionCounter, falseList);

        // 将 "if left relop right goto" 添加到 midCodes 中，作为条件为真时需要执行的中间代码
        midCodes.put(cnt1, "if " + left + " " + relop + " " + right + " goto");
        // 将 "goto" 添加到 midCodes 中，作为条件为假时需要执行的中间代码
        midCodes.put(cnt2, "goto");

        // 返回当前条件计数器的值
        return String.valueOf(conditionCounter);
    }


    //访问条件语句和对应的语句块，并生成相应的中间代码。
    @Override
    public String visitConditionStatement(PL0Parser.ConditionStatementContext ctx) {
        // 获取条件和语句对应的索引
        String conditionIndexStr = visit(ctx.condition());
        String entryIndexStr = visit(ctx.m());
        String nextIndexStr = visit(ctx.statement());

        // 将条件和语句对应的索引转换为整数类型
        int conditionIndex = Integer.parseInt(conditionIndexStr);
        int entryIndex = Integer.parseInt(entryIndexStr);

        // 获取条件为真时的列表
        List<Integer> trueList = trueLists.getOrDefault(conditionIndex, new ArrayList<>());
        // 更新条件为真时的中间代码，将 entryIndex 添加到每个中间代码的末尾
        for (Integer num : trueList) {
            String midCode = midCodes.get(num);
            midCode = midCode + " " + entryIndex;
            midCodes.put(num, midCode);
        }

        int nextIndex = nextIndexStr != null ? Integer.parseInt(nextIndexStr) : -1;
        // 更新 nextList 为 falseList + statementNextList
        int next = getNextCounter();
        List<Integer> newNextList = new ArrayList<>(falseLists.getOrDefault(conditionIndex, new ArrayList<>()));
        newNextList.addAll(nextLists.getOrDefault(nextIndex, new ArrayList<>()));

        // 将 newNextList 存储在 nextLists 中，以便在后续生成中间代码时使用
        nextLists.put(next, newNextList);

        // 返回当前下一个计数器的值
        return String.valueOf(next);
    }

//PL/0 语言中的循环语句的代码生成
    @Override
    public String visitLoopStatement(PL0Parser.LoopStatementContext ctx) {
        // 获取入口地址
        String m1Str = visit(ctx.m(0));
        int m1Entry = Integer.parseInt(m1Str);

        // 获取 trueList 和 falseList 的索引
        String conditionIndexStr = visit(ctx.condition());
        int conditionIndex = Integer.parseInt(conditionIndexStr);

        String m2Str = visit(ctx.m(1));
        int m2Entry = Integer.parseInt(m2Str);

        String nextIndexStr = visit(ctx.statement());
        int nextIndex = nextIndexStr != null? Integer.parseInt(nextIndexStr): -1;

        // M2 to E.trueList
        List<Integer> trueList = trueLists.getOrDefault(conditionIndex, new ArrayList<>());
        for (Integer num: trueList) {
            String midCode = midCodes.get(num);
            midCode = midCode + " " + m2Entry;
            midCodes.put(num, midCode);
        }

        // M1 to S1.nextList
        List<Integer> statementNextList = nextLists.getOrDefault(nextIndex, new ArrayList<>());
        for (Integer num: statementNextList) {
            String midCode = midCodes.get(num);
            midCode = midCode + " " + m1Entry;
            midCodes.put(num, midCode);
        }

        // S.nextList = E.falseList
        List<Integer> newNextList = new ArrayList<>(falseLists.getOrDefault(conditionIndex, new ArrayList<>()));
        int next = getNextCounter();
        nextLists.put(next, newNextList);

        // emit j,-,-,M1
        int cnt = getMidCodeCounter();
        midCodes.put(cnt, "goto " + m1Str);

        return String.valueOf(next);
    }

    //实现了 PL/0 语言中复合语句的代码生成
    @Override
    public String visitCompoundStatement(PL0Parser.CompoundStatementContext ctx) {
        String nextIndexStr = visit(ctx.multiStatement());
        int nextIndex = nextIndexStr != null? Integer.parseInt(nextIndexStr): -1;
        List<Integer> newNextList = new ArrayList<>(nextLists.getOrDefault(nextIndex, new ArrayList<>()));

        int next = getNextCounter();
        nextLists.put(next, newNextList);

        return String.valueOf(next);
    }

    //实现了 PL/0 语言中多条语句的代码生成
    @Override
    public String visitMultiStatement(PL0Parser.MultiStatementContext ctx) {
        var m = ctx.m();
        if (m != null) {
            String nextIndexStr = visit(ctx.multiStatement());
            int nextIndex = nextIndexStr != null? Integer.parseInt(nextIndexStr): -1;

            String mEntryStr = visit(m);
            int mEntry = mEntryStr != null? Integer.parseInt(mEntryStr): -1;

            List<Integer> nextList = nextLists.getOrDefault(nextIndex, new ArrayList<>());
            // 回填 multiStatement
            for (Integer num: nextList) {
                String midCode = midCodes.get(num);
                midCode = midCode + " " + mEntry;
                midCodes.put(num, midCode);
            }

            // L.nextList = S.nextList
            String statementNextIndexStr = visit(ctx.statement());
            int statementNextIndex = statementNextIndexStr != null? Integer.parseInt(statementNextIndexStr): -1;

            int next = getNextCounter();
            nextLists.put(next, new ArrayList<>(nextLists.getOrDefault(statementNextIndex, new ArrayList<>())));

            return String.valueOf(next);
        } else {
            // 只有一个 statement
            String nextIndexStr = visit(ctx.statement());
            int nextIndex = nextIndexStr != null? Integer.parseInt(nextIndexStr): -1;
            List<Integer> newNextList = new ArrayList<>(nextLists.getOrDefault(nextIndex, new ArrayList<>()));

            int next = getNextCounter();
            nextLists.put(next, newNextList);

            return String.valueOf(next);
        }
    }

    //实现了 PL/0 语言中语句的代码生成
    @Override
    public String visitStatement(PL0Parser.StatementContext ctx) {
        if (ctx.assignmentStatement() != null) {
            return visit(ctx.assignmentStatement());
        } else if (ctx.conditionStatement() != null) {
            return visit(ctx.conditionStatement());
        } else if (ctx.loopStatement() != null) {
            return visit(ctx.loopStatement());
        } else if (ctx.compoundStatement() != null) {
            return visit(ctx.compoundStatement());
        } else {
            return null;
        }
    }

    //实现了 PL/0 语言中的一些辅助功能：
    @Override
    public String visitM(PL0Parser.MContext ctx) {
        return String.valueOf(midCodeCounter);
    }

    @Override
    public String visitIdentifier(PL0Parser.IdentifierContext ctx) {
        String id = ctx.getText();
        if (!symbolTable.containsKey(id)) {
            System.out.println(id + "未声明");
        }
        return id;
    }

    public void output() {
        try (BufferedWriter writer = Files.newBufferedWriter(Paths.get(FILE_PATH))) {
            for (Map.Entry<Integer, String> entry : midCodes.entrySet()) {
                String line = entry.getKey() + ": " + entry.getValue();
                System.out.println(line);
                writer.write(line);
                writer.newLine();  // 插入换行符
            }
        } catch (IOException e) {
            throw new RuntimeException("Error writing to file", e);
        }

    }
}
