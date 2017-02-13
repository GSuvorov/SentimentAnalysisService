#include "StdAfx.h"
#include ".\box_containers.h"
#include ".\pool_client.h"

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

using namespace SS::Core::NdxSE::NdxProcessor::MemoryManagement;
using namespace SS::Interface::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//

unsigned int CBitChunk::ToArray(vector<unsigned int>* pBitArray, unsigned int uiStartValue)
{
	if(!pBitArray) return uiStartValue;
	if(IsEmpty()) return uiStartValue;

	unsigned int uiValue=uiStartValue;
	unsigned char ucLastBox=0;
	do{
		if(m_BitBoxes[ucLastBox].Type==cucBitBoxType){
			//для бокса с битами проверяем выставлены биты
			for(unsigned char i=0; i<=cucBitMaxBitValue; i++){
				if(m_BitBoxes[ucLastBox].IsSetBit(i))
				{
					pBitArray->push_back(uiValue);
				}
				uiValue++;
			}
		}else{
			//для нулбокса увеличиваем значение
			uiValue+=m_BitBoxes[ucLastBox].Value*cucBitBoxValueSize;
		}

	}while(++ucLastBox<=m_ucLastBox);
	
	return uiValue;
}

//--------------------------------------------------------------------//

CBitMap::CBitMap(void)
:m_pTailBitChunk(NULL), m_usBitChunkNumber(0), m_uiLastBitValue(EMPTY_VALUE)
{
}

CBitMap::~CBitMap(void)
{
	Reset();
}

void CBitMap::Reset(void)
{
	CBitChunk* pTempBitChunk=NULL;
	while(m_pTailBitChunk){
		pTempBitChunk=m_pTailBitChunk->GetNextItem();
		delete m_pTailBitChunk;
		m_pTailBitChunk=pTempBitChunk;
	}

	m_pTailBitChunk=NULL;
	m_usBitChunkNumber=0;
	m_uiLastBitValue=EMPTY_VALUE;
}

bool CBitMap::AddBit(unsigned int uiBitValue)
{

 	if(uiBitValue==m_uiLastBitValue) return false;

	if(!m_pTailBitChunk)
		AddBitChunk();

	//получаем требуемую дистанцию от последнего бита в крайнем боксе
	//получаем значение последнего бита в крайнем боксе
	unsigned int uiBitDistance;
	unsigned int uiLastBitInBox;
	if(m_uiLastBitValue==EMPTY_VALUE){
		uiBitDistance=uiBitValue+1;
		uiLastBitInBox=EMPTY_VALUE;
	}else{
		if(uiBitValue<m_uiLastBitValue){
			TO_CONSOLE(L"BitValue not increase, cann't add bit");
			ToConsole(L"BitValue", uiBitValue);
			ToConsole(L"LastBitValue", m_uiLastBitValue);
			return false;
		}
		uiBitDistance=uiBitValue-m_uiLastBitValue;
		uiLastBitInBox=(unsigned int)m_pTailBitChunk->GetLastBitBox()->GetLastBitValue();
	}

	//добавляем бит в карту
	if(uiLastBitInBox+uiBitDistance<=cucBitMaxBitValue){
		//если можем добавить в последний бокс, добавляем
		if(m_pTailBitChunk->IsEmpty()) m_pTailBitChunk->AddBitBox();
		m_pTailBitChunk->GetLastBitBox()->SetBit(uiLastBitInBox+uiBitDistance);
	}else{
		//если дистанция слишком большая
		//потихоньку сокращаем расстояние и добавляем бит
		if(m_uiLastBitValue!=EMPTY_VALUE) uiBitDistance-=cucBitMaxBitValue-uiLastBitInBox;
		
		//определяем сколько нужно нулевых боксов (сколько помещается 7-ми биток)
		//и значение бита в последнем боксе
		div_t div_result=div(uiBitDistance, cucBitBoxValueSize);
		unsigned int uiNeededBitBoxNumber=div_result.quot;
		unsigned char ucBitValue=(unsigned char)div_result.rem;
		
		//если значение делится нацело, бит будет находится в конце последнего бокса
		if(!ucBitValue){
			uiNeededBitBoxNumber--;
			ucBitValue=cucBitMaxBitValue;
		}else{
			ucBitValue--;
		}

		//определяем количество NullBox-ов для хранения числа пустых 7-ми биток
		div_result=div(uiNeededBitBoxNumber, cucBitMaxNumberValue);
		//количество полных NullBox-ов
		unsigned int uiNullBoxNumber=div_result.quot;
		//количество битбоксов в последнем NullBox-е
		unsigned int uiLastNullBoxValue=div_result.rem;
		
		//общее количестов нулбоксов с учетом последнего неполного
		if(uiLastNullBoxValue) uiNullBoxNumber++;
		//добавляем все это в битовую карту
		unsigned int uiValue=cucBitMaxNumberValue;
		while(uiNullBoxNumber){
			//для последнего нулбокса значение другое
			if(uiNullBoxNumber==1)	uiValue=uiLastNullBoxValue;
			
			//проверяем блок на полноту
			if(m_pTailBitChunk->IsFull()){
				//если блок полон боксов добавляем очередной блок 
				AddBitChunk();
			}

			//добавлям в блок нулбокс
			m_pTailBitChunk->AddNullBox(uiValue);
			
			//уменьшаем счетчик
			uiNullBoxNumber--;
		}
		
		//проверяем блок на полноту
		if(m_pTailBitChunk->IsFull()){
			//если блок полон боксов добавляем очередной блок 
			AddBitChunk();
		}
		
		//добавляем битбокс
		m_pTailBitChunk->AddBitBox();
		//выставляем в нем нужный бит
		m_pTailBitChunk->GetLastBitBox()->SetBit(ucBitValue);	
	}

	//устанавливаем значение последнего бита карты
	m_uiLastBitValue=uiBitValue;
	
	return true;
}

void CBitMap::AddBitChunk(void)
{
	CBitChunk* pTempBitChunk=new CBitChunk();
	pTempBitChunk->SetNextItem(m_pTailBitChunk);
	m_pTailBitChunk=pTempBitChunk;
	m_usBitChunkNumber++;
}

unsigned int CBitMap::Write(INdxLevelStorage* pNdxStorage)
{
	if(ISNULL(pNdxStorage)) return 0;

	unsigned int uiWritedBytes=0;
	//пишем значение последнего бита
	unsigned int uiLastBitValue=GetLastBitValue();
	uiWritedBytes+=WriteCompressed(pNdxStorage, uiLastBitValue);
	//m_pNdxStorage->Append((unsigned char*)&uiLastBitValue, sizeof(unsigned int));
	
	//пишем количество блоков с боксами
	unsigned short usBitChunkNumber=GetBitChunkNumber();
	pNdxStorage->Append((unsigned char*)&usBitChunkNumber, sizeof(unsigned short));
	uiWritedBytes+=sizeof(unsigned short);

	//пишем количество боксов в крайнем блоке
	CBitChunk* pTempBitChunk=GetTailBitChunk();
	unsigned char ucChunkSize=pTempBitChunk->GetBitBoxesNumber(); 
	pNdxStorage->Append((unsigned char*)&ucChunkSize, sizeof(unsigned char));
	while(pTempBitChunk){
		//пишем очередной блок с боксами
		unsigned char* pBuffer=pTempBitChunk->GetBitBoxBuffer();
		pNdxStorage->Append(pBuffer, ucChunkSize);
		uiWritedBytes+=ucChunkSize;
		ucChunkSize=cucBoxInBitChunk;
		pTempBitChunk=pTempBitChunk->GetNextItem();
	}

	//View();

	return uiWritedBytes;
}

void CBitMap::Read(INdxLevelStorage* pNdxStorage)
{
	if(ISNULL(pNdxStorage)) return;
	
	//читаем значение последнего бита
	//unsigned int uiLastBitValue;
	//m_pNdxStorage->Read((unsigned char*)&uiLastBitValue, sizeof(unsigned int));
	//pBitMap->SetLastBitValue(uiLastBitValue);
	SetLastBitValue(ReadCompressed(pNdxStorage));

	//читаем количество блоков с боксами
	unsigned short usBitChunkNumber;
	pNdxStorage->Read((unsigned char*)&usBitChunkNumber, sizeof(unsigned short));
	SetBitChunkNumber(usBitChunkNumber);

	//читаем количество боксов в крайнем блоке
	unsigned char ucChunkSize; 
	pNdxStorage->Read((unsigned char*)&ucChunkSize, sizeof(unsigned char));
	
	//читаем блоки с боксами, формируем список блоков
	CBitChunk* pTempBitChunk=NULL;
	while(usBitChunkNumber){

		if(pTempBitChunk){ 
			//если блок не первый добавляем к крайнему в списке
			pTempBitChunk->SetNextItem(new CBitChunk());
			pTempBitChunk=pTempBitChunk->GetNextItem();
		}else{
			m_pTailBitChunk=new CBitChunk();
			pTempBitChunk=GetTailBitChunk();
		}
		
		//читаем содержимое блока
		pNdxStorage->Read(pTempBitChunk->GetBitBoxBuffer(), ucChunkSize);
		pTempBitChunk->SetBitBoxesNumber(ucChunkSize);

		ucChunkSize=cucBoxInBitChunk;
		usBitChunkNumber--;
	}
	
	//View();
}

void CBitMap::ToString(wstring& szValue)
{
	//формируем прямую линейную коллекцию блоков
	list<CBitChunk*> BitChunkCollection;
	CBitChunk* pTempBitChunk=m_pTailBitChunk;
	while(pTempBitChunk){
		BitChunkCollection.push_front(pTempBitChunk);
		pTempBitChunk=pTempBitChunk->GetNextItem();
	}
	
	//формируем массив значений выставленных битов
	vector<unsigned int> BitArray;
	unsigned int uiBitValue=0;
	for(list<CBitChunk*>::iterator itBitChunk=BitChunkCollection.begin(); itBitChunk!=BitChunkCollection.end(); itBitChunk++){
		uiBitValue=(*itBitChunk)->ToArray(&BitArray, uiBitValue);
	}

	//переводим в строку
	if(BitArray.empty()){
		szValue.append(L"-empty-");
	}else{
		wchar_t buf[30];
		for(unsigned int i=0; i<BitArray.size(); i++){
			wsprintf(buf, L"%u ", BitArray[i]);
			szValue.append(buf);
		}
	}
}

void CBitMap::View(void)
{
	ToConsole(L"//-------Bof-CBitMap");
	wstring szValue;
	ToString(szValue);
	ToConsole(szValue.c_str());
	ToConsole(L"//-------Eof-CBitMap");
}

//--------------------------------------------------------------------//

}
}
}
}
}