//QueryResult.h
#pragma once
#include "..\ASCInterface\ICommonContainers.h"
#include <list>
#include "ContainersFactory.h"

namespace SS
{
	namespace Core
	{
		namespace CommonContainers
		{
			using SS::Interface::Core::CommonContainers::IQueryResult;
			using SS::Interface::Core::CommonContainers::IQueryIndex;
			
			///Индексный запрос предназначен для обмена результатами анализа и формирования индексов для поиска
			class CQueryResult : virtual public IQueryResult
			{
			private:
				std::list<IQueryIndex*> m_lpQueryIndex;
				std::list<IQueryIndex*>::iterator m_Iter;
				SS::Core::Types::IndexationSearch::TAbsentWords m_oAbsentWords;
				///Фабрика контейнеров
				CContainersFactory* m_pContainersFactory;

			public:
				//КОНСТРУКТОРЫ
				CQueryResult();
				virtual ~CQueryResult();
				//ДРУГИЕ МЕТОДЫ
				///очистка хранилища (удаляет все индексы)
				virtual void Clear();						
				///добавление индекса в хранилище (добавит себе индекс и вернет указатель на него)
				virtual IQueryIndex* AddQueryIndex();		
				///получение первого индекса коллекции (0 - если нет)
				virtual IQueryIndex* GetFirstQueryIndex();	
				///получение очередного индекса коллекции (не забывать вызвать получение первого индекса) (0 - если закончились)
				virtual IQueryIndex* GetNextQueryIndex();
				/// вернет список отсутствующих слов
				virtual SS::Core::Types::IndexationSearch::TAbsentWords* GetAbsentWords();
				//МЕТОДЫ IBase
				virtual ULONG Release();
				virtual HRESULT QueryInterface(REFIID pIID, void** pBase);	
				///Установить фабрику контейнеров
				void SetContainersFactory(CContainersFactory* pContainersFactory){ m_pContainersFactory = pContainersFactory; };
			};

		}
	}
}