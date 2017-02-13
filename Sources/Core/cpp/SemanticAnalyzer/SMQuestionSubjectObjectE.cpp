#include "StdAfx.h"
#include ".\smquestionsubjectobjecte.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Question
			{
				namespace English
				{
					CSMQuestionSubjectObjectE::CSMQuestionSubjectObjectE(void)
					{
						m_AnalyseLanguage=SS::Core::Types::ealEnglish;
						m_sDescription=_T(__FUNCTION__);
					}

					CSMQuestionSubjectObjectE::~CSMQuestionSubjectObjectE(void)
					{
					}

					bool CSMQuestionSubjectObjectE::AnalizeSentence()
					{
						SS_TRY
						{
							do{
								//wstring str = m_pCurrentUnit->GetWord();
								if((IsContainFeature(efcSemanticCommon) || IsContainFeature(efEDNounNominative))&&
									!IsContainFeature(efcDetectedNoun) && !IsContainFeature(efECaseGenetive))
								{
									AddSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin);
									AddSyntaxCategoryIndex(Values::SyntaxCategories.scObject);
									AddFeature(efcDetectedNoun);
								}

							}while(GetNextUnit());
							return true;
						}
						SS_CATCH(L"")
					}
				}

			}
		}
	}
}