//TextBlockCut.h
#pragma once
#include "..\ASCInterface\ICommonContainers.h"
#include "..\ASSInterface\IBlackBox.h"
#include "ContainersFactory.h"
#include <list>

namespace SS
{
	namespace Core
	{
		namespace CommonContainers
		{
			using SS::Interface::Core::CommonContainers::IIndexAndPosition;
			using SS::Interface::Core::CommonContainers::ITextBlockCut;
			using SS::Interface::Core::BlackBox::IBlackBoxTextStorage;
			using SS::Core::Types::IndexationSearch::TLinguisticProcessorMode;

			///"Вырезка" из тескстового блока, получаемая при доставании результатов поиска. Для одного блока может быть несколько вырезок
			/**
			содержит индексы на позиции, идущие подряд в тексте
			*/
			class CTextBlockCut : public ITextBlockCut
			{
			private:
				std::wstring m_wsIndexTypeName;
				std::wstring m_ws;
				wchar_t* m_wszTextCut;
				wchar_t* m_wszPluginName;
				std::list<IIndexAndPosition*> m_list_pIndexAndPosition;	
				std::list<IIndexAndPosition*>::iterator m_listIter;
				unsigned int m_uiRelevance;
				IBlackBoxTextStorage*		m_pBlackBoxTextStorage;
				TLinguisticProcessorMode*	m_pLinguisticProcessorMode;

				///Вспомогательные данные
				struct SBlockCutData
				{
					unsigned int
					TextCutType : 8,
					Level : 8,
					Number : 8;
				} m_oBlockCutData;
				///Фабрика контейнеров
				CContainersFactory* m_pContainersFactory;

			public:
				//КОНСТРУКТОРЫ
				CTextBlockCut(IBlackBoxTextStorage* pBlackBoxTextStorage, TLinguisticProcessorMode*	pLinguisticProcessorMode);
				virtual ~CTextBlockCut();
				//МЕТОДЫ ITextBlockCut
				///очистка вырезки (удалит все индексы)
				virtual void Clear();									
				///добавление индекса в коллекцию (добавит в себя и вернет указатель)
				virtual IIndexAndPosition* AddIndexAndPosition();		
				///получение первого индекса коллекции (0 - если нет)
				virtual IIndexAndPosition* GetFirstIndexAndPosition();	
				///получение очередного индекса коллекции (не забывать вызвать получение первого индекса) (0 - если закончились)
				virtual IIndexAndPosition* GetNextIndexAndPosition();
				/// присвоение текстового куска в виде строки (используется плагинами GlobalSearch)
				virtual void SetTextCut(const wchar_t* wszText);
				///присвоение "имени" типа индекса данной текстовой части
				void SetIndexTypeName(const wchar_t* wszName);
				///получение "имени" типа индекса данной текстовой части
				const wchar_t* GetIndexTypeName();
				/// получение релевантности данной текстовой части
				virtual unsigned int GetRelevance()
				{
					return m_uiRelevance;
				}
				/// установление релевантности данной текстовой части
				virtual void SetRelevance(unsigned int uiRelevance)
				{
					m_uiRelevance = uiRelevance;
				}
				/// получение текстового куска в виде строки
				virtual const wchar_t* GetTextCut();
				/// установление "имени" плагина
				virtual void SetPluginName(const wchar_t* wsName);
				/// получение "имени" плагина
				virtual const wchar_t* GetPluginName() const;
				/// возвращает тип текст блок 
				virtual SS::Core::Types::IndexationSearch::ETextCutType GetType();
				/// устанавливает тип текст блока
				virtual void SetType(SS::Core::Types::IndexationSearch::ETextCutType eTextCutType);
				/// возвращает уровень заголовка
				virtual unsigned int GetLevel();
				/// устанавливает уровень заголовка
				virtual void SetLevel(unsigned int uiLevel);
				/// возвращает номер группы текст блок катов
				virtual unsigned int GetNumber();
				/// устанавливает номер группы текст блок катов
				virtual void SetNumber(unsigned int uiNumber);
				//ДРУГИЕ
				/// получение XML - строки содержимого контейнера
				std::wstring* GetXML();
				///Установить фабрику контейнеров
				void SetContainersFactory(CContainersFactory* pContainersFactory){ m_pContainersFactory = pContainersFactory; };
			};


		}
	}
}