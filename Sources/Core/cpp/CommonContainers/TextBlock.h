//TextBlock.h
#pragma once
#include "..\ASCInterface\ICommonContainers.h"
#include "..\ASCInterface\ITextFeature.h"
#include ".\SearchResult.h"
#include "ContainersFactory.h"
#include <list>

namespace SS
{
namespace Core
{
namespace CommonContainers
{
	using SS::Interface::Core::CommonContainers::ITextBlock;		
	using SS::Interface::Core::CommonContainers::ITextFeature;
	using SS::Interface::Core::CommonContainers::ITextBlockCut;

	///Текстовый блок который используется как при индексации, так и при поиске
	class CTextBlock : virtual public ITextBlock
	{
	private:
		///Для XML
		std::wstring m_ws;								
		///Релевантность
		unsigned int m_uiRelevance;						
		//Указатель на текстовую фичу
		ITextFeature* m_pTextFeature;					
		///Указатель на породивший SearchResult
		SS::Core::CommonContainers::CSearchResult* m_pSearchResult;

	protected:
		///Cписок указателей на ITextBlockCut
		std::list<ITextBlockCut*> m_lpTextBlockCut;		
		///Итератор данного списка
		std::list<ITextBlockCut*>::iterator m_Iter;
		///Фабрика контейнеров
		CContainersFactory* m_pContainersFactory;

	protected:
		///Конструктор
		CTextBlock();
		///Добавляет в коллекцию объект СTextBlockCut
		void AddCut(ITextBlockCut* pTextBlockCut);

	public:
		///Деструктор
		virtual ~CTextBlock();
		///Получение XML - строки содержимого контейнера
		virtual std::wstring* GetXML();
		///характеристики текста, которому принадлежит сей текстовый блок
		virtual ITextFeature* GetTextFeature();			
		///Очистка блока (удалит все вырезки из текста)
		virtual void Clear();							
		///Получение первой вырезки коллекции (0 - если нет)
		virtual ITextBlockCut* GetFirstBlockCut();		
		///Получение очередной вырезки коллекции (0 - если закончились)
		virtual ITextBlockCut* GetNextBlockCut();		
		///Получение релевантности данного текстового блока
		virtual unsigned int GetRelevance()				
		{
			return m_uiRelevance;
		}
		///Установление релевантности данного текстового блока
		virtual void SetRelevance(unsigned int uiRelevance)	
		{
			m_uiRelevance = uiRelevance;
			if(m_pSearchResult)
			{ m_pSearchResult->ChangeSortedDataActuality(false); }
		}
		void UpdateRelevance();
		///Устанавливает указатель SearchResult
		void SetSearchResult(SS::Core::CommonContainers::CSearchResult* pSearchResult)
		{
			m_pSearchResult = pSearchResult;
		}
		void Sort();
		///Установить фабрику контейнеров
		void SetContainersFactory(CContainersFactory* pContainersFactory){ m_pContainersFactory = pContainersFactory; };
	};
}
}
}