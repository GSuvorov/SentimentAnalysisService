//ContainersFactory.h
#pragma once
#include "../ASCInterface/ICommonContainers.h"
#include "../ASSInterface/IBlackBox.h"

namespace SS
{
namespace Core
{
	using namespace SS::Interface::Core::CommonContainers;
	using namespace SS::Interface::Core::ResourceManagers;
	using namespace SS::Interface::Core::BlackBox;
	using namespace SS::Core::Types::IndexationSearch;
	///Фабрика классов для модуля
	class CContainersFactory
	{
	public:
		CContainersFactory(){};
		~CContainersFactory(){};
		///SearchResult
		ISearchResult* CreateSearchResult();
		///IndexationResult
		IIndexationResult* CreateIndexationResult();
		///TextBlock
		ITextBlock* CreateTextBlock(ILoadManager* pLoadManager);
		///TextBlockCut
		ITextBlockCut* CreateTextBlockCut( IBlackBoxTextStorage* pBlackBoxTextStorage,
										   TLinguisticProcessorMode* pLinguisticProcessorMode );
		///IndexAndPosition
		IIndexAndPosition* CreateIndexAndPosition( IBlackBoxTextStorage* pBlackBoxTextStorage,
												   TLinguisticProcessorMode* pLinguisticProcessorMode);
		///ServiceIndex
		IServiceIndex* CreateServiceIndex( IUnit* pUnit,
										   TLinguisticProcessorMode* pLinguisticProcessorMode,
										   IBlackBoxTextStorage* pBlackBoxTextStorage );
		///IndexationIndex
		IIndexationIndex* CreateIndexationIndex( IUnit* pUnit,
												 TLinguisticProcessorMode* pLinguisticProcessorMode,
												 IBlackBoxTextStorage* pBlackBoxTextStorage );
		///TextFeature
		ITextFeature* CreateTextFeature();
		///QueryResult
		IQueryResult* CreateQueryResult();
		///QueryIndex
		IQueryIndex* CreateQueryIndex();
	};
}
}