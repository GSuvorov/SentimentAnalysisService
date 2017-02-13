//SyntaxConstuctions.h
#pragma once

#include "..\ASSInterface\ISyntaxConstructions.h"
#include ".\dboSyntax.h"
#include ".\connect.h"
#include "..\ASSInterface\construction.h"

namespace SS
{
	namespace SyntaxConstructions
	{
		///Пространство имен SQL
		namespace SQL
		{
			///Класс, реализующий работу с синтаксическими конструкциями
			class CSyntaxConstuctions : public SS::Interface::Core::SyntaxConstructions::ISyntaxConstructions,
										public SS::CConnectSQL
			{
			private:
				//Группа детект-слов
				SS::Interface::Core::SyntaxConstructions::IDetectWordsGroups* m_pDetectWordsGroups;
				///Возвращает максимальный используемый индекс
				long GetMaxId();
		
			public:
				///Конструктор
				CSyntaxConstuctions(wchar_t* szBaseName,
									SS::Interface::Core::SyntaxConstructions::IDetectWordsGroups* pDetectWordsGroups);
				///Деструктор
				virtual ~CSyntaxConstuctions(){};
				///Методы ISyntaxConstructions
				///Добавляет в базу конструкцию и возвращает ее номер
				long Add(/*in*/SS::SyntaxConstructions::Types::CConstructionEx* p_SyntaxConstruction);
				///Удаляет текущую конструкцию
				void Delete();
				///Возвращает список всех синтаксических конструкций
				bool Get(/*out*/std::list<SS::SyntaxConstructions::Types::CConstructionEx>* p_list_OfSyntaxConstruction);
				///Возвращает текущую синтаксическую конструкцию
				bool Get(/*out*/SS::SyntaxConstructions::Types::CConstructionEx* p_SyntaxConstruction);
				///Методы ILoad
				///Сохраняет в бинарный вид структуры данных 
				bool ToBinary();
			};
		}
		
		///Пространство имен DBMS
		namespace DBMS
		{
			///Класс, реализующий работу с синтаксическими конструкциями
			class CSyntaxConstuctions :	public SS::Interface::Core::SyntaxConstructions::ISyntaxConstructions,
										public SS::CConnectDBMS
			{
			private:
				///Определят поле, по которому будет осуществляться поиск
				enum EFindMode
				{
					efmByID,
					efmByName
				};
				///Данные о синтаксической конструкции
				SS::SyntaxConstructions::Types::SSyntaxConstruction m_oSSyntaxConstruction;		
				///Массив указателей на данные, являющиеся полями в DBMS таблице
				void* TableRecord[6];
				///Текущая детект-группа
				SS::Interface::Core::SyntaxConstructions::IDetectWordsGroups* m_pDetectWordsGroups;
				//Показатель выполнения инициализации таблиц
				bool m_TableInited;
			
			private:
				///Инициализация соединений с таблицами
				void InitConnection();
		
			public:
				///Конструктор
				CSyntaxConstuctions( wchar_t* szBaseName, 
									 SS::Interface::Core::SyntaxConstructions::IDetectWordsGroups* pDetectWordsGroups );
				///Деструктор
				virtual ~CSyntaxConstuctions(){};
				/// Методы ISyntaxConstructions
				/// добавляет в базу конструкцию и возвращает ее номер
				long Add(SS::SyntaxConstructions::Types::CConstructionEx* pSyntaxConstruction);
				/// удаляет текущую конструкцию
				void Delete();
				/// возвращает список всех синтаксических конструкций
				bool Get(std::list<SS::SyntaxConstructions::Types::CConstructionEx>* p_list_OfSyntaxConstruction);
				/// возвращает текущую синтаксическую конструкцию
				bool Get(SS::SyntaxConstructions::Types::CConstructionEx* p_SyntaxConstruction);
				///Установить повторное выполнение инициализации таблиц
				void SetRepeatTableInit() { m_TableInited = false; };
			};
		}
	}
}