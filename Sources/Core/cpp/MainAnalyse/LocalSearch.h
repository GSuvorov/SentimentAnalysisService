#pragma once

#include "../ASCInterface/ILinguisticProcessor.h"
#include "../ASCInterface/ICommonContainers.h"

#include "../ASSInterface/ITextBlockEx.h"
#include "../ASSInterface/Constants.h"

#include "./LinguisticWorkingInterfaces.h"

#define ANAFORA

namespace SS
{
namespace LinguisticProcessor
{
namespace Search
{
using namespace SS::Interface::Core::CommonContainers;

class CLocalSearch : virtual public SS::LinguisticProcessor::CLinguisticWorkingInterfaces
{
	SS::Core::Types::IndexationSearch::EAnswerMode		m_AnswerMode;
	std::list<SS::Interface::Core::BlackBox::IText*>		m_l_IText;
private:
	void ClearTexts();
public:
	CLocalSearch(void);
	~CLocalSearch(void);
public:
	void SetLastQuery(SS::Interface::Core::BlackBox::Query::IQuery* pQuery);
	void UpdateRelevance(ISearchResult* pSearchResult);

};

}
}
}