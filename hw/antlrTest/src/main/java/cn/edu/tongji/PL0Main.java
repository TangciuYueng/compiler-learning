package cn.edu.tongji;

import cn.edu.tongji.PL0.PL0Lexer;
import cn.edu.tongji.PL0.PL0Parser;
import cn.edu.tongji.PL0.PL0VisitorImpl;
import org.antlr.v4.runtime.CharStreams;
import org.antlr.v4.runtime.CommonTokenStream;

import java.io.IOException;
import java.nio.file.Paths;
import java.nio.file.Path;

public class PL0Main {
    public static void main(String[] args) throws IOException {
        Path filePath = Paths.get("PL0SourceCode.txt");

        PL0Lexer lexer = new PL0Lexer(CharStreams.fromPath(filePath));
        PL0Parser parser = new PL0Parser(new CommonTokenStream(lexer));

        parser.setBuildParseTree(true);
        PL0Parser.ProgramContext context = parser.program();
        PL0VisitorImpl visitor = new PL0VisitorImpl();
        visitor.visit(context);
        visitor.output();

        System.out.println("finished!");
    }
}
