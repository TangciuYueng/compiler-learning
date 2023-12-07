package cn.edu.tongji;

import org.antlr.v4.runtime.ANTLRInputStream;
import org.antlr.v4.runtime.CommonTokenStream;

public class HelloMain {
    public static void main(String[] args) {
        String[] testStr = {
                "Hello world",
                "hello world",
                "hi world"
        };
        for (String s: testStr) {
            System.out.println("Input: " + s);
            run(s);
        }
    }
    public static void run(String expr) {
        ANTLRInputStream input = new ANTLRInputStream(expr);
        hello.HelloLexer lexer = new hello.HelloLexer(input);
        CommonTokenStream tokens = new CommonTokenStream(lexer);
        hello.HelloParser parser = new hello.HelloParser(tokens);
        parser.r();
    }
}
