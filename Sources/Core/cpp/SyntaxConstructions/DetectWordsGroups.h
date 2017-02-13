#pragma once

#include "..\ASSInterface\ISyntaxConstructions.h"
#include ".\dboSyntax.h"
#include ".\connect.h"

namespace SS
{
	namespace SyntaxConstructions
	{
		namespace SQL
		{
			class CDetectWordsGroups : public SS::Interface::Core::SyntaxConstructions::IDetectWordsGroups,
									   public SS::CConnectSQL				
			{
			public:
				///Конструктор
				CDetectWordsGroups(wchar_t* szBaseName);
				///Деструктор
				~CDetectWordsGroups(){};
				///Создает группу слов для детектирования
				bool CreateDetectWordsGroup( SS::SyntaxConstructions::Types::TDetectWordsGroup& oDetectGroup);
				///Удаляет текущую группу слов для детектирования
				void DeleteDetectWordsGroup();
				///Возвращает все элементы группы слов для детектирования
				void GetWords(SS::SyntaxConstructions::Types::TDetectWordsGroup & oDetectGroup);
				///Удаляет текущий набор элементов из группы слов для детектирования и устанавливает новый
				void SetWords(SS::SyntaxConstructions::Types::TDetectWordsGroup & oDetectGroup);
				///Определяет входит ли слово в группу для детектирования
				bool IsDetectWord(std::string & sDetectWord);
				///Сохраняет в бинарный вид структуры данных 
				bool ToBinary();
			};
		}
		
		namespace DBMS
		{
			class CDetectWordsGroups : public SS::Interface::Core::SyntaxConstructions::IDetectWordsGroups,
									   public SS::CConnectDBMS
			{
			private:
				///Определят поле, по которому будет осуществляться поиск
				enum EFindMode
				{
					efmByDetectWord,
					efmByDetectWordGroupName
				};
				///Массив указателей на данные, являющиеся полями в DBMS таблице
				void* TableRecord[2];
				///Структура для доступа к таблице DetectWordGroupName
				SS::SyntaxConstructions::Types::SDetectWordGroupName m_oDetectWordGroupName;
				///Показатель выполнения инициализации таблиц
				bool m_TableInited;
			
			private:
				///Инициализация соединения с таблицой				
				void InitConnection();
			
			public:
				///Конструктор
				CDetectWordsGroups(wchar_t* szBaseName);
				///Деструктор
				virtual ~CDetectWordsGroups(){};
				///Cоздает группу слов для детектирования
				bool CreateDetectWordsGroup(SS::SyntaxConstructions::Types::TDetectWordsGroup & oDetectGroup);
				///Удаляет текущую группу слов для детектирования
				void DeleteDetectWordsGroup();
				///Возвращает все элементы группы слов для детектирования
				void GetWords(SS::SyntaxConstructions::Types::TDetectWordsGroup & oDetectGroup);
				///Удаляет текущий набор элементов из группы слов для детектирования и устанавливает новый
				void SetWords(SS::SyntaxConstructions::Types::TDetectWordsGroup & oDetectGroup);
				///Определяет входит ли слово в группу для детектирования
				bool IsDetectWord(std::string & sDetectWord);
				///Установить повторное выполнение инициализации таблиц
				void SetRepeatTableInit() { m_TableInited = false; };
			};
		}
	}
}