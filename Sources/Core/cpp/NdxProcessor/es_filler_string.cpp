#include "StdAfx.h"
#include ".\es_filler_string.h"

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

using namespace SS::Core::NdxSE::NdxProcessor::Storages;

//--------------------------------------------------------------------//

CESFillerFromString::CESFillerFromString(Containers::CContainersFactory* pCntFactory, wstring* pExpression)
:CESFillerEx<wstring>(pCntFactory, pExpression)
{
}

CESFillerFromString::~CESFillerFromString(void)
{
}

void CESFillerFromString::Fill(CCalcEquation* pEquation, CSubster* pSubster)
{
	if(ISNULL(pSubster)) return;
	if(ISNULL(pEquation)) return;
	if(ISNULL(m_pFillingSource)) return;
	if(ISNULL(GetFactory())) return;

	if(m_pFillingSource->empty()) return;
	m_ArgCollectionForSubster.clear();
	CCalcArgument* pCurrArgument=pEquation->GetRootArgument();
	wstring szToken;
	unsigned int uiCurrPos=0;
	do{
		//достаем токен из выражения
		uiCurrPos=GetToken(m_pFillingSource, uiCurrPos, &szToken);	
		//вставлем его в дерево формулы
		pCurrArgument=pCurrArgument->InsertCalcArgument(&szToken);

		if(ISNULL(pCurrArgument)) break;
		
		//заполняем коллекцию для подстановщика
		AddForSubster(pCurrArgument);

	}while(uiCurrPos<m_pFillingSource->size());
	
	//заполянем подстановщик значениями
	FillSubster(pSubster);

	//оптимизируем формулу, удаляем лишние аргументы
	pEquation->OptimizeCalcEquation();

	//pEquation->View();
}

unsigned int CESFillerFromString::GetToken(wstring* pszExpression, unsigned int uiCurrPos, wstring* pszToken)
{
	if(ISNULL(pszExpression)) return EMPTY_VALUE;  
	if(ISNULL(pszToken)) return EMPTY_VALUE;  
	if(uiCurrPos>=pszExpression->size()){
		TO_CONSOLE(L"Too big uiCurrPos");
		ToConsole(L"CurrPos", uiCurrPos);
		return EMPTY_VALUE;
	}
	
	//готовим строку
	pszToken->reserve(10);
	pszToken->clear();

	switch((*pszExpression)[uiCurrPos]){
	case '&': case '|': case '~': case '(':	case ')':
		//проверяем на одиночные символы
		pszToken->push_back((*pszExpression)[uiCurrPos]);	
		uiCurrPos++;
		break;
	case '0': case '1': case '2': case '3':	case '4':case '5':case '6':case '7':case '8':case '9':
		//проверяем на цифровое значение
		while(uiCurrPos<pszExpression->size()){
			if((*pszExpression)[uiCurrPos]>='0' && (*pszExpression)[uiCurrPos]<='9'){
				pszToken->push_back((*pszExpression)[uiCurrPos]);
			}else{
				break;
			}
			uiCurrPos++;
		}
		break;
	default:
		TO_CONSOLE(L"Undefined expression simbol!!");
		pszToken->clear();
		return EMPTY_VALUE;
		break;
	}

	return uiCurrPos;
}

void CESFillerFromString::ViewFillingSource(void)
{
	if(ISNULL(m_pFillingSource)) return;

	ToConsole(L"BaseSearchWords expression:");
	if(m_pFillingSource->empty()){
		ToConsole(L"--EMPTY--");
		return;
	}

	ToConsole(m_pFillingSource->c_str());
	//SAVE_LOG(SS_MESSAGE, m_pFillingSource->c_str(), NULL);
}

//--------------------------------------------------------------------//

}
}
}
}
}