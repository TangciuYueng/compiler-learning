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
                        "CONST set:=23;" +
                "VAR a,b,c;" +
                        "BEGIN" +
                        "   a := 23*1 * 5;" +
                        "   b := set;" +
                        "   IF y > 0 THEN y:=-y*3;" +
                        "WHILE x < 2 DO x:=(x+1)/3;" +
                        "a := y+2" +
                        "END;"
                        ));
        PL0Parser parser = new PL0Parser(new CommonTokenStream(lexer));

        parser.setBuildParseTree(true);
        PL0Parser.ProgramContext context = parser.program();
        PL0VisitorImpl visitor = new PL0VisitorImpl();
        visitor.visit(context);

        System.out.println("finished!");
    }
}
