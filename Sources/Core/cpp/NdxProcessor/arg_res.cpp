#include "StdAfx.h"
#include ".\arg_res.h"
#include ".\index_storage.h"
#include ".\coordinate_convertor.h"

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
using namespace SS::Interface::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//

CResultArgument::CResultArgument(unsigned int uiArgumentID, float fKoeff)
:CTreeNode<CArgumentEx*>(new CArgumentEx(uiArgumentID, fKoeff)), m_bProcessed(false)
{
}

CResultArgument::~CResultArgument(void)
{
	//даем возможность результату самому удалять свое значение
	if(GetMyself()->GetValue())
		delete GetMyself()->GetValue();
}

CResultArgument* CResultArgument::AddChild(CResultArgument* pChildArgument)
{
	if(ISNULL(pChildArgument)) return NULL;
	
	//добавляем результат ребенком
	CResultArgument* pExistedChild=(CResultArgument*)(CTreeNode<CArgumentEx*>::AddChild(pChildArgument->GetMyself()->GetArgumentID(), pChildArgument));

	if(ISNULL(pExistedChild)) return NULL;
	
	//инитим буфер истории
	pExistedChild->GetHistoryBuffer()->assign(pExistedChild->GetDepth(), EMPTY_VALUE);
	//копируем ему свою историю
	copy(m_HistoryBuffer.begin(), m_HistoryBuffer.end(), pExistedChild->GetHistoryBuffer()->begin());

	//"вписываем" себя историю ребенка на нужной глубине
	if(pExistedChild->GetHistoryBuffer()->size()>GetDepth()){
		(*(pExistedChild->GetHistoryBuffer()))[GetDepth()]=pExistedChild->GetMyself()->GetArgumentID();
	}else{
		TO_CONSOLE(L"History buffer size too small");
	}

	return pExistedChild;
}

CResultArgument* CResultArgument::GetChild(unsigned int uiChildId)
{
	TChilds::iterator itChild=GetChilds()->find(uiChildId);
	if(itChild!=GetChilds()->end()){
		return (CResultArgument*)(itChild->second);
	}else{
		return NULL;
	}
}

void CResultArgument::CutByDepth(unsigned int uiDepth)
{
	if(GetDepth()==uiDepth){
		DeleteChilds();
	}else{
		TChilds::iterator itCurrChild=GetChilds()->begin();
		while(itCurrChild!=GetChilds()->end()){
			CResultArgument* pResultArgument=(CResultArgument*)(itCurrChild->second);
			if(pResultArgument){
				pResultArgument->CutByDepth(uiDepth);			
			}
			++itCurrChild;
		}	
	}
}

unsigned int CResultArgument::CalculateBaseIDFromResultHistory(Storages::CIndexStorage* pIndexStorage)
{
	if(ISNULL(pIndexStorage)) return 0;
	
	unsigned int uiCurrBasisTextsNumber=EMPTY_VALUE;
	THistoryBuffer* pHistoryBuffer=GetHistoryBuffer();
	if(pHistoryBuffer->empty()){
		TO_CONSOLE(L"pHistoryBuffer empty invalid pResultArgument");
		return 0;
	}

	THistoryBuffer::iterator itHistoryBuffer=pHistoryBuffer->begin();
	Storages::CIndexStorage::TCoordinateLevelInfo* pLevelCollection=pIndexStorage->GetCoordinateLevelCollection();
	if(ISNULL(pLevelCollection)) return 0;
	if(pLevelCollection->empty()){
		TO_CONSOLE(L"pLevelCollection empty");
		return 0;
	}

	uiCurrBasisTextsNumber=0;
	Storages::CIndexStorage::TCoordinateLevelInfo::iterator itLevelStorage=pLevelCollection->begin();
	do{
		SNdxLevelInfo const* pLevelInfo=(*itLevelStorage);
		if(ISNULL(pLevelInfo)){ 
			uiCurrBasisTextsNumber=EMPTY_VALUE;
			break;
		}

		if(pLevelInfo->m_IndexCoordinateType==SNdxLevelInfo::ictTextAbsNumber){
			//вытаскиваем из координаты кусочек для уровня
			SS::Core::NdxSE::NdxProcessor::CCoordinateConvertor::InsertToCoordinate(
				&uiCurrBasisTextsNumber,
				(unsigned int)(*itHistoryBuffer), 
				pLevelInfo->m_ucStartBit,
				pLevelInfo->m_ucBitPerLevel);

			if(++itHistoryBuffer==pHistoryBuffer->end()) break;
		}		
	}while(++itLevelStorage!=pLevelCollection->end());
	
	return uiCurrBasisTextsNumber;
}

void CResultArgument::ToString(wstring& szValue, wstring szOffset)
{
	szOffset.append(L" ");	
	szValue.append(L"\n");
	szValue.append(szOffset);

	if(IsRoot()){
		szValue.append(L"R");
	}else{
		szValue.append(L"|");
		wchar_t buf[10];
		wsprintf(buf, L"%u", GetMyself()->GetArgumentID());
		szValue.append(buf);
	}

	szValue.append(L"-->");

	if(GetMyself()){
		GetMyself()->ToString(szValue);	

		//char buf[30];
		//sprintf(buf, "co %u", GetMyself()->GetValue()->GetCurrentOrt());
		//szValue.append(buf);
	}
	//szValue.append(L" |h ");
	//char buf[100];
	//THistoryBuffer::iterator itCurrID=m_HistoryBuffer.begin();
	//while(itCurrID!=m_HistoryBuffer.end()){
	//	sprintf(buf, "%d ", *itCurrID);
	//	szValue.append(buf);
	//	++itCurrID;
	//}
	//szValue.append(L"h| ");

	TChilds::iterator itCurrChild=GetChilds()->begin();
	while(itCurrChild!=GetChilds()->end()){
		CResultArgument* pResultArgument=(CResultArgument*)(itCurrChild->second);
		if(pResultArgument){
			pResultArgument->ToString(szValue, szOffset);			
		}

		++itCurrChild;
	}	
}

void CResultArgument::View(void)
{
	wstring szValue=L"//--------ResultArgument";
	wstring szOffset=L"";	
	szValue.append(L"\n");
	szValue.append(szOffset);

	if(IsRoot()){
		szValue.append(L"R");
	}else{
		szValue.append(L"|");
		wchar_t buf[10];
		wsprintf(buf, L"%u", GetMyself()->GetArgumentID());
		szValue.append(buf);
	}

	szValue.append(L"-->");
	if(GetMyself()){
		wchar_t buf[30];
		wsprintf(buf, L" co %u", GetMyself()->GetValue()->GetCurrentOrt());
		szValue.append(buf);
	}

	//szValue.append(L" |h ");
	//char buf[100];
	//THistoryBuffer::iterator itCurrID=m_HistoryBuffer.begin();
	//while(itCurrID!=m_HistoryBuffer.end()){
	//	sprintf(buf, "%d ", *itCurrID);
	//	szValue.append(buf);
	//	++itCurrID;
	//}
	//szValue.append(L"h| ");

	szValue.append(L"\n//------Eof-ResultArgument\n");

	ToConsole(szValue.c_str());
}

//--------------------------------------------------------------------//

}
}
}
}
}