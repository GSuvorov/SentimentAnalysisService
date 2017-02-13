#include "StdAfx.h"
#include ".\SMExSimpleQueryParser.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{

			CSMExSimpleQueryParser::CSMExSimpleQueryParser(void)
			{
				m_AnalyseLanguage=SS::Core::Types::ealUndefined;
				m_sDescription=_T(__FUNCTION__);
			}

			CSMExSimpleQueryParser::~CSMExSimpleQueryParser(void)
			{
			}

			bool CSMExSimpleQueryParser::AnalizeSentence()
			{
				SS_TRY
				{
					UINT uiWordCountInSentence=0;
					wstring sz_Word;
					TBoxUnits l_units;
					IUnit * pHead;

					do{
						uiWordCountInSentence++;
						sz_Word=m_pCurrentUnit->GetWord();
						if ((IsWord(sz_Word.c_str()) && uiWordCountInSentence > 2))
						{
							Mem();
							do{
								if(!m_LexemType.IsSymbol())
									l_units.push_back(m_pCurrentUnit);
							}while(GetNextUnit());
							pHead=JoinUpUnits(l_units);
							if (pHead)
							{
								GotoMem();
								m_pCurrentUnit = pHead;
								SetSyntaxCategory(pHead,Values::SyntaxCategories.scAjunct);
								SetSyntax();
							}
						}
					}while(GetNextUnit());

					if (l_units.empty()==false)
					{
						l_units.clear();
					}
				

					return true;
				}
				SS_CATCH(L"")
			}

			bool CSMExSimpleQueryParser::Init(IText * pIText)
			{
				if (!CSMExpert::Init(pIText)) return false;

				return true;

			}


			bool CSMExSimpleQueryParser::IsWord(const TCHAR * sWord)
			{
				wstring sz_Word(sWord);
				m_StringService.LowerSz((TCHAR*)sz_Word.c_str());
				set<wstring>::iterator i=m_Words.find(sz_Word.c_str());
				return (i!=m_Words.end());
			}

		}
	}
}