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

//    这是main方法，程序的入口点。它抛出IOException异常，表示可能会有I/O操作引发异常。Path filePath是存储PL/0源代码文件路径的对象。
    public static void main(String[] args) throws IOException {
        Path filePath = Paths.get("PL0SourceCode.txt");
        try {
            // 在这里，我们创建了一个词法分析器PL0Lexer，
            // 它接受一个包含源代码的字符流作为输入。
            PL0Lexer lexer = new PL0Lexer(CharStreams.fromPath(filePath));
            // 然后，我们创建了一个语法分析器PL0Parser，它使用上一步创建的词法分析器生成的token流作为输入。
            PL0Parser parser = new PL0Parser(new CommonTokenStream(lexer));

            // 在这里，我们设置parser生成解析树（parse tree）的选项，
            // 并通过调用parser.program()解析整个程序。PL0Parser.ProgramContext是程序的根上下文。
            parser.setBuildParseTree(true);
            PL0Parser.ProgramContext context = parser.program();
            // 在这里，我们创建了一个自定义的PL0VisitorImpl对象visitor，它是我们实现的PL/0语言的访问者。
            // 我们调用visitor.visit(context)来访问解析树，并进行语义分析和代码生成等操作。最后，我们调用visitor.output()来输出结果。
            PL0VisitorImpl visitor = new PL0VisitorImpl();
            visitor.visit(context);
            visitor.output();

            System.out.println("finished!");
        } catch (Exception e) {
            System.out.println("语法错误 " + e.getMessage());
        }

    }
}
