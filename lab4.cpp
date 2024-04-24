#include <iostream>

using namespace std;

//Вариант 14: Найти сумму кубов всех положительных элементов массива A={a[i]}.

int input_correct_size() {
	int n;
	cout << "Enter the number of elements in array: ";
	while (!(cin >> n) || cin.peek() != '\n' || (n < 1)) { //пока не введено n, или следующий символ не равен переводу строки, или введено меньше 1
		cin.clear(); //очищаем поток от флага ошибки
		cin.ignore(32767, '\n'); //очищаем поток от символов
		cout << "Error, enter a positive number bigger than 0: ";
	}
	return n;
}

int input_correct_number(int i) {
	int n;
	cout << "Enter the element number "<<i<<": ";
	while (!(cin >> n) || cin.peek() != '\n') { //пока не введено n, или следующий символ не равен переводу строки
		cin.clear(); //очищаем поток от флага ошибки
		cin.ignore(32767, '\n'); //очищаем поток от символов
		cout << "Error, enter an integer: ";
	}
	return n;
}

int sum_of_cubes(int* mas, int size_of_mas) {
	int result = 0;
	int flag = 0;
	__asm {
		xor esi, esi; регистр индекса в массиве;
		xor edi, edi; счетчик результата;
		mov ebx, mas; ebx указывает на начало массива;
		mov ecx, size_of_mas; в ecx количество элементов массива;
		;jecxz exit;

	begin_loop:
		mov eax, [ebx + 4 * esi]; eax = текущий элемент массива;
		cmp eax, 0; сравнение eax с нулем;
		jle end_loop; если eax <= 0, переходим в конец цикла;
		; mov edx, eax; edx = текущий элемент массива;
		imul eax; eax = элемент ^ 2;
		jo error_of;
		mov edx, [ebx + 4 * esi]; edx = текущий элемент массива;
		imul edx; eax = элемент ^ 3;
		jo error_of;
		add edi, eax; прибавляем элемент массива к регистру, считающему результат;
		jo error_of;
	end_loop:
		inc esi; увеличиваем индекс;
		loope begin_loop; переходим к началу цикла;
		jmp end_program;
	error_of:
		mov flag, 1;
	end_program:
		mov eax, edi;
		mov result, eax;
	}
	if (flag == 1) {
		throw runtime_error("Overflow error");
	}
	return result;
}

int main()
{
	int size_of_array = input_correct_size();
	int* arr = new int[size_of_array];
	for (int i = 0; i < size_of_array; ++i) {
		arr[i] = input_correct_number(i);
	}
	try {
		cout << sum_of_cubes(arr, size_of_array)<<endl;
	}
	catch (runtime_error& e) {
		cout << e.what() << endl;
	}
}