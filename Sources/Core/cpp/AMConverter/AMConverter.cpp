/// AMConverter.cpp : Defines the entry point for the DLL application.
///

#include "stdafx.h"
#include "AMConverter.h"

#include "CoConverter.h"

#include ".\TestRunner.h"

/** \mainpage

1.	ОБЩИЕ СВЕДЕНИЯ \n
AMConverter.dll динамически линкуемый модуль, с возможностью статической линковки. 
Назначение: кодирование и декодирование морфологической информации слов, хранящихся в морфологическом словаре.
Данный модуль функционирует в составе программного комплекса дополнительных ограничений по наличию установленного программного обеспечения нет.
Модуль реализован на языке программирования C++ с использованием инструментальных средств из среды разработки Microsoft Visual Studio .Net. \n
2.	 ФУНКЦИОНАЛЬНОЕ НАЗНАЧЕНИЕ \n
Данный модуль решает следующие задачи: \n
•	Кодирование морфологической информации. \n
•	Декодирование морфологической информации. \n
•	Предоставление специальных типов данных для обеспечения разбора морфологических характеристик. \n
Функциональных ограничений нет. \n
3.	ИСПОЛЬЗУЕМЫЕ ТЕХНИЧЕСКИЕ СРЕДСТВА \n
Используется стандартная техническая база, на которую может быть установлена ОС Windows. \n
4.	ВЫЗОВ И ЗАГРУЗКА \n
4.1. Загрузка модуля происходит на этапе выполнения стандартными способами загрузки любых dll. Вызов функций происходит через интерфейс IAMConverter. \n
4.2. Точки входа: \n
•	DllMain \n
•	CreateInstance – точка входа, для внутренних интерфейсов SS \n
5.	ВХОДНЫЕ ДАННЫЕ \n
5.1	Указатель на структру CMorphoFeature  \n
5.2	Набор переменных типа данных int, содержащих информацию для декодирования. \n
6.	ВЫХОДНЫЕ ДАННЫЕ \n
6.1	Указатель на структру CMorphoFeature \n
6.2	Набор указателей на тип данных int, получающих сжатую морфологическую информацию после кодирования информации, содержащейся в структуре CMorphoFeature. \n

*/

SET_LOAD_MANAGER_IMPLEMENTATION;

///новый коментарий
/// вход в модуль

HRESULT CreateInstance(const GUID* pGuid, void** pBase)
{

	try
	{
		if(*pGuid == CLSID_AMConverterMorpho)
			*pBase = (SS::Interface::IBase*)new SS::Core::Features::CCoConverter();
		else
			if(*pGuid == CLSID_AMConverterSyntax)
				*pBase = (SS::Interface::IBase*)new SS::Core::Features::CCoConverter();
		else
			if(*pGuid == CLSID_AMConverterSemantic)
				*pBase = (SS::Interface::IBase*)new SS::Core::Features::CCoConverter();
		else 
			if(*pGuid==CLSID_TestRunner)
				*pBase=(SS::Interface::IBase*)
				new SS::UnitTests::AMConverter::CTestRunner();	
		else
			*pBase = NULL;
	
		return S_OK;
	}
	catch (...) {
		return E_FAIL;
	}
	
}



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

