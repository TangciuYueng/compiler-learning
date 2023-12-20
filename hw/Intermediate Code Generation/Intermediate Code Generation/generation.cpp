#include <iostream>
#include <vector>
#include <map>
#include<string>
class PL0Parser {
    // �����ﶨ������Ĵʷ���������ǣ����� PROGRAM��BEGIN �ȣ���

public:
    // �����ﶨ�����������

private:
    int tempVarCounter = 0;
    int conditionCounter = 0;
    int nextCounter = 0;
    int midCodeCounter = 100;
    std::map<int, std::string> midCodes;
    std::map<int, std::vector<int>> trueLists;
    std::map<int, std::vector<int>> falseLists;
    std::map<int, std::vector<int>> nextLists;

    // ��������
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
    // C++ �汾�� visitAssignmentStatement ����
    std::string visitAssignmentStatement(const std::string& id, const std::string& expr) {
        int cnt = getMidCodeCounter();
        midCodes[cnt] = id + " := " + expr;

        int next = getNextCounter();
        nextLists[next] = std::vector<int>();

        return std::to_string(next);
    }

    // ���������� visit ���������ڴ���ͬ�Ľ���������...

    // ������ɵ��м����
    void output() {
        for (const auto& entry : midCodes) {
            std::cout << entry.first << ": " << entry.second << std::endl;
        }
    }
};

int main() {
    // ʾ���÷�
    PL0Parser parser;
    std::string assignmentStatementResult = parser.visitAssignmentStatement("a", "b + c");
    std::cout << "��ֵ�������" << assignmentStatementResult << std::endl;

    // ��������...

    // ������ɵ��м����
    parser.output();

    return 0;
}
