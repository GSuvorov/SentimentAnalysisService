#ifndef CoutConvert_H
#define CoutConvert_H

#include <string>
#include <memory>

//Включать файл необходимо только по мере необходимости, не стоит включать его в stdafx.h, т.к.
//в этом случае он поменяет поведение потока cout во всём проекте, а остальные потоки ostream, 
//будут работать меделеннее (незначительно), в связи с дополнительной проверкой
   
/**
	Набор перегруженных операторов << для корректного вывода русских строк на консоль. Поддерживаются как простые так и 
	UNICODE строки. Перед выводом строка конвертируется в необходимый OEM формат и отправляется на консоль.
	Вообще возможность перегрузки этих операторов позволяется благодаря правилу: Обычная функция имеет больший приоретет при
	разрешении перегрузки чем шаблонная, а все операторы << которые я перегружаю определены как шаблонные функции.
	Пример: эта функция корректно отображает все строки в консоле:
	
	int main(int argc, _TCHAR* argv[])
	{   
		wchar_t *WStr = L"Это тест WStr";
		char *Str= "Это тест Str";
		const wchar_t *ConstWStr = L"Это тест ConstWStr";
		const char *ConstStr= "Это тест ConstStr";
		wchar_t rgWStr[] = L"Это тест rgWStr";
		char rgStr[]= "Это тест rgStr";
		const wchar_t rgConstWStr[] = L"Это тест rgConstWStr";
		const char rgConstStr[]= "Это тест rgConstStr";

		std::string	 CString("Это тест std::string");
		std::wstring WString(L"Это тест std::wstring");

		cout << WStr << endl;
		cout << Str << endl;
		cout << ConstWStr << endl;
		cout << ConstStr << endl;
		cout << rgWStr << endl;
		cout << rgStr << endl;
		cout << rgConstWStr << endl;
		cout << rgConstStr << endl;
		cout << "Это тест прямого вывода однобайтовой строки" << endl;
		cout << L"Это тест прямого вывода двухбайтовой строки" << endl;
		cout << CString << endl;
		cout << WString<< endl;;

		return 0;
	}


   Функции преобразовывают в OEM строки лишь для &stream == &cout
*/
namespace std
{
std::ostream &operator<< ( std::ostream &stream, const char * const obj );
std::ostream &operator<< ( std::ostream &stream, const std::string &obj );
std::ostream &operator<< ( std::ostream &stream, const wchar_t * const obj );
std::ostream &operator<< ( std::ostream &stream, const std::wstring &obj );

//Я так и не понял почему компилятор пишет 'operator <<' is ambiguous при выводе например std::list<T>::size()
//std::ostream &operator<<( std::ostream &stream, allocator<int>::size_type obj );
} //namespace std

#endif