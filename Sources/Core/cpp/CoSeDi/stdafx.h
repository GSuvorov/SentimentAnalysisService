/** \mainpage
1. ОБЩИЕ СВЕДЕНИЯ
	Динамически линкуемый модуль CoSeDi.dll.\n
	Данный модуль функционирует в составе программного комплекса, дополнительных ограничений по наличию установленного программного обеспечения нет.\n
	Модуль реализован на языке программирования C++ с использованием инструментальных средств из среды разработки Microsoft Visual Studio .Net, библиотеки STL.\n\n

2. ФУНКЦИОНАЛЬНОЕ НАЗНАЧЕНИЕ
	Основное назначение модуля - реализация набора интерфейсов по работе с толково-комбинаторным словарем

3. ОПИСАНИЕ ЛОГИЧЕСКОЙ СТРУКТУРЫ
	Модуль предоставляет следующие интерфейсы:
		1. ISemanticClassesTree - содержит набор методов по работе с деревом семантических классов
		2. ISemanticIDSourceTree - содержит набор методов, необходимых для работы с деревом основ
		3. ISemanticAnalysis - содержит набор методов для семантического анализа
		4. ISyntaxAnalysis - интерфейс для синтаксического анализа
		5. ISyntaxConstructions - интерфейс для работы с синтаксическими конструкциями, позволяет создавать, удалять и работать с созданными конструкциями 
		6. ISyntaxConstructionGroups - интерфейс для работы с группами синтаксических конструкций, содержит набор методов по созданию, удалению и работе с группами конструкций 
		7. ICombinatoryAnalysis	- интерфейс для комбинаторного анализа 
		8. ICoSeDiLoad - интерфейс для загрузки словаря
		9. ICoSeDiManager - интерфейс, управляющий работой кобинаторно синтактико семантического словаря			 

4. ИСПОЛЬЗУЕМЫЕ ТЕХНИЧЕСКИЕ СРЕДСТВА
	Используется стандартная техническая база, на которую может быть установлена ОС Windows.\n\n

5. ВЫЗОВ И ЗАГРУЗКА
	5.1. Загрузка модуля осуществляется динамически через методы интерфейса CManagerConnections,
	который так же позволяет ибо через интерфейсы IConnection,  IManagerConnections, ISubConnection.\n\n
	5.2. Точек входа нет.\n\n

6. ВХОДНЫЕ ДАННЫЕ
	Нет.\n\n

7. ВЫХОДНЫЕ ДАННЫЕ
	Нет.\n\n

*/
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <atldbcli.h>

#include <algorithm>

#include "..\ASCInterface\defines.h"

USES_LOAD_MANAGER;
//using namespace std;

// TODO: reference additional headers your program requires here
