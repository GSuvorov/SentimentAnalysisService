#include "StdAfx.h"
#include "SyntaxFeature.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CSyntaxFeature::CSyntaxFeature() : 
		m_bQuestion(false),
		m_uiSyntaxCategory(0)
{
}

CSyntaxFeature::~CSyntaxFeature()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// ISyntaxFeature interface

unsigned int CSyntaxFeature::GetMemorySize()
{
	unsigned int i = 4*4;// !!!! неточный размер синтаксической фигни
	return (i + sizeof(*this));
}

void CSyntaxFeature::SetSyntaxCategory(unsigned int uiSyntaxCategory)
{
	m_uiSyntaxCategory = uiSyntaxCategory;
}

void CSyntaxFeature::SetSyntaxFeatureAposteriori(SS::Core::Features::CSyntaxFeatureAposteriori* pSyntaxFeatureAposteriori)
{
	if(pSyntaxFeatureAposteriori)
		m_SyntaxFeatureAposteriori.Assign(pSyntaxFeatureAposteriori);
}

void CSyntaxFeature::SetSyntaxFeatureApriori(SS::Core::Features::CSyntaxFeatureApriori* pSyntaxFeatureApriori)
{
	if(pSyntaxFeatureApriori)
		m_SyntaxFeatureApriori.Assign(pSyntaxFeatureApriori);
}

unsigned int CSyntaxFeature::GetSyntaxCategory()
{
	return m_uiSyntaxCategory;
}

SS::Core::Features::CSyntaxFeatureAposteriori* CSyntaxFeature::GetSyntaxFeatureAposteriori()
{
	return &m_SyntaxFeatureAposteriori;
}

SS::Core::Features::CSyntaxFeatureApriori* CSyntaxFeature::GetSyntaxFeatureApriori()
{
	return &m_SyntaxFeatureApriori;
}

void CSyntaxFeature::SetIsQuestion()
{
	m_bQuestion = true;
}

void CSyntaxFeature::SetIsNotQuestion()
{
	m_bQuestion = false;
}

unsigned int CSyntaxFeature::IsQuestion()
{
    return (unsigned int) m_bQuestion;
}

}
}
}
