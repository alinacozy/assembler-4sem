#include <iostream>
using namespace std;

double input_correct_number(const char* name_of_variable) {
    double n;
    cout << "Enter " << name_of_variable << ": ";
    while (!(cin >> n) || cin.peek() != '\n') {
        cin.clear(); //очищаем поток от флага ошибки
        cin.ignore(32767, '\n'); //очищаем поток от символов
        cout << "Error, enter a number: ";
    }
    return n;
}

double result_cpp(double a, double c, double d) {
    return (2 * c - d + 23) / (a / 4 - 1);
}

double result_asm(double a, double c, double d) {
    int g = -1; //переменная которая показывает, есть ли ошибки

    //(2*с — d + 23)/(а/4 - 1);
    const double c1=1, c2 = 2, c4 = 4, c23 = 23;

    __asm
    {
        finit
        fld qword ptr[c]; st(0) = c
        fmul qword ptr[c2]; st(0) = c * 2
        fsub qword ptr[d]; st(0) = c * 2 - d
        fadd qword ptr[c23]; st(0) = c * 2 - d + 23
        fld qword ptr[a]; st(0) = a, st(1) = c * 2 - d + 23
        fdiv qword ptr[c4]; st(0) = a / 4
        fsub qword ptr[c1]; st(0) = a / 4 - 1
        fldz; загружаем в стек 0
        fucomp; сравниваем предыдущий вычисленный результат с 0 и выталкиваем 0
        ; st(0) = a / 4 - 1, st(1) = c * 2 - d + 23
        fnstsw ax; загружаем в ax состояние sw
        sahf; записываем ah в младший бит регистра флагов
        jz error_0; если знаменатель = 0, то ошибка деление на 0
        fdivp st(1), st; делим st(1) на st(0) и выталкиваем st(0), результат в st(0)
        ; fxam;
        jmp finish;
    error_0:
        mov g, 1; ошибка деление на 0;
    finish:
    }
    if (g == 1) {
        throw runtime_error("Error: division by zero\n");
    }

}

int main()
{
    double a, c, d, result = 0;
   
    //cout << "Enter numbers a, c, d: ";
    //cin >> a >> c >> d;
    a = input_correct_number("a");
    c = input_correct_number("c");
    d = input_correct_number("d");
    
    try {
        cout << "Result of assembler: " << result_asm(a, c, d) << endl;
        cout << "Result of c++: " << result_cpp(a, c, d) << endl;
    }
    catch(runtime_error& e) {
        cout << e.what();
    }
    return 0;
}