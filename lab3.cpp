#include <iostream>

using namespace std;

int input_correct_number(const char* name_of_variable) {
	int n;
	cout << "Enter "<<name_of_variable<<": ";
	while (!(cin >> n) || cin.peek() != '\n') { 
		cin.clear(); //очищаем поток от флага ошибки
		cin.ignore(32767, '\n'); //очищаем поток от символов
		cout << "Error, enter an int: ";
	}
	return n;
}

int result_assembler(int a, int b) {
	int result;
	int flag = -1;
	__asm {
		mov ecx, a;
		mov ebx, b;
		cmp ecx, ebx;
		jg a_bigger; переход если a > b;
		jl a_smaller; переход если a < b;
		mov eax, -89;
		jmp success_finish;

	a_bigger:
		; (2b - 2a) / (a + 7);
		sub ebx, ecx; ebx = b - a;
		jo of_error; ошибка переполнения;
		mov eax, 2; eax = 2;
		imul ebx; eax = 2(b - a);
		jo of_error; ошибка переполнения;
		add ecx, 7; ecx = a + 7;
		jz zf_error; ошибка деление на ноль;

		cdq; eax = > eax:edx;
		idiv ecx; eax = 2(b - a) / (a + 7);
		jo of_error; ошибка переполнения;

		jmp success_finish;
	a_smaller:
		; (-b + 5a) / b;
		cmp ebx, 0;
		je zf_error; ошибка деление на ноль;

		mov eax, ecx; eax = a;
		mov edx, 5;
		imul edx; eax = a * 5;
		jo of_error; ошибка переполнения;
		sub eax, ebx; eax = 5a - b;

		cdq; eax = > edx:eax;
		idiv ebx; eax = (5a - b) / b;
		jo of_error; ошибка переполнения;

		jmp success_finish;
	success_finish:
		mov result, eax;
		jmp end_of_program;
	zf_error:
		mov flag, 0;
		jmp end_of_program;
	of_error:
		mov flag, 1;
		jmp end_of_program;

	end_of_program:

	}
	if (flag == 0) {
		throw runtime_error("Error: division by zero!\n");
	}
	else if (flag == 1) {
		throw runtime_error("Overflow error!\n");
	}
	return result;
}

int result_cpp(int a, int b) {
	if (a > b)
		return (2 * b - 2 * a) / (a + 7);
	if (a == b)
		return -89;
	return (-b + 5 * a) / b;
}

int main()
{
	int a = input_correct_number("a");
	int b = input_correct_number("b");

	try {
		cout <<"Result on allembler: "<< result_assembler(a,b) << endl;
		cout << "Result on c++: " << result_cpp(a, b)<<endl;
	}
	catch (runtime_error& e) {
		cout << e.what();
	}
}
