package cn.edu.tongji.PL0;

public class PL0VisitorImpl extends PL0BaseVisitor<String> {
    private int tempVarCounter = 0;
    private int midCodeCounter = 100;
    private String newTempVar() {
        return "t" + tempVarCounter++;
    }
    private String getCounterFormat() {
        return midCodeCounter++ + ": ";
    }
    // +|- 运算 控制 条件
    @Override
    public String visitAssignmentStatement(PL0Parser.AssignmentStatementContext ctx) {
        String id = ctx.identifier().getText();
        String expr = visit(ctx.expression());
        System.out.println(getCounterFormat() + id + " := " + expr);
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
            System.out.println(getCounterFormat() + res + " = " + left + " " + op + " " + right);
            return res;
        } else {
            // (PLUS | MINUS)? term情况
            String term = visit(ctx.term());
            op = (minus != null) ? minus.getText() : (ctx.PLUS() != null ? ctx.PLUS().getText() : "");

            if (!op.isEmpty()) {
                String res = newTempVar();
                System.out.println(getCounterFormat() + res + " = " + op + " " + term);
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
            System.out.println(getCounterFormat() + res + " = " + left + " " + op + " " + right);
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
        } else {
            return ctx.getText();
        }
    }

}
