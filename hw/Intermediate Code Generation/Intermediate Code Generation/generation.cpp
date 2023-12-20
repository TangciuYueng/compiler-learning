#include <iostream>
#include <vector>
#include <map>
#include<string>
class PL0Parser {
    // 在这里定义所需的词法分析器标记（例如 PROGRAM、BEGIN 等）。

public:
    // 在这里定义解析器规则。

private:
    int tempVarCounter = 0;
    int conditionCounter = 0;
    int nextCounter = 0;
    int midCodeCounter = 100;
    std::map<int, std::string> midCodes;
    std::map<int, std::vector<int>> trueLists;
    std::map<int, std::vector<int>> falseLists;
    std::map<int, std::vector<int>> nextLists;

    // 辅助函数
    std::string newTempVar() {
        return "t" + std::to_string(tempVarCounter++);
    }

    int getMidCodeCounter() {
        return midCodeCounter++;
    }

    int getConditionCounter() {
        return conditionCounter++;
    }

    int getNextCounter() {
        return nextCounter++;
    }

public:
    // C++ 版本的 visitAssignmentStatement 函数
    std::string visitAssignmentStatement(const std::string& id, const std::string& expr) {
        int cnt = getMidCodeCounter();
        midCodes[cnt] = id + " := " + expr;

        int next = getNextCounter();
        nextLists[next] = std::vector<int>();

        return std::to_string(next);
    }

    // 定义其他的 visit 函数，用于处理不同的解析器规则...

    // 输出生成的中间代码
    void output() {
        for (const auto& entry : midCodes) {
            std::cout << entry.first << ": " << entry.second << std::endl;
        }
    }
};

int main() {
    // 示例用法
    PL0Parser parser;
    std::string assignmentStatementResult = parser.visitAssignmentStatement("a", "b + c");
    std::cout << "赋值语句结果：" << assignmentStatementResult << std::endl;

    // 其他测试...

    // 输出生成的中间代码
    parser.output();

    return 0;
}
