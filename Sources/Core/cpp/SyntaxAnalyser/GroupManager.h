#pragma once

#include "ExpertGroup.h"
#include "ParsingType.h"
#include "../ASSInterface/Construction.h"
#include "../ASCInterface/IInterface.h"
#include "../ASSInterface/ICommon.h"
#include "ExpertSemantic.h"

#include <vector>

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{
			///Управляет загрузкой и хранением групп экспертов.
			class CGroupManager : public SS::Interface::Core::Common::ILoad
			{
			public:
				///Тип - Хранилище указателей на группу экспертов
				typedef std::vector<CExpertGroup*>				TCollection;
				///Тип - Итератор хранилища
				typedef std::vector<CExpertGroup*>::iterator	TCollectionItr;

			private:
				///Загружены ли группы
				bool m_bGroupsLoaded;
				///Менеджер загрузки
				SS::Interface::Core::ResourceManagers::ILoadManager* m_pLoadManager;
				///Отдельный эксперт для разбора запроса. Выполняется отдельно, один раз, в самом конце разбора
				CBaseExpert* m_pExpertResumeQuery;
				///Текущая коллекция
				TCollection* m_pCurrentCollection;
				///Маркер текущей группы
				TCollectionItr m_itCurrentGroup;
				///Коллекция для инициализации итераторов
				TCollection m_NullCollection;
				
				///список групп экспертов для разбора английского языка
				TCollection m_vAllGroupsEng;
				///список групп экспертов для разбора русского языка
				TCollection m_vAllGroupsRus;
				///Группы разбора запроса (английский язык)
				TCollection m_vQueryGroupsEng;
				///Группа для полного разбора (английский язык)
				TCollection m_vIndexationGroupsEng;
				///Группы разбора запроса (русский язык)
				TCollection m_vQueryGroupsRus;
				///Группа для полного разбора (русский язык)
				TCollection m_vIndexationGroupsRus;

				///Группы разрешения омонимии для индексации (английский язык)
				TCollection m_vIndexationAmbGroupsEng;
				///Группы разрешения омонимии индексации (русский язык)
				TCollection m_vIndexationAmbGroupsRus;
				
				std::map<ParsingType, TCollection*> m_mapCollections;

				///Тип разбоа (индексация, разбор запроса, разрешение омонимиии)
				ParsingType	m_ParsingType;
				///Критическая секция
				SS::Core::CommonServices::CManagerLifeTimeForCriticalSection m_oManagerLifeTimeForCriticalSection;
				///Параметры анализа
				SS::Interface::Core::MainAnalyse::IAnalyseParams* m_pAnalyseParams;
				///Тип синтаксических баз
				ESyntaxBase m_eBase;
				///Отдельный эксперт для работы с семантикой
				SS::Syntax::ExpertSemantic::CExpertSemantic m_oExpertSemantic;

			private:
				///Загрузка групп для конкретного языка
				void LoadGroupsForLanguage( 
								SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionsManager* pSyntaxConstructionsManager,
								TCollection & rAllGroups,
								TCollection & rAmbiguityGroups,
								TCollection & rQueryGroups,
								TCollection & rIndexationGroups );
				///Один полный цикл динамического разбора
				bool DynamicAnalyse(bool bFragm, BoardElements::CBoard* pBoard);
				///МЕТОДЫ ILoad
				/// сохраняет в бинарный вид структуры данных
				bool ToBinary(){ return false; }
				///Выгрузить конструкции
				bool Clear();

			public:
				///Конструктор
				CGroupManager();
				///Деструктор
				~CGroupManager();
				///Установить LoadManager
				void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
				///Получение указателя на первую группу
				CExpertGroup* GetFirstGroup();
				///Получение указателя на следующую группу (вернет NULL если групп больше нет)
				CExpertGroup* GetNextGroup();
				///Запускает синтаксический разбор предложения с динамическим созданием гипотез
				void ParseSentenceDynamic(BoardElements::CBoard* pBoard, ParsingType pType);
				///Установка типа разбора
				void SetParsingType(ParsingType pType)
				{
					m_ParsingType = pType;
				}
				///МЕТОДЫ ДЛЯ ТЕСТЕРА
				///Возвращает итератор на первую группу (методы нужен только для тестера)
				TCollectionItr GetBeginGroup();
				///Возвращает итератор на группу за последней (методы нужен только для тестера)
				TCollectionItr GetEndGroup();
				///МЕТОДЫ ILoad
				///Загрузить конструкции
				bool Load();
				///Перезагрузитьт конструкции
				bool ReLoad();
				///Установить AnalyseParams
				void SetAnalyseParams(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams);
				///Установить тип синтаксических баз
				void SetBase(ESyntaxBase eBase) { m_eBase = eBase; };
			};

		}
	}
}