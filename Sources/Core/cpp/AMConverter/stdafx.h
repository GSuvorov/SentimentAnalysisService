/** \mainpage
1. ОБЩИЕ СВЕДЕНИЯ
Динамически линкуемый модуль AMConverter.dll – модуль конвертации различных (морфологических, семантических и синтаксических) характеристик\n
Данный модуль функционирует в составе программного комплекса. Дополнительных ограничений по наличию установленного программного обеспечения нет. \n
Модуль реализован на языке программирования C++ с использованием инструментальных средств из среды разработки Microsoft Visual Studio .Net, библиотеки STL \n
\n

2. ФУНКЦИОНАЛЬНОЕ НАЗНАЧЕНИЕ
Данный модуль решает следующие задачи:\n
•	конвертацию морфологических характеристик\n
•	конвертацию семантических характеристик\n
•	конвертацию синтаксических характеристик\n
\n
Список искользуемых интерфейсов:\n
•	SS::Interface::Core::IAMConverterMorpho - выполняет конвертацию морфологических характрестик\n
•	SS::Interface::Core::IAMConverterSyntax - выполняет конвертацию синтаксических характеристик\n
•	SS::Interface::Core::IAMConverterSyntaxAposteriori - выполняет конвертацию апострериорных синтаксических характеристик\n
•	SS::Interface::Core::IAMConverterSemantic - выполняет конвертацию семантических характрестик\n
\n

3. ИСПОЛЬЗУЕМЫЕ ТЕХНИЧЕСКИЕ СРЕДСТВА
Используется стандартная техническая база, на которую может быть установлена ОС Windows. \n
\n
4. ВЫЗОВ И ЗАГРУЗКА
	4.1. Загрузка модуля осуществляется динамически через методы интерфейса CManagerConnections\n\n

	4.2. Точек входа нет.\n\n

5. ВХОДНЫЕ ДАННЫЕ
	Нет.\n\n

6. ВЫХОДНЫЕ ДАННЫЕ
	Нет.\n\n

*/
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define  _WIN32_WINNT 0x0403 
#define APP_ID "AMConverter"
#define NWLIB_SS_FRAMEWORK

// Windows Header Files:

// Windows Header Files:
#include <tchar.h>
#include "../[libs]/FirstHeader.h"
#include <windows.h>
#include <atldbcli.h>
#include "stl.h"

#include "..\ASCInterface\defines.h"
USES_LOAD_MANAGER;

#include "..\FeatureLibrary\define.h"
#include "..\ASSInterface\IAMConverter.h"
#include "..\ASSInterface\TDictionaryManager.h"

// TODO: reference additional headers your program requires here
