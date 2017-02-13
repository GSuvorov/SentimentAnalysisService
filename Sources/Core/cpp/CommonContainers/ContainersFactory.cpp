///ContainersFactory.cpp
#include "StdAfx.h"
#include "containersfactory.h"

#include "SearchResult.h"
#include "IndexationResult.h"
#include "TextBlockEx.h"
#include "TextBlockCut.h"
#include "IndexAndPosition.h"
#include "ServiceIndex.h"
#include "IndexationIndex.h"
#include "TextFeature.h"
#include "QueryResultEx.h"
#include "QueryIndex.h"

namespace SS
{
namespace Core
{
	using namespace SS::Core::CommonContainers;
		
	///SearchResult
	ISearchResult* CContainersFactory::CreateSearchResult()
	{
		CSearchResult* pSearchResult = new CSearchResult;
		pSearchResult->SetContainersFactory(this);
		return pSearchResult;
	}
	
	///IndexationResult
	IIndexationResult* CContainersFactory::CreateIndexationResult()
	{
		CIndexationResult* pIndexationResult = new CIndexationResult;
		pIndexationResult->SetContainersFactory(this);
		return pIndexationResult;
	}
	
	///TextBlock
	ITextBlock* CContainersFactory::CreateTextBlock(ILoadManager* pLoadManager)
	{
		CTextBlockEx* pTextBlockEx = new CTextBlockEx(pLoadManager);
		pTextBlockEx->SetContainersFactory(this);
		return pTextBlockEx;
	}
	
	///TextBlockCut
	ITextBlockCut* CContainersFactory::CreateTextBlockCut( IBlackBoxTextStorage* pBlackBoxTextStorage,
														   TLinguisticProcessorMode* pLinguisticProcessorMode )
	{
		CTextBlockCut* pTextBlockCut = new CTextBlockCut(pBlackBoxTextStorage, pLinguisticProcessorMode);
		pTextBlockCut->SetContainersFactory(this);
		return pTextBlockCut;
	}
	
	///IndexAndPosition
	IIndexAndPosition* CContainersFactory::CreateIndexAndPosition( IBlackBoxTextStorage* pBlackBoxTextStorage,
																   TLinguisticProcessorMode* pLinguisticProcessorMode )
	{
		CIndexAndPosition* pIndexAndPosition = new CIndexAndPosition(pBlackBoxTextStorage, pLinguisticProcessorMode);
		pIndexAndPosition->SetContainersFactory(this);
		return pIndexAndPosition;
	}
	
	///ServiceIndex
	IServiceIndex* CContainersFactory::CreateServiceIndex( IUnit* pUnit,
														   TLinguisticProcessorMode* pLinguisticProcessorMode,
														   IBlackBoxTextStorage* pBlackBoxTextStorage )
	{
		return new CServiceIndex(pUnit, pLinguisticProcessorMode, pBlackBoxTextStorage);
	}
	
	///IndexationIndex
	IIndexationIndex* CContainersFactory::CreateIndexationIndex( IUnit* pUnit,
																 TLinguisticProcessorMode* pLinguisticProcessorMode,
																 IBlackBoxTextStorage* pBlackBoxTextStorage )
	{
		return new CIndexationIndex(pUnit, pLinguisticProcessorMode, pBlackBoxTextStorage);
	}

	///TextFeature
	ITextFeature* CContainersFactory::CreateTextFeature()
	{
		return new CTextFeature;
	}

	///QueryResult
	IQueryResult* CContainersFactory::CreateQueryResult()
	{
		CQueryResultEx* pQueryResultEx = new CQueryResultEx;
		pQueryResultEx->SetContainersFactory(this);
		return pQueryResultEx;
	}
	
	///QueryIndex
	IQueryIndex* CContainersFactory::CreateQueryIndex()
	{
		return new CQueryIndex;
	}
}
}




