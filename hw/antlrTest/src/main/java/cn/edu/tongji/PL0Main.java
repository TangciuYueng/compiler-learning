package cn.edu.tongji;

import cn.edu.tongji.PL0.PL0Lexer;
import cn.edu.tongji.PL0.PL0Parser;
import cn.edu.tongji.PL0.PL0VisitorImpl;
import org.antlr.v4.runtime.CharStreams;
import org.antlr.v4.runtime.CommonTokenStream;

public class PL0Main {
    public static void main(String[] args) {
        PL0Lexer lexer = new PL0Lexer(CharStreams.fromString(
                "PROGRAM xxx" +
                        "VAR set;" +
                        "BEGIN" +
//                        "   set := 1;" +
//                        "   a := (23+1) * set;" +
                        "   WHILE a > 23 DO " +
                        "   BEGIN " +
                        "       a := a + 23;" +
                        "       IF a > 23 THEN IF b > 23 + 2 THEN set := 23" +
                        "   END;" +
                        "   set := 23" +
                        "END"
                        ));
        PL0Parser parser = new PL0Parser(new CommonTokenStream(lexer));

        parser.setBuildParseTree(true);
        PL0Parser.ProgramContext context = parser.program();
        PL0VisitorImpl visitor = new PL0VisitorImpl();
        visitor.visit(context);
        visitor.output();

        System.out.println("finished!");
    }
}
