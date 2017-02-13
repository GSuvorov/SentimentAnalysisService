#include "StdAfx.h"


#include ".\syntaxaposteriorianalyzer.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			CSyntaxAposterioriAnalyzer::CSyntaxAposterioriAnalyzer(void)
			{
			}

			CSyntaxAposterioriAnalyzer::~CSyntaxAposterioriAnalyzer(void)
			{
			}

			/*	кодирование апосториорной синтаксической информации	*/
			void CSyntaxAposterioriAnalyzer::Code(
				/*in*/SS::Core::Features::CSyntaxFeatureAposteriori *pSyntaxFeatureAposteriori, 
				/*out*/unsigned int* piSyntaxInfo) 
			{
				SS_TRY;

					*piSyntaxInfo = pSyntaxFeatureAposteriori->m_SyntaxCategories.GetSingleValue();		

				SS_CATCH(L"");
			}
				/*	декодирование апосториорной синтаксической информации	*/
			void CSyntaxAposterioriAnalyzer::EnCode(
				/*in*/unsigned int iSyntaxInfo,	
				/*out*/SS::Core::Features::CSyntaxFeatureAposteriori *pSyntaxFeatureAposteriori)
			{
				SS_TRY;

					pSyntaxFeatureAposteriori->m_SyntaxCategories	= iSyntaxInfo;

				SS_CATCH(L"");
			}
		}
	}
}