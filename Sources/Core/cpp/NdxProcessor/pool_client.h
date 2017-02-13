#pragma once
#include ".\global_pool.h"

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

///клиент пула объектов
class CPoolClient
{
public:
	///перегрузка выделения памяти для объекта класса, память выделяется пулом
	void* operator new(size_t size)
	{
		return g_PoolsManager.AllocObject(size);
	}

	///перегрузка освобождения памяти для объекта класса, память освобождается пулом
	void operator delete(void* p, size_t size)
	{
		return g_PoolsManager.FreeObject(p);
	}
};

//--------------------------------------------------------------------//

}
}
}
}
}