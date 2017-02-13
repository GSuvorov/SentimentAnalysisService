#include "StdAfx.h"
#include ".\box_containers_s.h"

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

using namespace SS::Interface::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//

CBitMap_4s::CBitMap_4s(unsigned int uiBitMapSizeInBits, EBitMapState eBitMapState)
:m_uiBitMapSizeInBits(uiBitMapSizeInBits), m_ucCurrBitNumber(UC_EMPTY), 
m_eBitMapState(eBitMapState), m_uiCurrBitValue(EMPTY_VALUE),
m_uiCurrBoxPos(EMPTY_VALUE), m_uiLastBitValue(EMPTY_VALUE)
{
	m_itCurrBox = m_NullCollection.end();
	if(m_eBitMapState==esEmpty){
		Reset();
	}else if(m_eBitMapState==esFull){
		Fill();	
	}
}

unsigned int CBitMap_4s::GetBitMapBoxSize(void)
{
	div_t div_result=div(m_uiBitMapSizeInBits, cucBitBoxValueSize);
	return div_result.rem==0?div_result.quot:div_result.quot+1;
}

void CBitMap_4s::Fill(void)
{
	SBitBox BitBox;
	memset((void*)&BitBox, cucFullBitBox, 1);
	m_BitMapBoxes.assign(GetBitMapBoxSize(), BitBox);	
	SetState(esFull);
	m_uiLastBitValue=GetOrtsMaxNumber()-1;
}

void CBitMap_4s::Reset(void)
{
	m_BitMapBoxes.clear();
	SetState(esEmpty);
	m_uiLastBitValue=EMPTY_VALUE;
}

bool CBitMap_4s::AddBit(unsigned int uiBitValue)
{
 	if(m_uiLastBitValue!=EMPTY_VALUE && uiBitValue<=m_uiLastBitValue) return false;

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
		uiLastBitInBox=m_BitMapBoxes.back().GetLastBitValue();
	}

	//добавляем бит в карту
	if(uiLastBitInBox+uiBitDistance<=cucBitMaxBitValue){
		//если можем добавить в последний бокс, добавляем
		if(m_BitMapBoxes.empty()){
			SBitBox BitBox;
			BitBox.Type=cucBitBoxType;
			BitBox.Value=0;
			m_BitMapBoxes.push_back(BitBox);
		}
		m_BitMapBoxes.back().SetBit(uiLastBitInBox+uiBitDistance);
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
			
			//добавлям в блок нулбокс
			SBitBox NullBox;
			NullBox.Type=cucNullBoxType;
			NullBox.Value=uiValue;
			m_BitMapBoxes.push_back(NullBox);
			
			//уменьшаем счетчик
			uiNullBoxNumber--;
		}
		
		//добавляем битбокс
		//выставляем в нем нужный бит
		SBitBox BitBox;
		BitBox.Type=cucBitBoxType;
		BitBox.Value=0;
		m_BitMapBoxes.push_back(BitBox);
		m_BitMapBoxes.back().SetBit(ucBitValue);
	}

	//устанавливаем значение последнего бита карты
	m_uiLastBitValue=uiBitValue;
	
	return true;
}

bool CBitMap_4s::SetOrt(unsigned int uiBitPos)
{
	//для полной карты не нужно
	if(IsFull()) return false;
	
	SetNotEmpty();
	
	//если возможно производим добавление бита
 	if(m_uiLastBitValue==EMPTY_VALUE || uiBitPos>m_uiLastBitValue) 
		return AddBit(uiBitPos);

	unsigned int uiBoxPos=0;
	unsigned char ucBitValue=0;
	//определяем номер нужного хранилища и номер бита в нем
	div_t div_result=div(uiBitPos, cucBitBoxValueSize);
	uiBoxPos=div_result.quot;
	ucBitValue=(unsigned char)div_result.rem;

	unsigned int uiCurrPos=0;
	TBitBoxCollection::iterator itBox=m_BitMapBoxes.begin(); 
	while(itBox!=m_BitMapBoxes.end()){
		//определяем тип хранилища
		if(itBox->IsBitBox()){
			if(uiCurrPos==uiBoxPos){
				//если нашли нужное хранилище единиц выставляем в нем бит и выходим
				itBox->SetBit(ucBitValue);
				return true;
			}
			uiCurrPos++;
		}else{
			//если это хранилище нулей
			if(uiCurrPos+itBox->Value<=uiBoxPos){
				//если позиция бокса дальше пропускаем
				uiCurrPos+=itBox->Value;
			}else{
				//если позиция бокса где между
				//меняем количество нулбоксов
				unsigned char ucTemp=itBox->Value-(uiBoxPos-uiCurrPos)-1;
				//такой нулбокс будет перед битбоксом
				if(uiBoxPos-uiCurrPos){
					itBox->Value=(uiBoxPos-uiCurrPos);
					//если нулбокс не исчерпан, вставляем пустой битбокс для битов
					SBitBox NewBox;
					NewBox.SetNumber(0);
					itBox=m_BitMapBoxes.insert(++itBox, NewBox);
				}else{
					itBox->SetNumber(0);
				}

				//выставляем бит
				itBox->Type=cucBitBoxType;
				itBox->SetBit(ucBitValue);
				
				if(ucTemp){
					SBitBox NewBox;
					NewBox.Type=cucNullBoxType;
					NewBox.Value=ucTemp;
					itBox=m_BitMapBoxes.insert(++itBox, NewBox);
				}
				return true;
			}
		}
		itBox++;
	}

	return false;
}

bool CBitMap_4s::HasOrt(unsigned int uiBitPos)
{
	//для полной карты не нужно
	if(IsFull()) return true;
	//для пустой карты не нужно
	if(IsEmpty()) return false;

	//определяем номер нужного хранилища и номер бита в нем
	div_t div_result=div(uiBitPos, cucBitBoxValueSize);
	//позиция нужного хранилища
	unsigned int uiBoxPos=div_result.quot;
	
	TBitBoxCollection::iterator itBox=m_BitMapBoxes.begin(); 
	unsigned int uiCurrPos=0;
	do{
		//определяем тип хранилища
		if(itBox->IsBitBox()){
			if(uiCurrPos==uiBoxPos){
				//если нашли нужное хранилище единиц выставляем в нем бит и выходим
				return itBox->IsSetBit(div_result.rem);
			}
			uiCurrPos++;
		}else{
			//если это хранилище нулей, сдвигаем позицию на количество нулей
			uiCurrPos+=itBox->Value;
		}
		if(uiBoxPos<uiCurrPos){
			//если прошли нужную позицию хранилища нет, выходим			
			return false;
		}
	}while(++itBox!=m_BitMapBoxes.end());
	
	return false;
}

bool CBitMap_4s::MoveFirstOrt(void)
{
	//для пустой карты не получится
	m_ucCurrBitNumber=UC_EMPTY;
	m_uiCurrBoxPos=EMPTY_VALUE;
	m_uiCurrBitValue=EMPTY_VALUE;
	if(IsEmpty()) return false;

	//START_TICK(L"MoveFirstOrt");
	//переход на первый выставленный бит
	TBitBoxCollection::iterator itBox=m_BitMapBoxes.begin(); 
	unsigned int uiCurrPos=0;
	while(itBox!=m_BitMapBoxes.end()){
		if(itBox->IsBitBox()){
			m_ucCurrBitNumber=itBox->GetFirstBitValue();
			//определяем номер первого выставленного бита
			if(m_ucCurrBitNumber==UC_EMPTY){
				TO_CONSOLE(L"GetFirstBitValue error!");
				m_uiCurrBoxPos=EMPTY_VALUE;
				m_itCurrBox=m_NullCollection.end();
				return false;
			}else{
				m_itCurrBox=itBox;
				m_uiCurrBoxPos=uiCurrPos;
				m_uiCurrBitValue=m_ucCurrBitNumber+(m_uiCurrBoxPos*cucBitBoxValueSize);
				return true;
			}
		}else{
			uiCurrPos+=itBox->Value;
		}
		itBox++;
	}
	//STOP_TICK(L"MoveFirstOrt");

	return false;
}

bool CBitMap_4s::MoveNextOrt(void)
{
	if(m_itCurrBox == m_NullCollection.end()) return false;
	//для пустой карты не получится
	if(IsEmpty()) return false;
	
	//START_TICK(L"MoveNextOrt");
	//переход на следующий выставленный бит
	//начинаем с текущего хранилища
	TBitBoxCollection::iterator itBox=m_itCurrBox; 
	unsigned int uiCurrPos=m_uiCurrBoxPos;
	while(itBox!=m_BitMapBoxes.end()){
		if(itBox->IsBitBox()){
			//получаем номер очередного бита в текущем хранилище
			m_ucCurrBitNumber=itBox->GetNextBitValue(m_ucCurrBitNumber+1);
			if(m_ucCurrBitNumber!=UC_EMPTY){
				//если в данном хранилище есть еще бит
				m_itCurrBox=itBox;
				m_uiCurrBoxPos=uiCurrPos;
				m_uiCurrBitValue=m_ucCurrBitNumber+(m_uiCurrBoxPos*cucBitBoxValueSize);
				return true;
			}else{
				//если в данном хранилище нет очередного бита, смещаемся дальше
				uiCurrPos++;
			}
		}else{
			//нулевые хранилища просто проходим увеличивая позицию
			uiCurrPos+=itBox->Value;
		}
		itBox++;
	}

	//STOP_TICK(L"MoveNextOrt");
	
	//если очередного бита нет выходим
	m_ucCurrBitNumber=UC_EMPTY;
	m_uiCurrBoxPos=EMPTY_VALUE;
	m_uiCurrBitValue=EMPTY_VALUE;
	m_itCurrBox=m_NullCollection.end();
	return false;
}

bool CBitMap_4s::MoveLastOrt(void)
{
	m_ucCurrBitNumber=UC_EMPTY;
	m_uiCurrBoxPos=EMPTY_VALUE;
	m_uiCurrBitValue=EMPTY_VALUE;

	//для пустой карты не получится
	if(IsEmpty()) return false;

	m_uiCurrBitValue=GetLastBitValue();
	return true;

	////переход на последний выставленный бит
	//TBitBoxCollection::iterator itBox=m_BitMapBoxes.end(); 
	//unsigned int uiCurrPos=GetBitMapBoxSize()-1;
	//while(itBox--!=m_BitMapBoxes.begin()){
	//	if(itBox->IsBitBox()){
	//		m_ucCurrBitNumber=itBox->GetLastBitValue();
	//		if(m_ucCurrBitNumber==UC_EMPTY){
	//			TO_CONSOLE(L"GetLastBitValue error!");
	//			m_uiCurrBoxPos=EMPTY_VALUE;
	//			m_itCurrBox=NULL;
	//			return false;
	//		}else{
	//			m_itCurrBox=itBox;
	//			m_uiCurrBoxPos=uiCurrPos;
	//			m_uiCurrBitValue=m_ucCurrBitNumber+(m_uiCurrBoxPos*cucBitBoxValueSize);
	//			return true;
	//		}
	//	}else{
	//		uiCurrPos-=itBox->Value;
	//	}
	//}

	//return false;
}

void CBitMap_4s::MakeAnd(CValue* pValue)
{
	//сливаем битовые карты
	Merge(static_cast<CBitMap_4s*>(pValue), mmAND);
	//вызываем базовую операцию	
	CValue::MakeAnd(pValue);
}

void CBitMap_4s::MakeSmoothAnd(CValue* pValue, unsigned int uiAdjacentSize)
{
	//сливаем битовые карты
	MergeAdjacent(static_cast<CBitMap_4s*>(pValue), uiAdjacentSize);
	//вызываем базовую операцию	
	CValue::MakeSmoothAnd(pValue, uiAdjacentSize);
}

void CBitMap_4s::MakeOr(CValue* pValue)
{
	//сливаем битовые карты
	Merge(static_cast<CBitMap_4s*>(pValue), mmOR);
	//вызываем базовую операцию	
	CValue::MakeOr(pValue);
}

void CBitMap_4s::Assign(CBitMap_4s* pBitMap)
{
	SetState(pBitMap->GetState());
	m_BitMapBoxes.assign(pBitMap->m_BitMapBoxes.begin(), pBitMap->m_BitMapBoxes.end());
	SetLastBitValue(pBitMap->GetLastBitValue());
}

bool CBitMap_4s::PreMerge(CBitMap_4s* pBitMap, EMergeMode MergeMode)
{
	if(ISNULL(pBitMap)) return true;

	//карты должны быть одинаковых размеров
	if(GetBitMapSize()!=pBitMap->GetBitMapSize()){
		ERR_TO_CONSOLE(erNotEqual, L"GetBitMapSize");
		ToConsole(L"MergeMode", (unsigned int)MergeMode);
		ToConsole(L"This Box Size", GetBitMapSize());
		ToConsole(L"Input Box Size", pBitMap->GetBitMapSize());
		return true;
	}
	
	//для особых случаев...
	switch(MergeMode)
	{
	//сливаем значения по И
	case mmAND:
		if(IsEmpty() || pBitMap->IsEmpty()){
			//если хоть один пустой - результат пустой
			Reset();
			return true;
		}

		if(IsFull() && pBitMap->IsFull()){
			//если обе карты полные сразу выходим считать не нужно
			return true;
		}

		if(IsFull()){
			//если полная this карта, приравниваем ей значение другой
			Assign(pBitMap);
			return true;
		}

		if(pBitMap->IsFull()){
			//если полная входная карта выходим считать не нужно
			return true;
		}
		break;
	//сливаем значения по ИЛИ
	case mmOR:
		if(IsFull()){
			//если полный - результат полный
			return true;
		}

		if(pBitMap->IsFull()){
			//если хоть один полный - результат полный
			Fill();
			return true;
		}

		if(IsEmpty() && pBitMap->IsEmpty()){
			//если оба пустые - результат пустой
			return true;
		}

		if(IsEmpty()){
			//если this карта пустая, приравниваем ей значение другой
			Assign(pBitMap);
			return true;
		}

		if(pBitMap->IsEmpty()){
			//если входная карта пустая выходим считать не нужно
			return true;
		}
		break;
	default:
		TO_CONSOLE(L"MergeMode undefined");
		return true;
		break;
	}
	
	return false;
}

void CBitMap_4s::Merge(CBitMap_4s* pBitMap, EMergeMode MergeMode)
{
	//запускаем предвычисления если они прошли дальше считать не нужно
	if(PreMerge(pBitMap, MergeMode)) return;
	
	//if((unsigned int)MergeMode==1){
	//	ToConsole(L"Input:");
	//	View();
	//	pBitMap->View();
	//	ToConsole(L"MergeMode", (unsigned int)MergeMode);
	//}
	//буфер результата
	TBitBoxCollection ResultBitMapBoxes;

	SMoveInBitMap ThisMoveInBitMap;
	SMoveInBitMap ThatMoveInBitMap;
	SBitBox ResultBox;

	TBitBoxCollection::iterator  itThisBox=m_NullCollection.end();
	TBitBoxCollection::iterator  itThatBox=m_NullCollection.end();
	ThisMoveInBitMap.m_uiSteps=1;
	ThisMoveInBitMap.m_uiNotDoneSteps=0;
	ThatMoveInBitMap.m_uiSteps=1;
	ThatMoveInBitMap.m_uiNotDoneSteps=0;
	unsigned int uiTotalBoxesCount=0;

	GetNextBox(&m_BitMapBoxes, &itThisBox, &ThisMoveInBitMap);
	GetNextBox(&pBitMap->m_BitMapBoxes, &itThatBox, &ThatMoveInBitMap);
	while(itThisBox!=m_BitMapBoxes.end() && itThatBox!=pBitMap->m_BitMapBoxes.end()){

		//выполняем операцию с боксами, определяем на сколько нужно сместиться каждому
		switch(MergeMode){
		case mmAND:
			ResultBox=MakeBoxAnd(&ThisMoveInBitMap, &ThatMoveInBitMap);
			break;
		case mmOR:
			ResultBox=MakeBoxOr(&ThisMoveInBitMap, &ThatMoveInBitMap);
			break;
		default:
			return;
		}

		//добавляем бокс в результат, считаем общее количество боксов
		uiTotalBoxesCount+=ResultBox.IsNullBox()?ResultBox.Value:1;
		AddResultBox(&ResultBitMapBoxes, ResultBox);
		
		//получаем очередные боксы для вычисления
		GetNextBox(&m_BitMapBoxes, &itThisBox, &ThisMoveInBitMap);
		GetNextBox(&pBitMap->m_BitMapBoxes, &itThatBox, &ThatMoveInBitMap);
	}

	//постобработка
	switch(MergeMode){
	case mmAND:
		//убираем нулбоксы
		while(!ResultBitMapBoxes.empty() && ResultBitMapBoxes.back().IsNullBox()){ 
			uiTotalBoxesCount-=ResultBitMapBoxes.back().Value;
			ResultBitMapBoxes.pop_back();
		}
		if(ResultBitMapBoxes.empty())
			Reset();
		else
			//устанавливаем значение последнего бита
			SetLastBitValue((cucBitBoxValueSize*(uiTotalBoxesCount-1))+ResultBitMapBoxes.back().GetLastBitValue());
		break;
	case mmOR:
		{
			TBitBoxCollection::iterator  itBox=m_NullCollection.end();
			TBitBoxCollection* pBitMapBoxes=NULL;
			SMoveInBitMap* MoveInBitMap=NULL;
			if(itThisBox!=m_BitMapBoxes.end()){
				itBox=itThisBox;
				pBitMapBoxes=&m_BitMapBoxes;
				MoveInBitMap=&ThisMoveInBitMap;
			}else if(itThatBox!=pBitMap->m_BitMapBoxes.end()){
				itBox=itThatBox;
				pBitMapBoxes=&pBitMap->m_BitMapBoxes;
				MoveInBitMap=&ThatMoveInBitMap;
				//устанавливаем значение последнего бита
				SetLastBitValue(pBitMap->GetLastBitValue());
			}else if(itThisBox==m_BitMapBoxes.end() && itThatBox==pBitMap->m_BitMapBoxes.end()){
				//если битовые карты имеют одиноковое количество боксов
				//последним будет больший бит
				if(pBitMap->GetLastBitValue()>GetLastBitValue())
					SetLastBitValue(pBitMap->GetLastBitValue());
			}
			
			if(itBox!=m_NullCollection.end()){
				//добавляем оставшиеся боксы в конец
				if(itBox->IsNullBox())
					itBox->Value=MoveInBitMap->m_uiNotDoneSteps+1;
				ResultBitMapBoxes.push_back(*itBox);
				while(++itBox!=pBitMapBoxes->end()){
					ResultBitMapBoxes.push_back(*itBox);
				}
			}
		}
		break;
	default:
		return;
	}

	if(!ResultBitMapBoxes.empty() && ResultBitMapBoxes.back().IsNullBox()){
		TO_CONSOLE(L"Warning not correct result of merging");
		ToConsole(L"MergeMode", (unsigned int)MergeMode);
	}


	//свапируем коллекцию
	m_BitMapBoxes.swap(ResultBitMapBoxes);
	
	//if((unsigned int)MergeMode==1){
	//	ToConsole(L"Result:");
	//	View();
	//}
}

void CBitMap_4s::AddResultBox(TBitBoxCollection* pBitBoxCollection, SBitBox ResultBox)
{
	if(!pBitBoxCollection->empty() && pBitBoxCollection->back().IsNullBox() && ResultBox.IsNullBox()){
		//нулевой бокс пытаемся дополнить
		TBitBoxCollection::iterator  itLastBox=--pBitBoxCollection->end();
		unsigned char ucTemp=cucBitMaxNumberValue-itLastBox->Value;
		if(ucTemp!=0)
		{
			if(ResultBox.Value>ucTemp){
				itLastBox->Value+=ucTemp;
				ResultBox.Value-=ucTemp;
				pBitBoxCollection->push_back(ResultBox);
			}else{
				itLastBox->Value+=ResultBox.Value;
			}
		}else{
			//кладем очередной бокс
			pBitBoxCollection->push_back(ResultBox);
		}
	}else{
		//кладем очередной бокс
		pBitBoxCollection->push_back(ResultBox);
	}
}

void CBitMap_4s::GetNextBox(TBitBoxCollection* pBitBoxCollection, 
							   TBitBoxCollection::iterator*  pitCurrBox, 
							   SMoveInBitMap* pMoveInBitMap)
{
	if(pMoveInBitMap->m_uiSteps==0 && pMoveInBitMap->m_uiNotDoneSteps!=0)
		pMoveInBitMap->m_uiNotDoneSteps--;

	//смещаемся в коллекции на заданное число боксов возвращаем очередной бокс
	while(pMoveInBitMap->m_uiSteps){
		//смещаем итератор		
		if((*pitCurrBox)==m_NullCollection.end())
			(*pitCurrBox)=pBitBoxCollection->begin();
		else
			++(*pitCurrBox);
		
		if((*pitCurrBox)==pBitBoxCollection->end()){
			pMoveInBitMap->m_BitBox.Type=0;
			pMoveInBitMap->m_BitBox.SetNumber(cucBitMaxNumberValue);
			return;
		}

		if((*pitCurrBox)->IsNullBox()){
			if(pMoveInBitMap->m_uiSteps<(*pitCurrBox)->Value){
				pMoveInBitMap->m_uiNotDoneSteps=(*pitCurrBox)->Value-pMoveInBitMap->m_uiSteps;
				pMoveInBitMap->m_uiSteps=0;
				break;
			}else{
				pMoveInBitMap->m_uiSteps-=(*pitCurrBox)->Value;
			}
		}else{
			pMoveInBitMap->m_uiSteps--;
		}
	}

	pMoveInBitMap->m_BitBox=(**pitCurrBox);
}

SBitBox CBitMap_4s::MakeBoxAnd(SMoveInBitMap* pThisMoveInBitMap, SMoveInBitMap* pThatMoveInBitMap)
{
	pThisMoveInBitMap->m_uiSteps=1;
	pThatMoveInBitMap->m_uiSteps=1;

	SBitBox ResultBitBox;
	
	//если оба битбоксы, сливаем
	if(pThisMoveInBitMap->m_BitBox.IsBitBox() && pThatMoveInBitMap->m_BitBox.IsBitBox()){
		ResultBitBox.Type=cucBitBoxType;
		ResultBitBox.Value=pThisMoveInBitMap->m_BitBox.Value&pThatMoveInBitMap->m_BitBox.Value;
		//если результат нулевой делаем нулбоксом
		if(!ResultBitBox.Value){
			ResultBitBox.Type=cucNullBoxType;
			ResultBitBox.Value=1;
		}
	}else{
		//в остальных случаях битбокс делаем нулбоксами
		if(pThisMoveInBitMap->m_BitBox.IsBitBox()){
			pThisMoveInBitMap->m_BitBox.Type=cucNullBoxType;
			pThisMoveInBitMap->m_BitBox.SetNumber(1);
		}
		if(pThatMoveInBitMap->m_BitBox.IsBitBox()){
			pThatMoveInBitMap->m_BitBox.Type=cucNullBoxType;
			pThatMoveInBitMap->m_BitBox.SetNumber(1);
		}

		//если оба нулбоксы, выбираем наибольший
		if(pThisMoveInBitMap->m_BitBox.IsNullBox() && pThatMoveInBitMap->m_BitBox.IsNullBox()){
			unsigned char ucMax=max(pThisMoveInBitMap->m_uiNotDoneSteps, pThatMoveInBitMap->m_uiNotDoneSteps);
			pThisMoveInBitMap->m_uiSteps+=ucMax-pThisMoveInBitMap->m_uiNotDoneSteps;
			pThatMoveInBitMap->m_uiSteps+=ucMax-pThatMoveInBitMap->m_uiNotDoneSteps;
			ResultBitBox.Type=cucNullBoxType;
			ResultBitBox.SetNumber(ucMax+1);
		}
	}

	pThisMoveInBitMap->m_uiNotDoneSteps=0;
	pThatMoveInBitMap->m_uiNotDoneSteps=0;
	return ResultBitBox;
}

SBitBox CBitMap_4s::MakeBoxOr(SMoveInBitMap* pThisMoveInBitMap, SMoveInBitMap* pThatMoveInBitMap)
{
	pThisMoveInBitMap->m_uiSteps=1;
	pThatMoveInBitMap->m_uiSteps=1;
	SBitBox ResultBitBox;
	
	//если оба битбоксы, сливаем
	if(pThisMoveInBitMap->m_BitBox.IsBitBox() && pThatMoveInBitMap->m_BitBox.IsBitBox()){
		ResultBitBox.Type=cucBitBoxType;
		ResultBitBox.Value=pThisMoveInBitMap->m_BitBox.Value|pThatMoveInBitMap->m_BitBox.Value;
		//если результат нулевой делаем нулбоксом
		if(!ResultBitBox.Value){
			ResultBitBox.Type=cucNullBoxType;
			ResultBitBox.Value=1;
		}
		return ResultBitBox;
	}


	//в остальных случаях битбокс делаем нулбоксами
	if(pThisMoveInBitMap->m_BitBox.IsNullBox() && pThatMoveInBitMap->m_BitBox.IsBitBox()){
		ResultBitBox=pThatMoveInBitMap->m_BitBox;
		pThisMoveInBitMap->m_uiSteps=pThisMoveInBitMap->m_uiNotDoneSteps==0?1:0;
	}

	if(pThatMoveInBitMap->m_BitBox.IsNullBox() && pThisMoveInBitMap->m_BitBox.IsBitBox()){
		ResultBitBox=pThisMoveInBitMap->m_BitBox;
		pThatMoveInBitMap->m_uiSteps=pThatMoveInBitMap->m_uiNotDoneSteps==0?1:0;
	}

	//если оба нулбоксы, выбираем наименьший
	if(pThisMoveInBitMap->m_BitBox.IsNullBox() && pThatMoveInBitMap->m_BitBox.IsNullBox()){
		unsigned char ucMin=min(pThisMoveInBitMap->m_uiNotDoneSteps, pThatMoveInBitMap->m_uiNotDoneSteps);
		pThisMoveInBitMap->m_uiNotDoneSteps-=ucMin;
		pThatMoveInBitMap->m_uiNotDoneSteps-=ucMin;

		pThisMoveInBitMap->m_uiSteps=pThisMoveInBitMap->m_uiNotDoneSteps==0?1:0;
		pThatMoveInBitMap->m_uiSteps=pThatMoveInBitMap->m_uiNotDoneSteps==0?1:0;

		ResultBitBox.Type=cucNullBoxType;
		ResultBitBox.SetNumber(ucMin+1);
	}

	return ResultBitBox;
}

void CBitMap_4s::MergeAdjacent(CBitMap_4s* pBitMap, int iSizeAdjacent)
{
	if (IsEmpty() || pBitMap->IsFull()) return;

	if(pBitMap->IsEmpty() ) {
		Reset();
		return;
	}

	if(IsFull()) 
	{
		Merge(pBitMap, mmAND);
		return;
	}

	CBitMap_4s temp(m_uiBitMapSizeInBits);
	int bit1, bit2, i, n;
	if ( MoveFirstOrt() ) 
	{
		do{
			bit1=this->GetCurrentOrt();
			bit2=(bit1-iSizeAdjacent) >=0 ? (bit1-iSizeAdjacent):0;
			n=((int)(m_uiBitMapSizeInBits-1)>(bit1+iSizeAdjacent))?(bit1+iSizeAdjacent):m_uiBitMapSizeInBits-1;
			for ( i=bit2 ; i <= n; i++ ) 
				if ( pBitMap->HasOrt( i ) ) 
				{
					temp.SetOrt(bit1);
					break;
				}
		}while ( MoveNextOrt()) ;
	}
	m_BitMapBoxes.assign(temp.m_BitMapBoxes.begin(), temp.m_BitMapBoxes.end());
	SetState(temp.GetState());
	m_itCurrBox=temp.m_itCurrBox;
	m_uiCurrBoxPos=temp.m_uiCurrBoxPos;
}

void CBitMap_4s::Read(INdxLevelStorage* pNdxStorage)
{
	if(ISNULL(pNdxStorage)) return;
	
	START_TICK(L"CBitMap_4s::Read");
	//читаем значение последнего бита
	unsigned int uiLastBitValue=ReadCompressed(pNdxStorage);
	SetLastBitValue(uiLastBitValue);

	//читаем количество блоков с боксами
	unsigned short usBitChunkNumber;
	pNdxStorage->Read((unsigned char*)&usBitChunkNumber, sizeof(unsigned short));

	//читаем количество боксов в крайнем блоке
	unsigned char ucChunkSize; 
	pNdxStorage->Read((unsigned char*)&ucChunkSize, sizeof(unsigned char));
	
	//определяем общее количестов боксов
	unsigned int uiTotalBoxNumber=(usBitChunkNumber-1)*cucBoxInBitChunk+ucChunkSize;
	
	//ToConsole(L"uiTotalBoxNumber", uiTotalBoxNumber);

	//резервируем местов коллекции
	SBitBox BitBox;
	BitBox.Type=0;
	BitBox.SetNumber(0);
	m_BitMapBoxes.assign(uiTotalBoxNumber, BitBox);

	//читаем блоки с боксами, формируем список блоков
	CBitChunk* pTempBitChunk=NULL;
	unsigned char* pCurrentBox=((unsigned char*)(&m_BitMapBoxes.front()))+m_BitMapBoxes.size();
	while(usBitChunkNumber){
		pCurrentBox-=ucChunkSize;
		//читаем содержимое блока
		pNdxStorage->Read(pCurrentBox, ucChunkSize);
		ucChunkSize=cucBoxInBitChunk;
		usBitChunkNumber--;
	}
	STOP_TICK(L"CBitMap_4s::Read");
	
	//View();
}

void CBitMap_4s::ToString(wstring& szValue)
{
	wchar_t buf[500];
	if(MoveFirstOrt()){
		do{
			wsprintf(buf, L" %u ", GetCurrentOrt());
			szValue.append(buf);
		}while(MoveNextOrt());
	}else{
		szValue.append(L"--empty--");
	}
}

void CBitMap_4s::View(void)
{
	wstring szValue=L"";
	if(IsFull()){
		szValue=L"BitMap Filled";
	}else{
		ToString(szValue);
	}

	ToConsole(L"//------BoxedBitMap:");
	ToConsole(szValue.c_str());
	ToConsole(L"//--End-of-BoxedBitMap");
}

//--------------------------------------------------------------------//

}
}
}
}
}