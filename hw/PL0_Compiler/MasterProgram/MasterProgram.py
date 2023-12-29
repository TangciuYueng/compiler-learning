import sys
from intermediate import mid_codes
from PL0_Compiler import Ui_MainWindow
from LexicalAnalyzer import LexicalAnalyzer
from GrammarAnalyzer import GrammarAnalyzer
from PyQt5.QtCore import QThread, pyqtSignal
from PyQt5.QtWidgets import *


class UiThread(QThread):
    signal_output = pyqtSignal()

    def __init__(self):
        super(UiThread, self).__init__()
        self.signal_output.connect(update_output)

    def run(self):
        global main_window, output, update

        while True:  # 线程运行过程处在永真循环中
            if update:
                self.signal_output.emit()
                update = False


class CompileThread(QThread):
    def __init__(self):
        super(CompileThread, self).__init__()

    def run(self):
        global output, execute, update

        while True:
            if execute:
                pl0_compile()
                update = True
                execute = False


def trigger_compile():
    global execute, input_program
    input_program = main_window.sourceCodeEdit.toPlainText() + "\n#"  # 手动加上分隔符，使得最后一个词强制成词
    execute = True


def update_output():
    global main_window, output

    output = ""

    for num, mid_code in mid_codes.items():
        output += f'{num}: {mid_code}\n'

    main_window.output.setPlainText(output)


def pl0_compile():
    global output, input_program

    lexical_file_path = 'lexical.pkl'
    grammar_file_path = 'LR1.pkl'
    output = ""
    lexical_analyzer = LexicalAnalyzer(lexical_file_path)
    grammar_analyzer = GrammarAnalyzer(grammar_file_path)

    while lexical_analyzer.get_index() < len(input_program):
        word, element, if_lex_continue = lexical_analyzer.analyze_once(input_program[lexical_analyzer.get_index()])

        if word is not None:
            # output += f"{word} {element}\n"
            if_grammar_continue, if_next_word, reduce_num = grammar_analyzer.analyze_once(word, element)

            if not if_lex_continue:
                output += "Lexical Analysis Failed\n"  # 词法错误
                break

            if not if_grammar_continue:
                if if_next_word:
                    output += "Grammar Analysis Success!\n"  # acc
                else:
                    output += "Grammar Analysis Failed\n"    # 语法错误
                break
            else:
                while not if_next_word:  # 归约（连续归约若出现，则直到要移进、无法继续时停止循环）
                    if_grammar_continue, if_next_word, reduce_num = grammar_analyzer.analyze_once(word, element)

                    if not if_grammar_continue:
                        if if_next_word:
                            output += "Grammar Analysis Success!\n"  # acc
                        else:
                            output += "Grammar Analysis Failed\n"  # 语法错误
                        break


class MyMainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self):
        super().__init__()
        self.setupUi(self)  # 调用 setupUi 方法来设置界面


if __name__ == '__main__':
    app = QApplication(sys.argv)
    main_window = MyMainWindow()
    main_window.compileButton.clicked.connect(trigger_compile)
    main_window.show()
    execute = False
    update = False
    output = ""
    input_program = ""

    ui_thread = UiThread()
    ui_thread.start()
    compile_thread = CompileThread()
    compile_thread.start()

    sys.exit(app.exec_())
    