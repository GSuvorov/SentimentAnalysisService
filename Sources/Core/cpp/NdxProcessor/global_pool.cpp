#include "StdAfx.h"
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

//создаем глобальный менеджер пулов, в конструкторе 
//флагом указывается способ создания объектов 
//false - при помощи пула, true - при помощий стандартных new-delete
CPoolsManager g_PoolsManager(false/*true*/);

//--------------------------------------------------------------------//

}
}
}
}
}