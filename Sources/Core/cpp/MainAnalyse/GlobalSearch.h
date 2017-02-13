#pragma once

#include <list>

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

class CGlobalSearch : virtual public SS::LinguisticProcessor::CLinguisticWorkingInterfaces
{
	struct STextInfo
	{
		SS::Interface::Core::BlackBox::IText* pIText;
		std::wstring	wsPlaginName;
	};
	std::list< std::list<STextInfo*> >										m_l_l_ITextInfo;
	SS::Core::Types::IndexationSearch::EAnswerMode				m_AnswerMode;
	SS::Core::Types::IndexationSearch::TLinguisticProcessorMode	m_oMode;
	SS::Core::Types::IndexationSearch::TNativeTextForIndexation	m_oNativeText;
private:
	void ClearTexts();
	void UpdateRelevanceInTextCut(SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut, std::list<STextInfo*>& p_l_TextInfo);
	void RunRTFSearchOnlyKeyWords(SS::Interface::Core::BlackBox::IText* pText,const wchar_t *wsText, unsigned int uiBeginPosition);
	void RunRTFSearch(SS::Interface::Core::BlackBox::IText* pText,const wchar_t *wsText, unsigned int uiBeginPosition);

public:
	CGlobalSearch(void);
	~CGlobalSearch(void);
public:
	void SetLastQuery(SS::Interface::Core::BlackBox::Query::IQuery* pQuery);
	void UpdateRelevance(SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult, SS::Core::Types::IndexationSearch::EAnswerMode oAnswerMode);
	void SetMode(SS::Core::Types::IndexationSearch::TLinguisticProcessorMode* pMode)
	{
		m_oMode = *pMode;
	}

};

}
}
}