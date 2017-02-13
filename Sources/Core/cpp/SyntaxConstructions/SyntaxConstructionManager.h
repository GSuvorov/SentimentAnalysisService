//SyntaxConstructionManager.h
#pragma once

#include "..\ASSInterface\ISyntaxConstructions.h"
#include ".\CSyntaxConstructions.h"
#include ".\SyntaxConstructionGroups.h"
#include ".\SyntaxConstructionGroupsNames.h"
#include ".\DetectWordsGroups.h"

namespace SS
{
	namespace SyntaxConstructions
	{
		///Тип базы данных
		enum EDataBaseType
		{
			edbtUndefined,
         edbtDBMS,
			edbtSQL
		} ;

		///Класс управляющий работой с синтаксическими конструкциями
		class CSyntaxConstructionManager : public SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionsManager
		{
		private:
			///Указатель на объект класса SQL::CSyntaxConstuctions
			SS::SyntaxConstructions::SQL::CSyntaxConstuctions*		m_pSyntaxConstuctionsSQL;
			///Указатель на объект класса DBMS::CSyntaxConstuctions
			SS::SyntaxConstructions::DBMS::CSyntaxConstuctions*		m_pSyntaxConstuctionsDBMS;
			///Указатель на объект класса SQL::CSyntaxConstructionGroups
			SS::SyntaxConstructions::SQL::CSyntaxConstructionGroups*	m_pSyntaxConstructionGroupsSQL;
			///Указатель на объект класса DBMS::CSyntaxConstructionGroups
			SS::SyntaxConstructions::DBMS::CSyntaxConstructionGroups* m_pSyntaxConstructionGroupsDBMS;
			///Указатель на объект класса SQL::CDetectWordsGroups
			SS::SyntaxConstructions::SQL::CDetectWordsGroups*		m_pDetectWordsGroupsSQL;
			///Указатель на объект класса DBMS::CDetectWordsGroups
			SS::SyntaxConstructions::DBMS::CDetectWordsGroups*		m_pDetectWordsGroupsDBMS;
			///Имя базы
			wchar_t m_szBaseName[100];
			///Тип базы
			EDataBaseType m_eDataBaseType;
		
		private:
			///Создание объктов
			void CreateObjects();
		
		public:
			///Конструктор
			CSyntaxConstructionManager(const wchar_t* szBaseName);
			///Деструктор
			~CSyntaxConstructionManager();
			///Возвращает интерфес для работы с синтаксическими конструкциями	
			SS::Interface::Core::SyntaxConstructions::ISyntaxConstructions			* GetSyntaxConstructions();
			///Возвращает интерфес для работы с группами синтаксических конструкций	
			SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionGroups		* GetSyntaxConstructionGroups();
			///Возвращает интерфес для работы с группами синтаксических конструкций и их названиями	
			SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionGroupsNames	* GetSyntaxConstructionGroupsNames();
			///Возвращает интерфес для работы с группами слов для детектирования
			SS::Interface::Core::SyntaxConstructions::IDetectWordsGroups				* GetDetectWordsGroups();
			///Методы IBase 
			///Освобождение
			ULONG Release();
			///Приведение к интерфейсу
			HRESULT QueryInterface(REFIID pIID, void** pBase); 		
			///Методы ILoad
			///Сохраняет в бинарный вид структуры данных 
			bool ToBinary();
			///Перезагружает словарь
			bool ReLoad() { return false; };
			///Загружает словарь
			bool Load() { return false; };
			///Освобождает память
			bool Clear() { return false; };
		};
	
	}
}
