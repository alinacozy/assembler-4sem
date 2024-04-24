#include <iostream>

using namespace std;

double input_correct_number(const char* name_of_variable) {
	double n;
	cout << "Enter " << name_of_variable << ": ";
	while (!(cin >> n) || cin.peek() != '\n') {
		cin.clear(); //очищаем поток от флага ошибки
		cin.ignore(32767, '\n'); //очищаем поток от символов
		cout << "Error, enter an int: ";
	}
	return n;
}

double result_assembler(double a, double b) {
	int error_flag = 0;
	double res; 
	double status; 
	const double c_89=-89, c2=2, c7=7, c5=5;
	__asm {

		finit; инициализация сопроцессора
		fld qword ptr[b]; st(0) = b
		fld qword ptr[a]; st(0) = a, st(1) = b;

		fcomi st, st(1); сравниваем a и b и изменяем регистр флагов процессора

		ja a_bigger; переход если a больше
		jb b_bigger; переход если b больше

		; если равны
		fld qword ptr[c_89]; st(0) = -89, оно и возвращается из функции
		jmp endcalc; переходим в конец;

	a_bigger: 
		; (2 * b - 2 * a) / (a + 7);
		fsubp st(1), st; вычитаем из b а и выталкиваем, st(0) = b - a;
		fmul c2; st(0) = 2(b - a);
		fld qword ptr[a]; st(0) = a, st(1) = 2(b - a);
		fadd c7; st(0) = a + 7, st(1) = 2(b - a);
		fldz; st(0) = 0, st(1) = a + 7, st(2) = 2(b - a);
		fcomip st, st(1); сравниваем 0 и a + 7 и изменяем флаги процессора, и выталкиваем 0;
		je error_0; если a + 7 = 0, ошибка деление на 0;
		fdivp st(1), st; делим и выталкиваем, st(0) = 2(b - a) / (a + 7);
		jmp endcalc;

	b_bigger: 
		; (-b + 5 * a) / b;
		fldz; st(0) = 0, st(1) = a, st(2) = b;
		; fcomp st(1);
		; fnstsw ax; сохраняем регистр флагов сопроцессора
			; sahf; записываем в регистр флагов процессора
		fcomip st, st(2); сравнение b с 0, меняем флаги процессора и выталкиваем 0;
		; st(0) = a, st(1) = b;
		je error_0; переход если b = 0;

		fmul qword ptr[c5]; st(0) = a * 5, st(1) = b;
		fxch st(1); обмен значений st(0) и st(1)
			; st(0)=b, st(1)=a*5
		fsub st(1), st; st(0) = b; st(1) = a * 5 - b;
		fdivp st(1), st; st(0) = (a * 5 - b) / b;
		jmp endcalc;

	error_0 :
		mov error_flag, 1; ошибка деления на ноль
	endcalc : 
		; анализируем результат
	}
	if (error_flag == 1) {
		throw runtime_error("Error: division by zero!\n");
	}
}

double result_cpp(double a, double b) {
	if (a > b)
		return (2 * b - 2 * a) / (a + 7);
	if (a == b)
		return -89;
	return (-b + 5 * a) / b;
}

int main()
{
	double a = input_correct_number("a");
	double b = input_correct_number("b");

	try {
		cout << "Result on allembler: " << result_assembler(a, b) << endl;
		cout << "Result on c++: " << result_cpp(a, b) << endl;
	}
	catch (runtime_error& e) {
		cout << e.what();
	}
}
