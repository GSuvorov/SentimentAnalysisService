#include "StdAfx.h"
#include ".\memory_management.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace MemoryManagement
{

//--------------------------------------------------------------------//

CNdxHeap::CNdxHeap(void)
:m_hHeap(NULL)
{
	m_hHeap=HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
	ISNULL(m_hHeap);	
}

CNdxHeap::~CNdxHeap(void)
{
	HeapDestroy(m_hHeap);
}

void*	CNdxHeap::Alloc(size_t uiSize)
{
	if(ISNULL(m_hHeap)) return NULL;

	return HeapAlloc(m_hHeap, 0, uiSize);
}

void	CNdxHeap::Free(void* pBuffer)
{
	if(ISNULL(pBuffer)) return;
	HeapFree(m_hHeap, 0, pBuffer);
}

//--------------------------------------------------------------------//

CPool::CPool(CNdxHeap* pHeap, size_t uiBlockSize, unsigned int uiInitBlocksCount, const wchar_t* pwszObjectTypeName)
:m_cuiBlockSize(sizeof(SPoolBlock)+uiBlockSize), m_cpHeap(pHeap), m_uiInitBlocksNumber(uiInitBlocksCount),
m_uiCurrExtentSize(0), m_pCurrExtent(NULL), m_pFreeBlockList(NULL), m_pCurrBlock(NULL),
m_uiCurrBlocksNumber(0), m_uiFreeBlocksNumber(0), m_uiCurrExtentsNumber(0), m_wsObjectTypeName(pwszObjectTypeName)
{
	if(m_uiInitBlocksNumber)
		AddExtent(m_uiInitBlocksNumber);
}

CPool::~CPool(void)
{
	ClearExtents();
}

void* CPool::Alloc(void)
{
	SPoolBlock* pBlock=AllocBlock();
   /*  аждый блок начинаетс€ с заголовка. ѕоэтому перед тем, как отдать блок
     * потребителю, необходимо сместить указатель на область, идущую
     * непосредственно за заголовком блока. “ак как размер заголовка равен
     * размеру структуры SPoolBlock, можно просто увеличить указатель на 
     * блок (p) на единицу.
     */
  	pBlock++;
	return pBlock;
}

void CPool::Free(void* p)
{
	FreeBlock((SPoolBlock*)((unsigned char*)p-sizeof(SPoolBlock)));
}

SPoolBlock* CPool::AllocBlock(void)
{
	++m_uiCurrBlocksNumber;

	if(m_pFreeBlockList){
		--m_uiFreeBlocksNumber;

		SPoolBlock* pBlock=m_pFreeBlockList;
		m_pFreeBlockList=m_pFreeBlockList->m_pNextBlock;
		return pBlock;
	}else
		return GetBlock();
}

SPoolBlock* CPool::GetBlock(void)
{
   //провер€ем, не исчерпан ли текущий экстент...
	if(m_pCurrBlock==m_pCurrExtent->m_arrBlocks){
      //если исчерпан, добавл€ем новый экстент
      AddExtent((unsigned int)m_uiCurrExtentSize);
	}

    SPoolBlock* pRet=m_pCurrBlock;
		
	//выставл€ем блоку родительский пул, что можно было потом освобождать
	pRet->m_pBlockPool=this;
    
	// ”меньшаем указатель на текущий блок, сдвига€ его тем самым назад к
    // m_arrBlocks. “ак как размер блока определ€етс€ в байтах, необходимо 
    // привести m_pCurrent к указателю на байт (unsigned char*). „тобы получить
    // lvalue добавл€етс€ знак ссылки Ђ&ї.
	(unsigned char*&)m_pCurrBlock-=m_cuiBlockSize;
    
	return pRet;
}

void CPool::FreeBlock(SPoolBlock* pBlock)
{
	--m_uiCurrBlocksNumber;
	++m_uiFreeBlocksNumber;
	pBlock->m_pNextBlock=m_pFreeBlockList;
	m_pFreeBlockList=pBlock;
}

void CPool::AddExtent(unsigned int uiBlocksCount)
{
	// «анимаем пам€ть под экстент (заголовок и вход€щие в экстент блоки).
	SPoolExtent* pExtent=AllocExtent(uiBlocksCount);

    // ѕомещаем указатель на предыдущий экстент в переменную m_pNextExtent
    // нового экстента.
	pExtent->m_pNextExtent=m_pCurrExtent;
   
    // «аписываем в m_pCurrent указатель на самый последний блок.
    // Ётот указатель рассчитываетс€ как адрес первого блока плюс количество
    // блоков, умноженное на их размер. ѕриведение к unsigned char* требуетс€ из-за 
    // того, что смещение рассчитываетс€ в байтах. ≈сли этого не сделать, по 
    // правилам C++ смещение будет увеличиватьс€ на размер структуры SPoolBlock.
	// - 1 - т.к. мы должны находитьс€ на начале последнего элемента.
	// без это -1 указатель бы указывал бы на конец посденего элемента.
	m_pCurrBlock=(SPoolBlock*)((unsigned char*)(pExtent->m_arrBlocks) 
      + (uiBlocksCount-1) * m_cuiBlockSize);

    // ƒелаем новый экстент текущим. “ем самым заставл€ем выдел€ть 
    // блоки именно из него.
    m_pCurrExtent=pExtent;

    // «апоминаем размер текущего экстента 
    m_uiCurrExtentSize=uiBlocksCount;
}

SPoolExtent* CPool::AllocExtent(unsigned int uiBlocksCount)
{
	if(ISNULL(m_cpHeap)) return NULL;

	++m_uiCurrExtentsNumber;

	return (SPoolExtent*)m_cpHeap->Alloc(sizeof(SPoolExtent)+
		uiBlocksCount * m_cuiBlockSize);
}

void CPool::FreeExtent(SPoolExtent* pExtent)
{
	//ToConsole(L"CPool::FreeExtent");
	
	if(ISNULL(m_cpHeap)) return;

	--m_uiCurrExtentsNumber;

	m_cpHeap->Free(pExtent);
	
	//ToConsole(L"End CPool::FreeExtent");
}

void CPool::ClearExtents(void)
{
	//ToConsole(L"CPool::ClearExtents");

	SPoolExtent* pExtent=m_pCurrExtent;
	SPoolExtent* pTemp;
	while(pExtent){
		pTemp=pExtent->m_pNextExtent;
		FreeExtent(pExtent);
		pExtent=pTemp;
	}

	m_pCurrExtent=NULL;
	m_uiCurrExtentSize=0;

	if(m_uiCurrExtentsNumber) 
		TO_CONSOLE(L"Not all Extents cleared");

	//ToConsole(L"End CPool::ClearExtents");
}

void CPool::Reset(void)
{
	//удал€ем екстенты
	ClearExtents();

	m_pFreeBlockList=NULL;
	m_pCurrBlock=NULL;
	m_uiCurrBlocksNumber=0;
	m_uiFreeBlocksNumber=0;

	//создаем новую
	if(m_uiInitBlocksNumber)
		AddExtent(m_uiInitBlocksNumber);
}

void CPool::ToString(wstring& szValue, unsigned int* puiSize)
{
	wchar_t buf[300];
	wsprintf(buf, L"Pool: \n\tObjects name: %s, \tObjects size: %u, \tAlloc objects count: %u, \tFree objects count: %u, \tAll objects count: %u, \n\tExtents count: %u, \tAprox alloc: %u bytes\n", 
		m_wsObjectTypeName.c_str(),
		m_cuiBlockSize,
		m_uiCurrBlocksNumber, 
		m_uiFreeBlocksNumber,
		m_uiCurrBlocksNumber+m_uiFreeBlocksNumber,
		m_uiCurrExtentsNumber, 
		(m_uiCurrBlocksNumber+m_uiFreeBlocksNumber)*m_cuiBlockSize);
	szValue.append(buf);

	(*puiSize)+=(m_uiCurrBlocksNumber+m_uiFreeBlocksNumber)*m_cuiBlockSize;
}

//--------------------------------------------------------------------//

}
}
}
}
}
