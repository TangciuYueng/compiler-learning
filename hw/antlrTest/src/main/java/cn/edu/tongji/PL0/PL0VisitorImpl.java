package cn.edu.tongji.PL0;

public class PL0VisitorImpl extends PL0BaseVisitor<String> {
    private int tempVarCounter = 0;
    private String newTempVar() {
        return "t" + tempVarCounter++;
    }
    // +|- 运算 控制 条件
//    @Override
//    public String visitAssignmentStatement(PL0Parser.AssignmentStatementContext ctx) {
//        String id = ctx.identifier().getText();
//        String expr = visit(ctx.expression());
//        System.out.println(id + " := " + expr);
//        return super.visitAssignmentStatement(ctx);
//    }

    @Override
    public String visitExpression(PL0Parser.ExpressionContext ctx) {
        var expr = ctx.expression();
        String op;
        var minus = ctx.MINUS();
        if (expr != null) {
            // expression (PLUS|MINUS) term
            // 判断符号
            if (minus != null) {
                op = minus.getText();
            } else {
                op = ctx.PLUS().getText();
            }
            // 递归调用
            String left = visit(expr);
            String right = visit(ctx.term());
            String res = newTempVar();
            // 打印中间代码
            System.out.println(res + " = " + left + " " + op + " " + right);
            return res;
        } else {
            // (PLUS | MINUS)? term情况
            var plus = ctx.PLUS();

            String term = visit(ctx.term());

            if (minus != null) {
                String res = newTempVar();
                System.out.println(res + " = " + minus.getText() + term);
                return res;
            } else if (plus != null) {
                String res = newTempVar();
                System.out.println(res + " = " + plus.getText() + term);
                return res;
            }
            return ctx.getText();
        }
    }

    @Override
    public String visitTerm(PL0Parser.TermContext ctx) {
        var term = ctx.term();
        if (term != null) {
            // term (TIMES | DIV) factor
            var times = ctx.TIMES();
            String op;
            if (times != null) {
                op = times.getText();
            } else {
                op = ctx.DIV().getText();
            }
            // 递归调用
            String left = visit(term);
            String right = ctx.factor().getText();
            String res = newTempVar();
            // 打印中间代码
            System.out.println(res + " = " + left + " " + op + " " + right);
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
