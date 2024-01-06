import sys

import intermediate
from intermediate import reset, get_mid_codes
from PL0_Compiler import Ui_MainWindow
from LexicalAnalyzer import LexicalAnalyzer
from GrammarAnalyzer import GrammarAnalyzer
from styles import *
from PyQt5.QtCore import QThread, pyqtSignal
from PyQt5.QtWidgets import *


class UiThread(QThread):
    signal_update = pyqtSignal()

    def __init__(self):
        super(UiThread, self).__init__()
        self.signal_update.connect(update_window)

    def run(self):
        global main_window, update

        while True:  # 线程运行过程处在永真循环中
            if update:
                self.signal_update.emit()
                update = False


class CompileThread(QThread):
    def __init__(self):
        super(CompileThread, self).__init__()

    def run(self):
        global execute, update, intermediate_file_path

        while True:
            if execute:
                pl0_compile()
                write_mid_codes_to_file(intermediate_file_path)
                update = True
                execute = False


def trigger_compile():
    global execute, input_program
    reset_master()
    reset()
    input_program = main_window.sourceCodeEdit.toPlainText() + "\n#"  # 手动加上分隔符，使得最后一个词强制成词
    execute = True


def update_window():
    global main_window, message, success, intermediate_file_path

    if success:
        main_window.messageBox.setStyleSheet(message_text_success_style)
        main_window.messageLabel.setStyleSheet(message_label_success_style)
        message += f"Intermediate code generated, storing in file {intermediate_file_path}"
    else:
        main_window.messageBox.setStyleSheet(message_text_error_style)
        main_window.messageLabel.setStyleSheet(message_label_error_style)

    main_window.outputBox.setPlainText(get_mid_codes())
    main_window.messageBox.setPlainText(message)


def write_mid_codes_to_file(file_path):
    with open(file_path, 'w') as file:
        file.write(get_mid_codes())


def reset_master():
    global execute, update, success, message, input_program, line

    execute = False
    update = False
    success = True
    message = ""
    input_program = ""
    line = 1


def error_operations(origin_message):
    global success, message

    success = False
    message = origin_message


def pl0_compile():
    global message, input_program, line

    lexical_file_path = 'lexical.pkl'
    grammar_file_path = 'LR1.pkl'
    message = ""
    lexical_analyzer = LexicalAnalyzer(lexical_file_path)
    grammar_analyzer = GrammarAnalyzer(grammar_file_path)

    while lexical_analyzer.get_index() < len(input_program):
        intermediate.print_existing_symbols()

        try:
            word, element, if_next_line = \
                lexical_analyzer.analyze_once(input_program[lexical_analyzer.get_index()], line)
        except SyntaxError as e:
            error_operations(str(e))
            return

        if word is not None:
            try:
                if_grammar_continue, if_next_word, reduce_num = \
                    grammar_analyzer.analyze_once(word, element, line)
            except ValueError as e:
                error_operations(str(e))
                return
            except SyntaxError as e:
                error_operations(str(e))
                return

            if lexical_analyzer.get_index() == len(input_program) - 1:  # 词法分析结束
                message += "Lexical Analysis Success!\n"

            if not if_grammar_continue:  # 语法分析停止，根据返回值判断是否出现了报错
                if if_next_word:
                    message += "Grammar Analysis Success!\n"  # acc
                else:
                    message += "Grammar Analysis Failed\n"    # 语法错误
                break
            else:
                while not if_next_word:  # 归约（连续归约若出现，则直到要移进、无法继续时停止循环）
                    try:
                        if_grammar_continue, if_next_word, reduce_num = grammar_analyzer.analyze_once(word, element, line)
                    except ValueError as e:
                        error_operations(str(e))
                        return
                    except SyntaxError as e:
                        error_operations(str(e))
                        return

                    if not if_grammar_continue:
                        if if_next_word:
                            message += "Grammar Analysis Success!\n"  # acc
                        else:
                            message += "Grammar Analysis Failed\n"  # 语法错误
                        break

        if if_next_line:
            line += 1


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
    success = True
    message = ""
    input_program = ""
    line = 1
    intermediate_file_path = 'intermediate.txt'

    ui_thread = UiThread()
    ui_thread.start()
    compile_thread = CompileThread()
    compile_thread.start()

    sys.exit(app.exec_())
    