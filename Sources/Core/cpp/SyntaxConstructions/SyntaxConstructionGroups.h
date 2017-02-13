#pragma once

#include "..\ASSInterface\ISyntaxConstructions.h"
#include ".\dboConstruction.h"
#include ".\connect.h"
#include "..\ASSInterface\construction.h"

namespace SS
{
	namespace SyntaxConstructions
	{
		namespace SQL
		{
			using SS::SyntaxConstructions::Types::SGroupAndCharacteristics;

			/// класс, реализующий работу с группами конструкций
			class CSyntaxConstructionGroups : public SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionGroups,
											  public SS::CConnectSQL
			{
			private:
				///Сохраняет в бинарный вид структуры данных 
				void ToBinaryGroupsType();

			public:
				///Конструктор
				CSyntaxConstructionGroups(wchar_t* szBaseName) : SS::CConnectSQL(szBaseName){};
				///Деструктор
				virtual ~CSyntaxConstructionGroups(){};
				///Сохраняет в бинарный вид структуры данных 
				bool ToBinary();
				///МЕТОДЫ ISyntaxConstructionGroups
				///Добавляем текущую пару (m_iConstructionId, m_ConstructionGroupId) в базу
				bool Add();	
				/// возвраращает все пары (m_iConstructionId, m_ConstructionGroupId), хранимые в базе
				bool Get(std::list<SS::SyntaxConstructions::Types::SGroupAndConstruction> & listConstructionGroups);
				///	удаляет текущую пару из базы
				bool Delete();
				///Возвращает список структур содержащих группу для типа разбора и принадлежность к гр. разр. омонимии
				void GetGroupsForType( SS::Core::Types::EAnalyseMods eAnalyseMode,
									   std::list<SGroupAndCharacteristics> & lstGroupCharacteristics);
			};
		
		}
		
		namespace DBMS
		{
			using SS::SyntaxConstructions::Types::SGroupAndCharacteristics;
			
			///Класс, реализующий работу с группами конструкций
			class CSyntaxConstructionGroups : public SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionGroups,
											  public SS::CConnectDBMS
			{
			private:
				///Структура для доступа к таблице SyntaxConstructionGroups
				SS::SyntaxConstructions::Types::SGroupAndConstruction	m_oGroupAndConstruction;
				///Структура для доступа к таблице SyntaxConstructionGroupsType
				SS::SyntaxConstructions::Types::SGroupAndCharacteristics	m_oGroupAndCharacteristics;
				///Запись из таблицы SyntaxConstructionGroups
				void* m_GroupTableRecord[2];
				///Запись из таблицы SyntaxConstructionGroupsType
				void* m_GroupTypeTableRecord[3];
				///Показатель выполнения инициализации таблиц
				bool m_TableInited;
			
			private:
				///Метод для начальной инициализации необходимых таблиц
				void InitTables();
			
			public:
				///Конструктор
				CSyntaxConstructionGroups(wchar_t* szBaseName);
				///Деструктор
				virtual ~CSyntaxConstructionGroups(){};
				///МЕТОДЫ ISyntaxConstructionGroups								
				///Добавляем текущую пару (m_iConstructionId, m_ConstructionGroupId) в базу
				bool Add();	
				///Возвраращает все пары (m_iConstructionId, m_ConstructionGroupId), хранимые в базе
				bool Get(std::list<SS::SyntaxConstructions::Types::SGroupAndConstruction> & listConstructionGroups);					
				///Удаляет текущую пару из базы
				bool Delete();
				///Возвращает список структур содержащих группу для типа разбора и принадлежность к гр. разр. омонимии
				void GetGroupsForType( SS::Core::Types::EAnalyseMods eAnalyseMode,
									   std::list<SGroupAndCharacteristics> & lstGroupCharacteristics );
				///Установить повторное выполнение инициализации таблиц
				void SetRepeatTableInit() { m_TableInited = false; };
			};
		
		}
	}
}