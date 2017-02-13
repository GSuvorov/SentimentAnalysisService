#pragma once

#include ".\index_storage.h"
#include "..\ASCInterface\ICommonContainers.h"
#include "..\ASDInterface\TNdxSearchEngineTypes.h"
#include ".\int_containers.h"
#include ".\res_containers.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Extracting
{

//--------------------------------------------------------------------//

///заполн€ет коллекцию результатов поиска
class CSearchResultFiller
{
public:
	typedef list<unsigned int> TResultsTextsNumbers;

	CSearchResultFiller(SS::Core::NdxSE::NdxProcessor::Containers::CFindResultCollection* pSearchResultCollection, 
		SS::Core::NdxSE::NdxProcessor::Storages::CIndexStorage* pIndexStorage, 
		TResultsTextsNumbers* pResultsTextsNumbers, unsigned int uiResultsIntervalRange, 
		SS::Interface::Core::NdxSE::CNdxSearchSettings::SSearchSettings::EStorageUsingMode StorageUsingMode);
	virtual ~CSearchResultFiller(void);

	///добавл€ет координату по буферу полной истории
	void AddCoordinate(unsigned int* puiBuffer, unsigned int uiBufferSize, float fRelevance);
	///запускает добавление координат дл€ данного результата, 
	void StartAddCoordinate(unsigned int* puiBuffer, unsigned int uiBufferSize);
	///добавл€ет координату по последней составл€ющей истории
	void AddCoordinate(unsigned int uiLastValue, float fRelevance);
	///добавл€ет координату по последней составл€ющей истории, с коллекцией интервалов
	bool AddCoordinate(unsigned int uiLastValue, float fRelevance, 
		SS::Core::NdxSE::NdxProcessor::Containers::TIntervalCollection* pIntervalCollection);
	///добавл€ет координату по последней составл€ющей истории, с коллекцией идентификаторов блоков
	bool AddCoordinate(unsigned int uiLastValue, float fRelevance, 
		SS::Core::NdxSE::NdxProcessor::Containers::CFindResult::TBlockHIDCollection* pBlockHIDCollection);
	
	///возвращает линейную коллекцию результатов
	SS::Core::NdxSE::NdxProcessor::Containers::CFindResultCollection* GetCurrSearchResultCollection(void){return m_pCurrSearchResultCollection;};
	///устанавливает линейную коллекцию результатов
	void SetCurrSearchResultCollection(SS::Core::NdxSE::NdxProcessor::Containers::CFindResultCollection* pSearchResult){m_pCurrSearchResultCollection=pSearchResult;};
	///возвращает информацию по структуре уровней
	Storages::CIndexStorage* GetIndexStorage(void){return m_pIndexStorage;};
	///возвращает коллекци€ номеров текстов результатов 
	TResultsTextsNumbers* GetResultsTextsNumbers(void){return m_pResultsTextsNumbers;};
	///отображает хранилище результатов поиска
	static void ViewSearchResult(SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult, set<unsigned int>* pQueryIndexes = NULL);
	///отображает блоккат
	static void ViewTextBlockCut(
		SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut, 
		bool bViewWords, bool bViewIndexes);

protected:
	///коллекци€ результатов поиска
	SS::Core::NdxSE::NdxProcessor::Containers::CFindResultCollection* m_pCurrSearchResultCollection;
	///хранилище индексов и информацией по структуре делени€ координат
	SS::Core::NdxSE::NdxProcessor::Storages::CIndexStorage* m_pIndexStorage;

	///коллекци€ номеров текстов результатов 
	///(формируетс€ по мере конвертации результатов за всю сессию поиска включа€ поиск далее)
	///добавление номеров происходит поступательно в пор€дке возрастани€
	TResultsTextsNumbers* m_pResultsTextsNumbers;
	///размер интервала, в котором будет извлекатьс€ результат
	unsigned int m_uiResultsIntervalRange;

	///текущий номер текста в коллекции
	unsigned int m_uiCurrTextNumber;
	unsigned int m_uiCurrTextBlockCutNumber;
	
	///текущий режим использовани€ хранилищ при поиске
	SS::Interface::Core::NdxSE::CNdxSearchSettings::SSearchSettings::EStorageUsingMode m_StorageUsingMode;
};

//--------------------------------------------------------------------//

}
}
}
}
}