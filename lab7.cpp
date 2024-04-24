#include <iostream>

using namespace std;

const double PI = 3.141592653589793;
double k_final = 0;

double input_correct_x() {
	double n;
	cout << "Enter x: ";
	while (!(cin >> n) || cin.peek() != '\n' || n<0 || n>PI/2) {
		cin.clear(); //очищаем поток от флага ошибки
		cin.ignore(32767, '\n'); //очищаем поток от символов
		cout << "Error, enter a number between 0 and PI/2: ";
	}
	return n;
}
double input_correct_epsilon() {
	double n;
	cout << "Enter epsilon: ";
	while (!(cin >> n) || cin.peek() != '\n' || n<=0) {
		cin.clear(); //очищаем поток от флага ошибки
		cin.ignore(32767, '\n'); //очищаем поток от символов
		cout << "Error, enter a positive number: ";
	}
	return n;
}

double result_assembler(double x, double eps) {
	int k = 0;
	int c_1 = -1;
	int c2 = 2;
	
	__asm {
		finit; инициализация сопроцессора
		fld qword ptr[eps]; st(0) = eps
		fld qword ptr[x]; st(0) = x, st(1) = eps;
		fldz; st(0) = 0, st(1) = x, st(2) = eps;
		fld1; st(0) = 1 (k), st(1) = 0 (sum), st(2) = x, st(3) = eps;

	begin_loop:
		fld qword ptr[x]; st(0) = x, st(1) =k, st(2) = sum, st(3) = x, st(4) = eps;
		fxch st(1); st(0) = k, st(1) = x, st(2) = sum, st(3) = x, st(4) = eps;
		fmul st(1), st; st(0) = k, st(1) = k*x, st(2) = sum, st(3) = x, st(4) = eps;
		fxch st(1); st(0) = k*x, st(1) = k, st(2) = sum, st(3) = x, st(4) = eps;
		fcos; st(0) = cos(k * x), st(1) = k, st(2) = sum, st(3) = x, st(4) = eps;
		fxch st(1); st(0) = k, st(1) = cos(k * x), st(2) = sum, st(3) = x, st(4) = eps;
		fdiv st(1), st; st(0) = k, st(1) = cos(k * x) / k, st(2) =sum, st(3) = x, st(4) = eps;
		fdiv st(1), st; st(0) = k, st(1) = cos(k * x) / k ^ 2, st(2) = sum, st(3) = x, st(4) = eps;
		
		; узнаем остаток от деления k на 2, исходя из этого определим(-1) ^ k
		fist k; получаем из стека сопроцессора число k;
		mov eax, k; eax = k;
		cdq; eax->edx:eax;
		mov ebx, 2;
		div ebx; eax - частное, edx - остаток;
		cmp edx, 0; сравниваем остаток с 0;
		je k_even; если остаток = 0, k - четное, знак члена ряда положительный;

		; fldz; st(0) = 0, st(1) = k, st(2) = cos(k * x) / k ^ 2, st(3) = sum, st(4) = x, st(5) = eps;
		; fadd; st(0) = k, st(1) = k, st(2) = cos(k * x) / k ^ 2, st(3) = sum, st(4) = x, st(5) = eps;
		; fild dword ptr[c2]; st(0) = 2, st(1) = k, st(2) = k, st(3) = cos(k * x) / k ^ 2, st(4) = sum, st(5) = x, st(6) = eps;
		; fxch st(1); st(0) = k, st(1) = 2, st(2) = k, st(3) = cos(k * x) / k ^ 2, st(4) = sum, st(5) = x, st(6) = eps;
		; fprem; st(0) = k % 2, st(1) = 2, st(2) = k, st(3) = cos(k * x) / k ^ 2, st(4) = sum, st(5) = x, st(6) = eps;

		; fxch st(1); st(0) = 2, st(1) = k % 2, st(2) = k, st(3) = cos(k * x) / k ^ 2, st(4) = sum, st(5) = x, st(6) = eps;
		;; удаляем двойку
			; fstp temp; st(0) = k % 2, st(1) = k, st(2) = cos(k * x) / k ^ 2, st(3) = sum, st(4) = x, st(5) = eps;
		; fldz; st(0) = 0, st(1) = k % 2, st(2) = k, st(3) = cos(k * x) / k ^ 2, st(4) = sum, st(5) = x, st(6) = eps;
		; fcomip st, st(1);
		; je k_even;

		;иначе умножаем член ряда на -1
		fild dword ptr[c_1]; st(0)=-1, st(1) = k, st(2) = cos(k * x) / k ^ 2, st(3) = sum, st(4) = x, st(5) = eps;
		fmulp st(2), st; st(0) = k, st(1) = -cos(k * x) / k ^ 2, st(2) = sum, st(3) = x, st(4) = eps;
	k_even:
		;меняем местами k и член ряда (член ряда наверх стека)
		fxch st(1); st(0) = +-cos(k * x) / k ^ 2, st(1) = k, st(2) = sum, st(3) = x, st(4) = eps;

		;добавляем вычисленный член суммы к сумме
		fadd st(2), st; st(0) = +-cos(k * x) / k ^ 2, st(1) = k, st(2) = sum+st(0), st(3) = x, st(4) = eps;

		; увеличиваем k;
		;добавляем 1 в вершину стека
		fld1;  st(0)=1, st(1) = +-cos(k * x) / k ^ 2, st(2) = k, st(3) = sum + C_k, st(4) = x, st(5) = eps;
		faddp st(2), st; st(0) = +-cos(k * x) / k ^ 2, st(1) = k+1, st(2) = sum + C_k, st(3) = x, st(4) = eps;
		; берем модуль от члена ряда;
		fabs; st(0) = | +-cos(k * x) / k ^ 2 | , st(1) = k + 1, st(2) = sum + C_k, st(3) = x, st(4) = eps;

		; сравниваем модуль члена ряда с погрешностью, выталкиваем st(0) и изменяем флаги процессора
		fcomip st, st(4); st(0) = k + 1, st(1) = sum + C_k, st(2) = x, st(3) = eps;
		; если член ряда больше погрешности, позвращаемся в начало цикла;
		ja begin_loop;

		fstp k_final;
		
	}
	
}


double result_cpp(double x) {
	return (pow(x, 2) - pow(PI, 2) / 3) / 4;
}

int main()
{
	double x = input_correct_x();
	double eps = input_correct_epsilon();

	double analitic = result_cpp(x);
	double sum_of_row = result_assembler(x, eps);
	cout << "Result on c++: " << analitic << endl;
	cout << "Result on allembler: " << sum_of_row << endl;
	cout << "It took " << k_final - 1 << " iterations.\n";
	cout << "Error rate: " << analitic - sum_of_row;
}
