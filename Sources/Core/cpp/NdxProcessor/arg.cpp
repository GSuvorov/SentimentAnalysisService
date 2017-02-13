#include "StdAfx.h"
#include ".\arg.h"
#include ".\arg_complex.h"

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

using namespace SS::Core::NdxSE::NdxProcessor;
using namespace SS::Core::NdxSE::NdxProcessor::Containers;

//--------------------------------------------------------------------//

CArgument::CArgument(unsigned int uiArgumentID, float fKoeff, SS::Interface::Core::CommonContainers::SQueryIndexAttributes ArgumentAttributes)
:m_uiArgumentID(uiArgumentID), m_fKoeff(fKoeff), m_pCurrentValue(NULL), m_Attributes(ArgumentAttributes)
{
}

CArgument::~CArgument(void)
{
}

void CArgument::ToString(wstring& szValue)
{
	wchar_t buf[100];
	
	if(m_uiArgumentID==EMPTY_VALUE){
		wcscpy(buf, L"Arg: IM ");
	}else{
		wsprintf(buf, L"Arg: ID-%u ", m_uiArgumentID);
	}

	szValue.append(buf);
}

//--------------------------------------------------------------------//

CArgumentEx::CArgumentEx(unsigned int uiArgumentID, float fKoeff, SS::Interface::Core::CommonContainers::SQueryIndexAttributes ArgumentAttributes)
:CArgument(uiArgumentID, fKoeff, ArgumentAttributes), m_pValueOrtsWeights(NULL)
{
}

CArgumentEx::~CArgumentEx(void)
{
	if(m_pValueOrtsWeights){
		delete m_pValueOrtsWeights;
	}
}

CArgumentEx::TValueOrtsWeightsCollection* CArgumentEx::GetValueOrtsWeights(void)
{
	if(!m_pValueOrtsWeights && GetValue()){
		m_pValueOrtsWeights=new TValueOrtsWeightsCollection;
		m_pValueOrtsWeights->assign(GetValue()->GetOrtsMaxNumber(), m_fKoeff);
	}
	return m_pValueOrtsWeights;
}

void CArgumentEx::SetValue(CCompositeValue* pValue)
{	
	//устанавливаем значение
	m_pCurrentValue=pValue;
}

void CArgumentEx::ToString(wstring& szValue)
{
	//wchar_t buf[100];
	
	//отображаем значение аргумента 
	if(GetValue()){
		GetValue()->ToString(szValue);	

		//szValue.append(L" |w ");
		////текущий орт значения
		//unsigned int uiCurrentOrt=0;
		////отображаем веса орт значения
		//CCompositeValue* pCompositeValue=static_cast<CCompositeValue*>(GetValue());
		//if(pCompositeValue->MoveFirstOrt()) 
		//{
		//	do{ 
		//		uiCurrentOrt=pCompositeValue->GetCurrentOrt();

		//		wsprintf(buf, L"%.3f ", m_pValueOrtsWeights->at(uiCurrentOrt));
		//		szValue.append(buf);
		//	}while(pCompositeValue->MoveNextOrt());
		//}
		//szValue.append(L"w|");
	}
}

//--------------------------------------------------------------------//

}
}
}
}
}