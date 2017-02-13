#include "StdAfx.h"
#include ".\logged_index_container.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Containers
{

//--------------------------------------------------------------------//

CLoggedIndexContainer::CLoggedIndexContainer(void) : 
	m_pCheckResultLog(NULL), m_uiCurrBasisTextsNumber(0), m_itCurrentIndex()
{	
}

CLoggedIndexContainer::~CLoggedIndexContainer(void)
{
	Clear();
}

void CLoggedIndexContainer::Clear(void)
{
	while(!m_CheckingTextCollection.empty()){
		if(m_CheckingTextCollection.back()){
			delete m_CheckingTextCollection.back();
		}
		m_CheckingTextCollection.pop_back();
	}

	TLoggedIndexesCollection::iterator itIndex=m_LoggedIndexesCollection.begin();
	while(itIndex!=m_LoggedIndexesCollection.end()){
		if(itIndex->second){
			for(unsigned int i=0; i<itIndex->second->size(); i++){
				if(itIndex->second->at(i))
					delete itIndex->second->at(i);
			}
			delete itIndex->second;
		}
		itIndex++;
	}
	m_LoggedIndexesCollection.clear();

	m_LostPositions.clear();
	m_NotCorrectPositions.clear();
}

void CLoggedIndexContainer::Init(wstring* pwsLogFilePath)
{
	Clear();

	FILE* pFileStream=_wfopen(pwsLogFilePath->c_str(), L"r");
	if(ISNULL(pFileStream)) return;

	char szLine[10000];
	wchar_t wszLine[10000];
	while(!feof(pFileStream)){
		szLine[0]=0;
		wszLine[0]=0;

		if(!fgets(szLine, 10000, pFileStream))
			break;

		szLine[strlen(szLine)-1]=0;
		MultiByteToWideChar(CP_ACP, 0, szLine, -1, wszLine, 10000); 
		wszLine[strlen(szLine)]=0;
		AddIndexInfo(wszLine);
	}

	fclose(pFileStream);

}

void CLoggedIndexContainer::AddIndexInfo(const wchar_t* pwszIndexInfo)
{
	unsigned int uiIndex=0;
	unsigned int uiText=0;
	unsigned int uiPosition=0;
	unsigned int uiStep=0;

	try{
	//вытаскиваем индекс
	wchar_t sepsForIndexAttr[]   = L"; ";

	wchar_t* wszIndexAttrToken;
	wszIndexAttrToken = wcstok((wchar_t*)pwszIndexInfo, sepsForIndexAttr);
	unsigned int uiAttrInc=0;
	while(wszIndexAttrToken != NULL)
	{
		switch(uiAttrInc){
		case 0:
			uiIndex=_wtoi(wszIndexAttrToken);
			break;
		case 1:
			uiText=_wtoi(wszIndexAttrToken);
			break;
		case 2:
			uiPosition=_wtoi(wszIndexAttrToken);
			break;
		default:
			break;
		}
		uiAttrInc++;
		wszIndexAttrToken = wcstok(NULL, sepsForIndexAttr);
	}

	uiStep=1;

	TLoggedIndexesCollection::iterator itIndex=m_LoggedIndexesCollection.find(uiIndex);
	if(itIndex==m_LoggedIndexesCollection.end()){
		itIndex=m_LoggedIndexesCollection.insert(TLoggedIndexesCollection::value_type(uiIndex, new TIndexInTextInfoCollection())).first;
		itIndex->second->push_back(new CIndexInTextInfo());
		itIndex->second->back()->SetTextIndex(uiText);
	}

	uiStep=2;
	
	if(itIndex->second->back()->GetTextIndex()==uiText){
		uiStep=3;
		itIndex->second->back()->AddPosition(uiPosition);
	}else if(itIndex->second->back()->GetTextIndex()<uiText){
		uiStep=4;
		itIndex->second->push_back(new CIndexInTextInfo());
		uiStep=5;
		itIndex->second->back()->SetTextIndex(uiText);
		uiStep=6;
		itIndex->second->back()->AddPosition(uiPosition);
	}else{
		TO_CONSOLE(L"Not correct index info!!!");
	}

	}catch(...){
		ToConsole(L"uiIndex", uiIndex);
		ToConsole(L"uiText", uiText);
		ToConsole(L"uiPosition", uiPosition);
		ToConsole(L"uiStep", uiStep);
		throw;
	}
}	

bool CLoggedIndexContainer::MoveFirstIndex(void)
{
	if(m_LoggedIndexesCollection.empty()){
		m_itCurrentIndex=m_NullCollection.end();
		FillIndexInfoForChecking(m_itCurrentIndex);
		return false;
	}else{
		m_itCurrentIndex=m_LoggedIndexesCollection.begin();
		FillIndexInfoForChecking(m_itCurrentIndex);

		m_pCheckResultLog=_wfopen(L"./log_index_vrf.txt", L"wt");

		return true;
	}
}

bool CLoggedIndexContainer::MoveNextIndex(void)
{
	if(m_itCurrentIndex==m_NullCollection.end()) return false;

	if(++m_itCurrentIndex==m_LoggedIndexesCollection.end()){
		m_itCurrentIndex=m_NullCollection.end();
		FillIndexInfoForChecking(m_itCurrentIndex);
		if(m_pCheckResultLog){ 
			fclose(m_pCheckResultLog);
			m_pCheckResultLog=NULL;
		}
		return false;
	}else{
		
		//перед переходом к следующему индексу сбрасываем реузльтаты проверки предыдущего
		LogCheckResults();

		FillIndexInfoForChecking(m_itCurrentIndex);
		return true;
	}
}

void CLoggedIndexContainer::LogCheckResults(void)
{
	if(m_itCurrentIndex==m_NullCollection.end()) return;
	if(!m_pCheckResultLog) return;
	
	//если все хорошо ниче не пишем

	if(m_LostPositions.empty() && m_NotCorrectPositions.empty()) return;

	fwprintf(m_pCheckResultLog, L"Index: %u\n", m_itCurrentIndex->first);

	if(m_LostPositions.empty() && m_NotCorrectPositions.empty()){
		fwprintf(m_pCheckResultLog, L"\tAll OK\n");
	} 

	if(!m_LostPositions.empty()){
		fwprintf(m_pCheckResultLog, L"\tLostPositions: %u\n", m_itCurrentIndex->first);
		for(unsigned int i=0; i<m_LostPositions.size(); i++){
			fwprintf(m_pCheckResultLog, L"\t\t%u\n", m_LostPositions[i]);
		}
	}

	if(!m_NotCorrectPositions.empty()){
		fwprintf(m_pCheckResultLog, L"\tNotCorrectPositions: %u\n", m_itCurrentIndex->first);
		for(unsigned int i=0; i<m_NotCorrectPositions.size(); i++){
			fwprintf(m_pCheckResultLog, L"\t\t%u\n", m_NotCorrectPositions[i]);
		}
	}
}

void CLoggedIndexContainer::CurrentIndexCheckCoordinates(unsigned int uiTextIndex, unsigned int uiPosition)
{
	if(m_itCurrentIndex==m_NullCollection.end()) return;

	try{
	//добавляем базовый номер текста
	uiTextIndex+=m_uiCurrBasisTextsNumber;

	if(uiTextIndex<m_CheckingTextCollection.size()){
		if(m_CheckingTextCollection[uiTextIndex]==NULL){
			ToConsole(L"Undefined Text", uiTextIndex);
		}else{
			CIndexInTextInfo::TPositionsCollection::iterator itPosition=m_CheckingTextCollection[uiTextIndex]->find(uiPosition);
			if(itPosition==m_CheckingTextCollection[uiTextIndex]->end()){
				//проверяемой позиции в выборке нет, она неправильная
				m_NotCorrectPositions.push_back(uiPosition);
			}else{
				//правильную позицию удаляем, чтобы потом проверить сколько осталось не востребованных
				m_CheckingTextCollection[uiTextIndex]->erase(itPosition);
			}
		}
	}

	}catch(...){
		ToConsole(L"Index", m_itCurrentIndex->first);
		ToConsole(L"uiText", uiTextIndex);
		ToConsole(L"uiPosition", uiPosition);
		throw;
	}

}

void CLoggedIndexContainer::FillIndexInfoForChecking(TLoggedIndexesCollection::iterator itCurrentIndex)
{
	m_LostPositions.clear();
	m_NotCorrectPositions.clear();

	while(!m_CheckingTextCollection.empty()){
		if(m_CheckingTextCollection.back()){
			delete m_CheckingTextCollection.back();
		}
		m_CheckingTextCollection.pop_back();
	}

	if(itCurrentIndex==m_NullCollection.end()) return;
	
	//проходим по коллекции перегоняем все контайнеры проверки
	TIndexInTextInfoCollection* pIndexInTextInfoCollection=itCurrentIndex->second;
	m_CheckingTextCollection.resize(pIndexInTextInfoCollection->back()->GetTextIndex()+1, NULL);
	
	CIndexInTextInfo::TPositionsCollection* pPositionsCollection=NULL;
	for(unsigned int i=0; i<pIndexInTextInfoCollection->size(); i++){
		pPositionsCollection=new CIndexInTextInfo::TPositionsCollection();
		pIndexInTextInfoCollection->at(i)->ConvertPosition(pPositionsCollection);
		unsigned int uiCurrentText=pIndexInTextInfoCollection->at(i)->GetTextIndex();
		
		m_CheckingTextCollection[uiCurrentText]=pPositionsCollection;
	}
}

//--------------------------------------------------------------------//

}
}
}
}
}