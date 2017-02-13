#pragma once

#include "./QueryResult.h"
#include "..\ASSInterface\IQueryResultEx.h"
#include "../ASSInterface/IBlackBoxQuery.h"
#include "../ASSInterface/IDictionaryManager.h"

namespace SS
{
	namespace Core
	{
		namespace CommonContainers
		{
			using SS::Interface::Core::CommonContainers::IQueryResultEx;

			using SS::Interface::Core::BlackBox::Query::IQuery;
			using SS::Interface::Core::BlackBox::Query::ITransformation;
			using SS::Interface::Core::BlackBox::Query::IGroup;
			using SS::Interface::Core::BlackBox::Query::IQueryUnit;
			using SS::Interface::Core::BlackBox::Query::IQueryUnitIndexCollection;
			using SS::Interface::Core::BlackBox::Query::IQueryUnitIndex;

			using SS::Core::Types::IndexationSearch::TQueryParams;
			using SS::Core::Types::IndexationSearch::TLinguisticProcessorMode;

			/// реализует функциональность IQueryResult + его заполнение из IQuery
			class CQueryResultEx : public IQueryResultEx, public CQueryResult
			{
			private:
				IQuery*			m_pQuery;
				unsigned int	m_uiQueryUnitID;
				bool			m_IsSynonimCollection;
				bool			m_IsMorphoCollection;
				///Показывает что  втекущей группе только один юнит
				bool			m_bOneUnit;
				//Структура для работы с SequenceID
				struct SDataID
				{
					unsigned int uiRealSequenceID;
					unsigned int uiVirtualSequenceID;
					unsigned int uiTransformationID;
					bool		 bChangeRealSequenceID; 
					bool		 bChangeTransformID;
					//Значение получается из IQueryUnit-а
					bool		 IsObligatory;
					//Количество групп для каждой трансформации
					unsigned int uiGroupCount;
					//Вес слова запроса
					float fUnitWeight;
					//Обнуление структуры ID
					void DataIdToZero();
					//Расчёт SequenceID
					unsigned int GetSequenceID();

					void Init() 
					{
						uiRealSequenceID = 0;
						uiVirtualSequenceID = 0;
						uiTransformationID = 0;
						bChangeRealSequenceID = false;
						bChangeTransformID = false;
						IsObligatory = false;
						uiGroupCount = 0;
						fUnitWeight = 0.0;		
					};
				} m_oDataID;
				///Словарь(для тестового метода)
				SS::Interface::Core::Dictionary::IDictionary* m_pDictionary;
				///Слово запроса из QueryUnit-а
				std::wstring m_strWord;
				
				//Собственные методы
				void ApplyCollections(IQueryUnitIndexCollection* pQueryUnitIndexCollection, IQueryIndex** ppqiFirst, IQueryIndex** ppqiLast);
				void ApplyUnits(IQueryUnit* pQueryUnit, IQueryIndex** ppqiFirst, IQueryIndex** ppqiLast);
				void ApplyGroups(IGroup* pGroup, IQueryIndex** ppqiFirst, IQueryIndex** ppqiLast);
				void ApplyTransform(ITransformation* pTransformation, IQueryIndex** ppqiFirst, IQueryIndex** ppqiLast);
				//параметры
				TQueryParams				m_QueryParams;
				TLinguisticProcessorMode	m_LinguisticProcessorMode;
				//собственные методы
				bool ApplyLinguisticProcessorMode(IQueryUnitIndexCollection* pCollection);
				///Определяет наличие коллекции синоним-индексов в Unit - е
				void SearchSynonimCollection(IQueryUnit* pQueryUnit);
				///Определяет наличие морфо-коллекции в Unit-е
				void SearchMorphoCollection(IQueryUnit* pQueryUnit);
				///Вывод данных(тестовый метод)
				void Show();
				///Вывод слов запроса(тестовый метод)
				void ShowWords();
				///Определение слова запроса
				void FindQueryWord(IQueryUnit* pQueryUnit);
				///Подсчет количества Unit-ов в одной группе
				void ControlCountUnitInGroup(IGroup* pGroup);
				
			public:
				//КОНСТРУКТОРЫ
				CQueryResultEx();
				virtual ~CQueryResultEx();
				//МЕТОДЫ IQueryResultEx
				///преобразует запрос (IQuery) в список CQueryIndex - ов
				virtual void SetIQuery(IQuery* pQuery);
				///возврат IQuery установленного с помошью SetIQuery
				virtual IQuery* GetIQuery();
				///установление параметров запроса
				virtual void SetQueryParams(TQueryParams* pQueryParams);
				///установление режима лингвистического процессора
				virtual void SetLinguisticProcessorMode(TLinguisticProcessorMode* pLinguisticProcessorMode);
				//ПЕРЕОПРЕДЕЛЯЮ МЕТОД IBase
				virtual HRESULT QueryInterface(REFIID pIID, void** pBase);	//приведение к интерфейсу с поданным REFIID. 
																			//pBase будет NULL, если данный интерфейс не поддерживается
				///ПЕРЕОПРЕДЕЛЯЮ МЕТОД ИЗ CBaseCoreClass
				virtual void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
			};

		}
	}
}