#pragma once

#include ".\const.h"
#include ".\console.h"
#include ".\res_containers.h"
#include ".\hdr_logic.h"
#include ".\int_containers.h"

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

///информация по расположению заголовка
#pragma pack(push,1)  // Fix packing size 
struct SHeaderPositions
{
	SHeaderPositions(void):m_uiHeadPosition(EMPTY_VALUE), m_uiTailPosition(EMPTY_VALUE), m_uiBoundPosition(EMPTY_VALUE){};

	///возвращает длину заголовка
	unsigned int GetHeaderLength(void){return m_uiTailPosition-m_uiHeadPosition;};
	///возвращает длину зоны влияния заголовка
	unsigned int GetAffectedZoneLength(void){return m_uiBoundPosition-m_uiTailPosition;};

	///сериализация в строку
	void ToString(wstring& wszValue)
	{
		wchar_t buf[100];
		wsprintf(buf, L"Head: %u, Tail: %u, Bound: %u", m_uiHeadPosition, m_uiTailPosition, m_uiBoundPosition);
		wszValue.assign(buf);
	};

	///головная позиция (начало)
	unsigned int m_uiHeadPosition;
	///хвостовая позиция (конец)
	unsigned int m_uiTailPosition;
	///граничная позиция (определяется границу зоны влияния заголовка)
	///EMPTY_VALUE означает границой является конец документа
	unsigned int m_uiBoundPosition;
};
#pragma pack(pop)

//--------------------------------------------------------------------//

///класс иденитификатор заголовка, шаблонный параметр количество уровней заголовков
#pragma pack(push,1)  // Fix packing size 
template <unsigned int LevelsNumber>
class CHeaderID
{
public:
	CHeaderID(void){Reset();};
	CHeaderID(unsigned int uiHeaderID)
	{
		Reset();
		
		unsigned short* pucValue=(unsigned short*)&uiHeaderID;
		unsigned int uiLevelsNumber=0;
		do{
			m_arrHeaderLevels[LevelsNumber-(uiLevelsNumber+1)]=*pucValue;
			pucValue++;
		}while(++uiLevelsNumber<LevelsNumber);
	};

	void operator=(const CHeaderID& HeaderID)
	{
		memcpy(m_arrHeaderLevels, HeaderID.m_arrHeaderLevels, LevelsNumber*sizeof(unsigned short));
	};

	bool operator==(const CHeaderID& HeaderID) const
	{
		return memcmp(m_arrHeaderLevels, HeaderID.m_arrHeaderLevels, LevelsNumber*sizeof(unsigned short))==0?true:false;
	};

	///оператор меньше определяет младше ли левый заголовок правого
	bool operator<(const CHeaderID& HeaderID) const
	{
		unsigned int uiLevelNumber=0;
		unsigned int uiLeftValue=0;
		unsigned int uiRightValue=0;
		do{
			uiLeftValue=GetValueAtLevel(uiLevelNumber);
			uiRightValue=HeaderID.GetValueAtLevel(uiLevelNumber);
			if(uiRightValue==0 && uiLeftValue!=0) return true;
		}while(++uiLevelNumber<LevelsNumber);
		
		return false;
	};

	///сбрасывает содержимое идентификатора заголовка
	void Reset(void){memset(m_arrHeaderLevels, 0, LevelsNumber*sizeof(unsigned short));};
	///проверяет идентификатор заголовка на пустой
	bool IsEmpty(void){return m_arrHeaderLevels[0]==0?true:false;};
	
	///создает по текущему родительский заголовок
	CHeaderID MakeParentHeader(void)
	{
		CHeaderID HeaderID(GetID());
		unsigned int uiLevelNumber=LevelsNumber;
		while(uiLevelNumber--!=0){
			if(HeaderID.m_arrHeaderLevels[uiLevelNumber]!=0){
				HeaderID.m_arrHeaderLevels[uiLevelNumber]=0;
				break;
			}
		}
		return HeaderID;
	};

	///возвращает уровень заголовка
	unsigned int GetHeaderLevel(void)
	{
		unsigned int uiLevelNumber=LevelsNumber;
		while(uiLevelNumber--!=0){
			if(m_arrHeaderLevels[uiLevelNumber]!=0)	return uiLevelNumber;
		}
		return EMPTY_VALUE;
	};

	///увеличивает значение идентификатора заголовка на заддном уровне
	void IncAtLevel(unsigned int uiLevelValue)
	{
		if(uiLevelValue<LevelsNumber){
			//увеличиваем счетчик текущего уровня
			m_arrHeaderLevels[uiLevelValue]++;
			//сбрасываем все нижние
			while(++uiLevelValue<LevelsNumber) m_arrHeaderLevels[uiLevelValue]=0;
		}else{
			TO_CONSOLE(L"Too big level number!!!");
			ToConsole(L"uiLevelValue", uiLevelValue);
		}
	};

	///возвращает значение идентификатора заголовка
	unsigned int GetID(void)
	{
		unsigned int uiValue=0;
		unsigned int uiLevelsNumber=0;
		do{
			uiValue|=((unsigned int)m_arrHeaderLevels[uiLevelsNumber])<<(LevelsNumber-(uiLevelsNumber+1))*16;
		}while(++uiLevelsNumber<LevelsNumber);
		return uiValue;
	};
	
	///проверяет является ли идентификатор эквивалентным на данном уровне
	bool IsAtSameLevel(CHeaderID* pHeaderID)
	{
		if(ISNULL(pHeaderID)) return false;
		unsigned int uiLevelNumber=0;
		unsigned int uiLeftValue=0;
		unsigned int uiRightValue=0;
		do{
			uiLeftValue=GetValueAtLevel(uiLevelNumber);
			uiRightValue=pHeaderID->GetValueAtLevel(uiLevelNumber);
			if(!((uiRightValue==0 && uiLeftValue==0) ||
				(uiRightValue!=0 && uiLeftValue!=0))) return false;
		}while(++uiLevelNumber<LevelsNumber);

		return true;
	};

	///возвращает значение на заданном уровне
	unsigned int GetValueAtLevel(unsigned int uiLevelValue) const
	{
		if(uiLevelValue>=LevelsNumber){
			TO_CONSOLE(L"Too big level number!!!");
			ToConsole(L"uiLevelValue", uiLevelValue);
			return 0;
		}else{
			return m_arrHeaderLevels[uiLevelValue];
		}
	};
	
	///возвращает идентификатор заголвка созданный из составляющих до определенного уровня
	///получается как бы старший заголовок
	unsigned int GetIDTillLevel(unsigned int uiLevelValue)
	{
		if(uiLevelValue>=LevelsNumber){
			TO_CONSOLE(L"Too big level number!!!");
			ToConsole(L"uiLevelValue", uiLevelValue);
			return 0;
		}

		unsigned int uiValue=0;
		unsigned int uiLevelsNumber=0;
		do{
			uiValue|=((unsigned int)m_arrHeaderLevels[uiLevelsNumber])<<(LevelsNumber-(uiLevelsNumber+1))*16;
		}while(++uiLevelsNumber<=uiLevelValue);
		return uiValue;
	};

	///сериализация в строку
	void ToString(wstring& wszValue)
	{
		wchar_t buf[100];
		wszValue.assign(L"HID: ");
		unsigned int uiLevelNumber=0;
		do{
			wsprintf(buf, L"%u ", GetValueAtLevel(uiLevelNumber));
			wszValue.append(buf);
		}while(++uiLevelNumber<LevelsNumber);
	};

	///отображение объекта класса
	void View(void)
	{
		wstring wszValue;
		ToString(wszValue);
		ToConsole(wszValue.c_str());
	};

protected:
	///массив составляющих идентификатора заголовка
	unsigned short m_arrHeaderLevels[LevelsNumber];
};
#pragma pack(pop)

typedef CHeaderID<cuiHeaderLevelsNumber> CLevelHeaderID;

//--------------------------------------------------------------------//

///информация о заголовке
#pragma pack(push,1)  // Fix packing size 
struct SHeaderInfo
{
	void operator=(const SHeaderInfo& HeaderInfo)
	{
		m_HeaderID=HeaderInfo.m_HeaderID;
		memcpy((unsigned char*)&m_HeaderPositions, (unsigned char*)&HeaderInfo.m_HeaderPositions, sizeof(SHeaderPositions));
	};

	///сериализация в строку
	void ToString(wstring& wszValue)
	{
		wszValue.assign(L"SHeaderInfo: ");
		wstring wszTemp;
		m_HeaderID.ToString(wszTemp);
		wszValue.append(wszTemp);

		m_HeaderPositions.ToString(wszTemp);
		wszValue.append(wszTemp);
	};

	///идентификатор заголовка
	CLevelHeaderID m_HeaderID;
	///информация по расположению заголовка
	SHeaderPositions m_HeaderPositions;
};
#pragma pack(pop)

//--------------------------------------------------------------------//

///информация по заголовкам документа
class CDocumentHeadersInfo : public CConsoleClient
{
public:
	///коллекция заголовков документа
	typedef vector<SHeaderInfo> THeaderInfoCollection;
	
	///обновляет позиция заголовка по идентификатору заголовка
	void UpdateHeaderPosition(unsigned int uiPosition);
	///возвращает коллекцию заголовков документа
	THeaderInfoCollection* GetHeaderInfoCollection(void){return &m_HeaderInfoCollection;};
	///сбрасывает содержимое 
	void Reset(void){m_CurrentHeaderID.Reset(); m_HeaderInfoCollection.clear();};
	///возвращает идентификатор текущего заголовка
	CLevelHeaderID* GetCurrentHeaderID(void){return &m_CurrentHeaderID;};
	///инкрементирует идентификатор текущего заголовка на данном уровне
	bool IncCurrentHeaderID(unsigned int uiHeaderLevel);

	///сериализация в строку
	void ToString(wstring& wszValue);

protected:
	///идентификатор текущего заголовка
	CLevelHeaderID m_CurrentHeaderID;
	///коллекция заголовков документа
	THeaderInfoCollection m_HeaderInfoCollection;
};

//--------------------------------------------------------------------//

//коллекция слова заголовка, содердит флаговый массив
//который указывает слова с какими идентификаторами есть в заголовке
template <unsigned int WordsNumber>
class CHeaderWords
{
public:
	CHeaderWords(void){Reset();};

	///выставляет флаг присутствия слова
	void SetWordID(unsigned int uiWordID)
	{
		if(uiWordID<WordsNumber){
			m_arrWordsIDs[uiWordID]=true;
		}
	};
	///сбрасывает содержимое 
	void Reset(void){memset(m_arrWordsIDs, 0, WordsNumber);};
	///возвращает идентификатор заголовка
	CLevelHeaderID* GetHeaderID(void){return &m_HeaderID;};
	///возвращает максимальное количество слов
	unsigned int GetMaxWordsNumber(void){return WordsNumber;}
	///проверяет есть ли слово с таким идентификатором
	bool IsWordExist(unsigned int uiWordID)
	{
		if(uiWordID<WordsNumber){
			return m_arrWordsIDs[uiWordID];
		}
		return false;
	};

	///сериализация в строку
	void ToString(wstring& wszValue)
	{
		wchar_t buf[100];
		wszValue.assign(L"CHeaderWords: ");
		wstring wszTemp;
		m_HeaderID.ToString(wszTemp);
		wszValue.append(wszTemp);
		wszValue.append(L" WordIDs: ");
		for(unsigned int k=0; k<GetMaxWordsNumber(); k++){
			if(IsWordExist(k)){
				wsprintf(buf, L"%u ", k);
				wszValue.append(buf);
			}
		}
	};

protected:
	///флаговый массив
	bool m_arrWordsIDs[WordsNumber];
	///идентификатор заголовка
	CLevelHeaderID m_HeaderID;
};

//--------------------------------------------------------------------//

///узел дерева заголовков, содержит слова заголовка
///и если есть детей, подчиненные заголовки
class CHeaderNode : public CConsoleClient,
	public CTreeNode< CHeaderWords<Extracting::Logics::CHeaderLogic::MaxBlockCount>* >
{
public:
	CHeaderNode(CLevelHeaderID* pHeaderID);

	//вставляет загловочный узел 
	void InsertHeaderNode(unsigned int uiWordID, CLevelHeaderID* pHeaderID);
	//возвращает уровень заголовка
	unsigned int GetHeaderLevel(void){return GetDepth();};
	///сериализация в строку
	void ToString(wstring& wszValue, wstring wszOffset);
	///сериализация в строку
	void ToString(wstring& wszValue){
		wstring wszOffset;
		ToString(wszValue, wszOffset);
	};
};

//--------------------------------------------------------------------//

///дерево заголовков документа, 
//содержит узла заголовка в порядкеи их подчинения
class CHeadersTree : public CConsoleClient
{
public:
	///ветка дерева заголовков
	typedef vector<CHeaderNode*> THeadersBranch;

	CHeadersTree(unsigned int uiDocumentIndex);
	virtual ~CHeadersTree(void);

	///строит дерево по коллекции слов с заголовками
	//void BuildTree(CFindResult::TBlockHIDCollection* pBlockHIDCollection);
	///добавляет узлы к дереву
	void AddNodes(vector<unsigned int>* pWordIDCollection, CFindResult::THIDCollection* pHIDCollection);

	///позиционируется на стартовой ветке дерева
	bool MoveFirstBranch(void);
	///переходит к следующей ветке дерева
	bool MoveNextBranch(void);
	///возвращает идентификаторы слов текущей ветки и крайний узел ветки
	CHeaderNode* GetCurrentLastHeaderAndWordsIDs(CHeaderWords<Extracting::Logics::CHeaderLogic::MaxBlockCount>* pHeaderBranchWords);

	///сериализация в строку
	void ToString(wstring& wszValue);

protected:

	///идентификатор документа
	const unsigned int m_uiDocumentIndex;
	///корневой узел, старщий заголовок
	CHeaderNode* m_pRootNode;
	///текущая ветка заголовков
	THeadersBranch m_CurrentBranch;
};

//--------------------------------------------------------------------//

///хранилище интервала c идентификатором заголовка
class CIntervalAndHeader : public CInterval 
{
public:
	CIntervalAndHeader(void){};
	CIntervalAndHeader(unsigned int uiHeadPos, unsigned int uiTailPos, unsigned int uiWeight = 0)
		:CInterval(uiHeadPos, uiTailPos, uiWeight){};

	void operator=(const CIntervalAndHeader& IntervalAndHeader)
	{
		m_HeaderID=IntervalAndHeader.m_HeaderID;
		m_uiHeadPos=IntervalAndHeader.m_uiHeadPos;
		m_uiTailPos=IntervalAndHeader.m_uiTailPos;
		m_uiWeight=IntervalAndHeader.m_uiWeight;
	};

	///создает клона объекта класса
	CIntervalAndHeader* Clone(void)
	{
		CIntervalAndHeader* pIntervalAndHeader=new CIntervalAndHeader(m_uiHeadPos, m_uiTailPos, m_uiWeight);
		pIntervalAndHeader->m_HeaderID=m_HeaderID;
		return pIntervalAndHeader;
	}

	///идентификатор заголовка
	CLevelHeaderID m_HeaderID;
};

//--------------------------------------------------------------------//

}
}
}
}
}
