#include "StdAfx.h"
#include ".\linguisticindexation.h"

#include "..\ASSInterface\ITextBlockEx.h"
#include "..\ASSInterface\Iterators.h"

#include <iostream>

#include <crtdbg.h>
#include <assert.h>

//#define _SS_UNITTESTS

namespace SS
{
namespace LinguisticProcessor
{
namespace Indexation
{

unsigned int GetLastPositon(SS::Interface::Core::BlackBox::IText * pText)
{
	unsigned int uiLastPosition = 0;
	SS_TRY 
	{

		using namespace SS::Interface::Core::BlackBox;

		if(!pText->GetLastParagraph())
		{
#ifdef _SS_UNITTESTS
			std::wstring ws = __WFUNCTION__;
			ws += L" !pText->GetLastParagraph()";
			SL_OUT_DEBUG_STR(ws.c_str());
#endif
			return uiLastPosition;
		}
		if(!pText->GetLastParagraph()->GetLastSentence())
		{
#ifdef _SS_UNITTESTS
			std::wstring ws = __WFUNCTION__;
			ws += L" !pText->GetLastParagraph()->GetLastSentence()";
			SL_OUT_DEBUG_STR(ws.c_str());
#endif
			return uiLastPosition;
		}

		IUnit * pUnit = pText->GetLastParagraph()->GetLastSentence()->GetLastUnit();

		if(pUnit)
		{
			SS::Iterators::CSentenceIterator si;
			uiLastPosition = si.GetRightmostChild(pUnit)->GetPosition();

			/*if(pUnit->HasChilds())
			{
				if(pUnit->GetRightChildUnit()->HasChilds())
				{
					uiLastPosition = pUnit->GetRightChildUnit()->GetRightChildUnit()->GetPosition();
				}else
				{
					uiLastPosition = pUnit->GetRightChildUnit()->GetPosition();
				}

			}else
			{
				uiLastPosition = pUnit->GetPosition();
			}*/
		}		

	}SS_CATCH(L"")

	return uiLastPosition;
}

unsigned int GetUnitLastPositon(SS::Interface::Core::BlackBox::IUnit * pUnit)
{
	SS_TRY
	{
		if(pUnit)
		{
			if(pUnit->HasChilds())
			{
				return GetUnitLastPositon(pUnit->GetRightChildUnit());
				//uiLastPosition = ->GetPosition();

			}else
			{
				return pUnit->GetPosition();
			}
		}
	}
	SS_CATCH(L"")
}

unsigned int GetFistPositon(SS::Interface::Core::BlackBox::IText * pText)
{
	unsigned int uiFirstPosition = 0;
	SS_TRY 
	{

		using namespace SS::Interface::Core::BlackBox;

		if(!pText->GetFirstParagraph())
		{
#ifdef _SS_UNITTESTS
			std::wstring ws = __WFUNCTION__;
			ws += L" !pText->GetFirstParagraph()";
			SL_OUT_DEBUG_STR(ws.c_str());
#endif
			return uiFirstPosition;
		}
		if(!pText->GetFirstParagraph()->GetFirstSentence())
		{
#ifdef _SS_UNITTESTS
			std::wstring ws = __WFUNCTION__;
			ws += L" !pText->GetFirstParagraph()->GetFirstSentence()";
			SL_OUT_DEBUG_STR(ws.c_str());
#endif
			return uiFirstPosition;
		}

		IUnit * pUnit = pText->GetFirstParagraph()->GetFirstSentence()->GetFirstUnit();
		
		if(pUnit)
		{
			if(pUnit->HasChilds())
			{
				uiFirstPosition = pUnit->GetLeftChildUnit()->GetPosition();

			}else
			{
				uiFirstPosition = pUnit->GetPosition();
			}
		}

	}SS_CATCH(L"")

	return uiFirstPosition;
}

CLinguisticIndexation::CLinguisticIndexation(void) 
: m_uiCurPosition(0), m_bIsTableIndexation(false), m_bIsTextIndexation(false), uiFistCharPosition(0)
{
}
void CLinguisticIndexation::SetMode(SS::Core::Types::IndexationSearch::TLinguisticProcessorMode* pMode)
{
	m_oTextIndexation.SetMode(pMode);
	m_oTableIndexation.SetMode(pMode);
}

CLinguisticIndexation::~CLinguisticIndexation(void)
{
}

void CLinguisticIndexation::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
	SS_TRY
	{
		m_oTextIndexation.SetLoadManager(pLoadManager);
		m_oTableIndexation.SetLoadManager(pLoadManager);
	}
	SS_CATCH(L"")
}

HRESULT CLinguisticIndexation::StartDocumentIndexation()
{
	SS_TRY
	{
		uiFistCharPosition = 0;
		m_uiCurPosition = 0;
		return S_OK;
	}
	SS_CATCH(L"")
}

HRESULT CLinguisticIndexation::EndDocumentIndexation()
{
	SS_TRY
	{
		return S_OK;
	}
	SS_CATCH(L"")
}	

void CLinguisticIndexation::Init(
	SS::Core::Types::IndexationSearch::TNativeTextForIndexation* pNativeText
	, SS::Core::Types::IndexationSearch::TIndexationParams* pIndexationParams
	)
{
	SS_TRY
	{
		SL_OUT_DEBUG_STR (__WFUNCTION__);
		//SaveNativeText(pNativeText) ;

		m_bIsTextIndexation = true;
		m_bIsTableIndexation = false;

		m_oTableIndexation.Clear();
		m_oTextIndexation.Clear();

		m_oTextIndexation.Init(pNativeText, pIndexationParams, uiFistCharPosition);

		SL_OUT_DEBUG_STR (__WFUNCTION__);

	}
	SS_CATCH(L"")

}

void CLinguisticIndexation::Init(
	std::list< std::list<SS::Core::Types::IndexationSearch::TNativeTextForIndexation*> > * p_l_l_NativeText
	/// параметры индексации текста
	, SS::Core::Types::IndexationSearch::TIndexationParams* pIndexationParams
	)
{
	SS_TRY
	{
		SL_OUT_DEBUG_STR (__WFUNCTION__);

		m_bIsTextIndexation = false;
		m_bIsTableIndexation = true;

		m_oTableIndexation.Clear();
		m_oTextIndexation.Clear();

		m_oTableIndexation.Init(p_l_l_NativeText, pIndexationParams);

		SL_OUT_DEBUG_STR (__WFUNCTION__);
	}
	SS_CATCH(L"")
}



unsigned int CLinguisticIndexation::GenerateNext(SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult)
{
//	assert( _CrtCheckMemory( ) );

	unsigned int uiRetVal;

	SS_TRY
	{
		SL_OUT_DEBUG_STR (__WFUNCTION__);

		pIndexationResult->Clear();
		if(m_bIsTextIndexation)
            uiRetVal = m_oTextIndexation.GenerateNext(uiFistCharPosition, m_uiCurPosition, pIndexationResult);
		else if(m_bIsTableIndexation)
			uiRetVal = m_oTableIndexation.GenerateNext(uiFistCharPosition, m_uiCurPosition, pIndexationResult);

		SL_OUT_DEBUG_STR (__WFUNCTION__);
	}
	SS_CATCH(L"")

	return uiRetVal;
}



//void CLinguisticIndexation::SaveText(int iTextNumber, const wchar_t* wzText, const wchar_t* wzTextType)
//{
//	SS_TRY
//	{
//		wchar_t wsTextName[100];
//		wchar_t wsTextNumber[20];
//		_itow(iTextNumber, wsTextNumber, 10);
//		wcscat(wsTextNumber, L".txt");
//
//		wcscpy(wsTextName, L"d:\\IndexationTest\\Out\\");
//		wcscat(wsTextName, wzTextType);		
//		wcscat(wsTextName, wsTextNumber);
//
//		HANDLE hFile = ::CreateFile(
//			wsTextName, 
//			GENERIC_READ|GENERIC_WRITE, 
//			FILE_SHARE_READ|FILE_SHARE_WRITE, 
//			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);	
//
//		if ( INVALID_HANDLE_VALUE == hFile) 
//		{
//			SAVE_LOG(SS_ERROR AND wsTextName AND L"Open" AND __WFUNCTION__);
//			return ;
//		}
//
//		USES_CONVERSION_EX;		
//
//		DWORD dwSizeOf = 0;
//		DWORD dwSize = (DWORD) wcslen(wzText);
//		LPSTR lpBuff = W2A_EX(wzText, (dwSize + 1));
//		
//		LONG lDistanceToMoveHigh;
//		SetFilePointer(hFile, 0, &lDistanceToMoveHigh,	FILE_END);
//
//
//		if(!::WriteFile(hFile, lpBuff, dwSize, &dwSizeOf, NULL)/*::ReadFile(m_hFile, lpBuff, dwSize, &dwSize, NULL) || dwSize == 0*/)
//		{
//			SAVE_LOG(SS_ERROR AND wsTextName AND L"Write" AND __WFUNCTION__);
//		}
//		::CloseHandle(hFile);
//
//		return ;
//		
//	}
//	SS_CATCH(L"")
//
//}
//
//void CLinguisticIndexation::SaveNativeText(SS::Core::Types::IndexationSearch::TNativeTextForIndexation* pNativeText) 
//{
//	SS_TRY
//	{
//		static int siTextNumber = 0;
//
////		if(wcslen(pNativeText->GetText()) > 200)
////		{
//			siTextNumber++;
//	
//			SaveText(siTextNumber, pNativeText->GetText(), L"Text");
//	//		SaveText(siTextNumber, pNativeText->GetTextStructure(), L"TextStructure");
//	//		SaveText(siTextNumber, pNativeText->GetTextInfo(), L"TextInfo");
////		}
//		return ;
//		
//	}
//	SS_CATCH(L"")
//}


}
}
}



