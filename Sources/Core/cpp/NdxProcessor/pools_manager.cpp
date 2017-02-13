#include "StdAfx.h"

#include ".\pools_manager.h"
#include ".\index_cash.h"

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

using namespace SS::Core::NdxSE::NdxProcessor::Containers;

//--------------------------------------------------------------------//

CPoolsManager::CPoolsManager(bool bUseStdHeap)
:m_bUseStdHeap(bUseStdHeap)
{
}

CPoolsManager::~CPoolsManager(void)
{
	for(TPoolsCollection::iterator itPool=m_PoolsCollection.begin(); itPool!=m_PoolsCollection.end(); itPool++){
		if(itPool->second!=NULL){
			delete itPool->second;
		}
	}
}

void CPoolsManager::RegisterClass(const wchar_t* pwszObjectTypeName, size_t uiSize, unsigned int uiInitObjectsCount)
{
	//если используется стандартная куча ниче не регистрируем
	if(m_bUseStdHeap) return;

	TPoolsCollection::iterator itPool=m_PoolsCollection.find(uiSize);
	if(itPool==m_PoolsCollection.end()){
		m_PoolsCollection.insert(
			TPoolsCollection::value_type(uiSize, new CPool(&m_Heap, uiSize, uiInitObjectsCount, pwszObjectTypeName)));
	}
}

void* CPoolsManager::AllocObject(size_t uiSize)
{
	SS::Core::CommonServices::CCriticalSection CriticalSection(&m_CriticalSection);

	//ToConsole(L"CPoolsManager::AllocObject");
	//ToConsole(L"Object Size", uiSize);
	void* p=NULL;
	if(m_bUseStdHeap){
		//создаем стандартно
		p=malloc(uiSize);		
	}else{
		TPoolsCollection::iterator itPool=m_PoolsCollection.find(uiSize);
		if(itPool!=m_PoolsCollection.end()){
			p=itPool->second->Alloc();
		}else{
			TO_CONSOLE(L"No pool for such object");	
			ToConsole(L"Object size", (unsigned int)uiSize);
		}
	}
	//ToConsole(L"End CPoolsManager::AllocObject");

	return p;
}

void CPoolsManager::FreeObject(void* p)
{
	SS::Core::CommonServices::CCriticalSection CriticalSection(&m_CriticalSection);

	//ToConsole(L"CPoolsManager::FreeObject");
	if(ISNULL(p)) return;

	if(m_bUseStdHeap){
		//освобождаем стандартно
		free(p);		
	}else{
		SPoolBlock* pBlock=(SPoolBlock*)((unsigned char*)p-sizeof(SPoolBlock));

		//по блоку определяем родительский пул, освобождаем блок
		pBlock->m_pBlockPool->Free(p);
		//ToConsole(L"FreeObject size", pBlock->m_pBlockPool->GetPoolBlockSize());
	}
	//ToConsole(L"End CPoolsManager::FreeObject");

}

void	CPoolsManager::ResetPools(void)
{
	for(TPoolsCollection::iterator itPool=m_PoolsCollection.begin(); itPool!=m_PoolsCollection.end(); itPool++){
		if(itPool->second!=NULL){
			itPool->second->Reset();
		}
	}
}

void CPoolsManager::View(void)
{
	wstring szValue=L"";
	unsigned int uiTotalSize=0;
	ToConsole(L"//---------PoolsManager:");
	for(TPoolsCollection::iterator itPool=m_PoolsCollection.begin(); itPool!=m_PoolsCollection.end(); itPool++){
		if(itPool->second!=NULL){
			itPool->second->ToString(szValue, &uiTotalSize);
		}
	}
	ToConsole(szValue.c_str());
	ToConsole(L"Total Mem Alloc", uiTotalSize);
	ToConsole(L"//--------End-of-PoolsManager");
}

//--------------------------------------------------------------------//

}
}
}
}
}
