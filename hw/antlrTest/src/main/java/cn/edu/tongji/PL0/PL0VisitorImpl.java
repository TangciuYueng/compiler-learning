package cn.edu.tongji.PL0;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;

public class PL0VisitorImpl extends PL0BaseVisitor<String> {
    private static final String FILE_PATH = "midCodes.txt";
    private static final String CONST = "const";
    private static final String VAR = "var";
    private int tempVarCounter = 0;
    private int conditionCounter = 0;
    private int nextCounter = 0;
    private int midCodeCounter = 100;
    private final Map<Integer, String> midCodes = new HashMap<>();
    private final Map<Integer, List<Integer>> trueLists = new HashMap<>();
    private final Map<Integer, List<Integer>> falseLists = new HashMap<>();
    private final Map<Integer, List<Integer>> nextLists = new HashMap<>();
    private final Map<String, String> symbolTable = new HashMap<>();
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
    @Override
    public String visitAssignmentStatement(PL0Parser.AssignmentStatementContext ctx) {
        String id = visit(ctx.identifier());

        String expr = visit(ctx.expression());

        int cnt = getMidCodeCounter();
        midCodes.put(cnt, id + " := " + expr);

        int next = getNextCounter();
        nextLists.put(next, new ArrayList<>());

        return String.valueOf(next);
    }

    @Override
    public String visitConstantDefinition(PL0Parser.ConstantDefinitionContext ctx) {
        String id = ctx.identifier().getText();
        if (symbolTable.containsKey(id)) {
            System.out.println(id + "已声明");
        } else {
            symbolTable.put(id, CONST);
        }
        String uInt = ctx.unsignedInteger().getText();

        int cnt = getMidCodeCounter();
        midCodes.put(cnt, id + " := " + uInt);

        return null;
    }

    @Override
    public String visitVariableDeclaration(PL0Parser.VariableDeclarationContext ctx) {
        for (PL0Parser.IdentifierContext identifierContext : ctx.identifier()) {
            String id = identifierContext.getText();
            // 判断是否已经在符号表中
            if (symbolTable.containsKey(id)) {
                System.out.println(id + "已经声明");
            } else {
                symbolTable.put(id, VAR);
            }
        }
        return null;
    }

    @Override
    public String visitExpression(PL0Parser.ExpressionContext ctx) {
        var expr = ctx.expression();
        String op;
        var minus = ctx.MINUS();
        if (expr != null) {
            // expression (PLUS|MINUS) term
            // 判断符号
            op = (minus != null) ? "-" : "+";

            // 递归调用
            String left = visit(expr);
            String right = visit(ctx.term());
            String res = newTempVar();

            // 打印中间代码
            midCodes.put(getMidCodeCounter(), res + " = " + left + " " + op + " " + right);
            return res;
        } else {
            // (PLUS | MINUS)? term情况
            String term = visit(ctx.term());
            op = (minus != null) ? minus.getText() : (ctx.PLUS() != null ? ctx.PLUS().getText() : "");

            if (!op.isEmpty()) {
                String res = newTempVar();
                midCodes.put(getMidCodeCounter(), res + " = " + op + " " + term);
                return res;
            } else {
                return term;
            }
        }
    }

    @Override
    public String visitTerm(PL0Parser.TermContext ctx) {
        var term = ctx.term();
        if (term != null) {
            // term (TIMES | DIV) factor
            String op = (ctx.TIMES() != null) ? "*" : "/";

            // 递归调用
            String left = visit(term);
            String right = visit(ctx.factor());
            String res = newTempVar();

            // 打印中间代码
            midCodes.put(getMidCodeCounter(), res + " = " + left + " " + op + " " + right);
            return res;
        } else {
            // factor
            return visit(ctx.factor());
        }
    }

    @Override
    public String visitFactor(PL0Parser.FactorContext ctx) {
        var expr = ctx.expression();
        if (expr != null) {
            // 去掉括号
            return visit(expr);
        }
        var uInt = ctx.unsignedInteger();
        if (uInt != null) {
            return ctx.getText();
        }
        return visit(ctx.identifier());
    }

    @Override
    public String visitCondition(PL0Parser.ConditionContext ctx) {
        String left = visit(ctx.expression(0));
        String right = visit(ctx.expression(1));
        String relop = ctx.relationalOperator().getText();

        List<Integer> trueList = new ArrayList<>();
        List<Integer> falseList = new ArrayList<>();

        int cnt1 = getMidCodeCounter();
        trueList.add(cnt1);
        int cnt2 = getMidCodeCounter();
        falseList.add(cnt2);

        int conditionCounter = getConditionCounter();
        trueLists.put(conditionCounter, trueList);
        falseLists.put(conditionCounter, falseList);

        midCodes.put(cnt1, "if " + left + " " + relop + " " + right + " goto");
        midCodes.put(cnt2, "goto");

        return String.valueOf(conditionCounter);
    }

    @Override
    public String visitConditionStatement(PL0Parser.ConditionStatementContext ctx) {
        // 获取条件和语句对应的索引
        String conditionIndexStr = visit(ctx.condition());
        String entryIndexStr = visit(ctx.m());
        String nextIndexStr = visit(ctx.statement());

        int conditionIndex = Integer.parseInt(conditionIndexStr);
        int entryIndex = conditionIndexStr != null? Integer.parseInt(entryIndexStr): -1;

        // 获取条件为真时的列表
        List<Integer> trueList = trueLists.getOrDefault(conditionIndex, new ArrayList<>());
        // 更新条件为真时的中间代码
        for (Integer num : trueList) {
            String midCode = midCodes.get(num);
            midCode = midCode + " " + entryIndex;
            midCodes.put(num, midCode);
        }

        int nextIndex = nextIndexStr != null? Integer.parseInt(nextIndexStr): -1;
        // 更新 nextList 为 falseList + statementNextList
        int next = getNextCounter();
        List<Integer> newNextList = new ArrayList<>(falseLists.getOrDefault(conditionIndex, new ArrayList<>()));
        newNextList.addAll(nextLists.getOrDefault(nextIndex, new ArrayList<>()));

        nextLists.put(next, newNextList);

        return String.valueOf(next);
    }

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

    @Override
    public String visitCompoundStatement(PL0Parser.CompoundStatementContext ctx) {
        String nextIndexStr = visit(ctx.multiStatement());
        int nextIndex = nextIndexStr != null? Integer.parseInt(nextIndexStr): -1;
        List<Integer> newNextList = new ArrayList<>(nextLists.getOrDefault(nextIndex, new ArrayList<>()));

        int next = getNextCounter();
        nextLists.put(next, newNextList);

        return String.valueOf(next);
    }

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
