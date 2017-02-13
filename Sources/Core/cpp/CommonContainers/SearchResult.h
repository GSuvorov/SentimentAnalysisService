// SearchResult.h
#pragma once
#include "..\ASSInterface\ICommonContainersEx.h"
#include <list>
#include "SearchResultFile.h"
#include "ContainersFactory.h"

namespace SS
{
	namespace Core
	{
		namespace CommonContainers
		{

			using SS::Interface::Core::CommonContainers::ISearchResultEx;
			using SS::Interface::Core::CommonContainers::ITextBlock;
						
			///Результат поиска предназначен для обмена результатами поиска
			class CSearchResult : public ISearchResultEx
			{
			private:
				std::list<ITextBlock*> m_lpTextBlock;
				std::list<ITextBlock*>::iterator m_lIter;
				bool m_bSorted;
				bool m_bIsSortedDataActuality;
				std::list<ITextBlock*> m_lpTextBlockSort;
				std::list<ITextBlock*>::iterator m_lIterSort;
				XML::CSearchResultFile m_oXML;
				unsigned int m_uiRequestID;
				///Показатель глобальной версии
				bool m_bGlobal;
				///Фабрика контейнеров
				CContainersFactory* m_pContainersFactory;
				
			private:
				void Sort();

			public:
				virtual ULONG Release();
				virtual HRESULT QueryInterface(REFIID pIID, void** pBase);
				virtual void AddTextBlock(ITextBlock * pTextBlock);
				CSearchResult();
				virtual ~CSearchResult();
				///очистка хранилища (удаляет все текстовые блоки)
				virtual void Clear();						
				///добавление текстового блока в хранилище (добавит себе блок и вернет указатель на него)
				virtual ITextBlock *AddTextBlock();			
				///получение первого текстового блока коллекции (0 - если нет)
				virtual ITextBlock *GetFirstTextBlock();	
				///получение очередного текстового блока коллекции (не забывать вызвать получение первого текстового блока) (0 - если закончились)
				virtual ITextBlock *GetNextTextBlock();		
				///вернет результат поиска под определенным номером
				virtual ITextBlock* GetTextBlock(unsigned int nPosition);
				///вернет количество результатов поиска
				virtual unsigned int Count();				
				///установит режим обхода результатов 
				virtual void SetSortedMode(bool bOn);
				virtual bool GetXML(std::wstring* pXML);
				virtual bool GetXML(std::wstring* pXML, int iFirst, int iCount);
				virtual void SetRequestID(unsigned int id);
				virtual unsigned int GetRequestID();
				//Установить результат из xml (каждый вызов создает новый TextBlock)
				virtual void SetGsXML(const wchar_t* wzXml);
				void ChangeSortedDataActuality(bool bSortedDataActuality)
				{
					m_bSorted = bSortedDataActuality;
				}
				///Установить фабрику контейнеров
				void SetContainersFactory(CContainersFactory* pContainersFactory){ m_pContainersFactory = pContainersFactory; };
			};
		}
	}
}