//IndexationResult.h
#pragma once
#include "..\ASCInterface\ICommonContainers.h"
#include "ContainersFactory.h"
#include <list>

namespace SS
{
	namespace Core
	{
		namespace CommonContainers
		{

			using SS::Interface::Core::CommonContainers::ITextBlock;
			using SS::Interface::Core::CommonContainers::IIndexationResult;
			
			///Является хранилищем текстовых блоков CTextBlockEx
			class CIndexationResult : public IIndexationResult
			{
			private:
				std::list<ITextBlock*> m_lpTextBlock;
				std::list<ITextBlock*>::iterator m_listIter;
				///Фабрика контейнеров
				CContainersFactory* m_pContainersFactory;
				
			public:
				//КОНСТРУКТОРЫ
				CIndexationResult();
				virtual ~CIndexationResult();
				///очистка хранилища (удаляет все текстовые блоки)
				virtual void Clear();						
				///добавление текстового блока в хранилище (добавит себе блок и вернет указатель на него)
				virtual ITextBlock *AddTextBlock();			
				///получение первого текстового блока коллекции (0 - если нет)
				virtual ITextBlock *GetFirstTextBlock();	
				///получение очередного текстового блока коллекции (не забывать вызвать получение первого текстового блока) (0 - если закончились)
				virtual ITextBlock *GetNextTextBlock();		
				//МЕТОДЫ IBase
				virtual ULONG Release();
				virtual HRESULT QueryInterface(REFIID pIID, void** pBase);	
				///Установить фабрику контейнеров
				void SetContainersFactory(CContainersFactory* pContainersFactory){ m_pContainersFactory = pContainersFactory; };
			};

		}
	}
}