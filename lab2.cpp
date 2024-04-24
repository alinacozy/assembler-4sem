#include <iostream>
using namespace std;

int input_correct_number(const char* name_of_variable) {
    int n;
    cout << "Enter " << name_of_variable<<": ";
    while (!(cin >> n) || cin.peek() != '\n') { 
        cin.clear(); //очищаем поток от флага ошибки
        cin.ignore(32767, '\n'); //очищаем поток от символов
        cout << "Error, enter an int: ";
    }
    return n;
}

int result_cpp(int a, int c, int d) {
    return (2*c - d + 23) / (a / 4 - 1);
}

int main()
{
    int a, c,d, result=0;
    int g = -1; //переменная которая показывает, есть ли ошибки
    //cout << "Enter numbers a, c, d: ";
    //cin >> a >> c >> d;
    a = input_correct_number("a");
    c = input_correct_number("c");
    d = input_correct_number("d");
    //(2*с — d + 23)/(а/4 - 1);
    __asm
    {
        mov eax, a; помещаем a в eax
        cdq; eax = > edx:eax
        mov ebx, 4; помещаем 4 в ebx
        idiv ebx; результат деления сейчас в eax, остаток в edx
        dec eax; eax=a/4-1
        jz error_zf; ошибка знаменатель ноль
        jo error_of; ошибка переполнения

        push eax; в стеке сейчас(a / 4 - 1)

        mov eax, c; eax=c
        mov ebx, 2; ebx=2
        imul ebx; умножили c на 2, c*2 находится в edx : eax
        jo error_of; ошибка переполнения

        sub eax, d; eax=c*2-d
            jo error_of; ошибка переполнения
        add eax, 23; eax=c*2-d+23
            jo error_of; ошибка переполнения
        

        ; готовимся к делению

        pop ebx; достали знаменатель из стека, ebx=знаменатель
        cdq; eax = > edx:eax
        idiv ebx; результат деления сейчас в eax
        jo error_of; ошибка переполнения

        mov result, eax
        jmp end_of_program

            error_zf:
        mov g, 1
        jmp end_of_program
            error_of:
        mov g, 0
            pop ebx; очистка стека
           jmp end_of_program

        end_of_program:
    }
    if (g == 1) {
        cout << "Error: division by zero\n";
    }
    else if (g == 0) {
        cout << "Overflow error\n";
    }
    else {
        cout << "Result of assembler: " << result << endl;
        cout << "Result of c++: " << result_cpp(a, c, d) << endl;
    }
    return 0;
}