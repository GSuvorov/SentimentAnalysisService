#pragma once

#include "../ASSInterface/ITextBlockEx.h"
#include "./LinguisticWorkingInterfaces.h"
//#include "./LinguisticIndexation.h"

namespace SS
{
namespace LinguisticProcessor
{
namespace Indexation
{

class CTextIndexation : virtual public SS::LinguisticProcessor::CLinguisticWorkingInterfaces

{
public:
	CTextIndexation(void);
	~CTextIndexation(void);
public:
	/// инициирует процесс индексации очередного текстового блока очередного текста 
	void Init(
		/// контейнер с индексируемым текстом
		SS::Core::Types::IndexationSearch::TNativeTextForIndexation* pNativeText
		/// параметры индексации текста
		, SS::Core::Types::IndexationSearch::TIndexationParams* pIndexationParams
		/// стартовая позиция анализа текста
		, unsigned int uiFistCharPosition 
		);

	unsigned int GenerateNext(unsigned int& uiFistCharPosition, unsigned int& uiPosition, SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult);

	void Clear();
private:
	void Init();

};


}
}
}