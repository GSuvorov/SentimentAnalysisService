#pragma once

#include ".\memory_management.h"
#include "..\ASCInterface\services.h"

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

///менеджер пулов - содержит коллекцию для объектов зарегистрированных размеров
class CPoolsManager
{
public:
	typedef map<size_t, CPool*> TPoolsCollection;
	
	CPoolsManager(bool bUseStdHeap);
	virtual ~CPoolsManager(void);
	
	///защита от копирования
	CPoolsManager(const CPoolsManager&){};
	///защита от копирования
	CPoolsManager& operator==(const CPoolsManager&){return *this;};
	
	///регистрация размера объектов
	void	RegisterClass(const wchar_t* pwszObjectTypeName, size_t uiSize, unsigned int uiInitObjectsCount = 1000);
	///алокирует объект
	void*	AllocObject(size_t uiSize);
	///освобождает объект
	void	FreeObject(void* p);
	///очищает пулы с объектами
	void	ResetPools(void);
	///отображает на консоль
	void View(void);

protected:
	///критическая секция для синхронизации нескольких потоков
	SS::Core::CommonServices::CManagerLifeTimeForCriticalSection m_CriticalSection;
	///коллекция пулов объектов
	TPoolsCollection m_PoolsCollection;
	///рабочая куча менеджера
	CNdxHeap m_Heap;
	///флаг использования стандартной кучи
	bool m_bUseStdHeap;
};

//--------------------------------------------------------------------//

}
}
}
}
}