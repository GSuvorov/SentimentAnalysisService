#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

namespace SS
{
	namespace SyntaxConstructions
	{
		namespace Types
		{			
			///Описывает группу слов детектирования
			typedef std::set<std::wstring> TDetectWordsGroup;
			///Описывает тип кэша при работе с группами слов для детектирования
			typedef std::map<std::wstring, TDetectWordsGroup> TDetectWordsGroups;
			
			///Структура для доступа к таблице SyntaxConstructionGroupsList
			struct SSyntaxConstructionGroupName
			{
				INT32		m_ID;
				std::string m_sName;
				SSyntaxConstructionGroupName() : m_sName(""), m_ID(0) {}
			};
			
			///Структура для доступа к таблице DetectWordGroupName
			struct SDetectWordGroupName
			{
				std::wstring m_wsDetectWordGroupName;
				std::wstring m_wsDetectWord;
				SDetectWordGroupName() : m_wsDetectWord(L""), m_wsDetectWordGroupName(L"") {}
			};

			///Структура для работы с таблицей SyntaxConstructionGroups
			struct SGroupAndConstruction
			{
				INT32 m_i32IDConstruction;
				INT32 m_i32IDGroup;
				SGroupAndConstruction() : m_i32IDConstruction(0), m_i32IDGroup(0) {}
			};

			///Cтруктура для доступа к таблице SyntaxConstruction
			struct SSyntaxConstruction
			{
				INT32				m_i32ID;
				std::wstring		m_wsName;
				std::wstring		m_wsWord;
				std::vector<BYTE>	m_v_Data;
				INT32				m_i32DataSize;
				INT32				m_i32Level;
				SSyntaxConstruction() : m_i32ID(0), m_wsName(L""), m_wsWord(L""), m_i32DataSize(0), m_i32Level(0) {}

				bool operator<(const SSyntaxConstruction& rhs) const
				{
					return (this->m_i32Level < rhs.m_i32Level);
				} 
			};
			///Структура для доступа к таблице - (группа - тип разбора)
			struct SGroupAndCharacteristics
			{
				//Идентификатор группы
				int iIdGroup;
				//Режим разбора
				int iAnalyseMode;
				//Является ли группой разрешения омонимиии
				int iResolveAmbiguity;
				//Конструктор
				SGroupAndCharacteristics() : iIdGroup(-1), iAnalyseMode(-1), iResolveAmbiguity(-1) {}
			};

		}
	}
}