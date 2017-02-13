#pragma once

#include ".\arg_res.h"
#include ".\res_containers.h"

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

//--------------------------------------------------------------------//

///аргумент результат для поиска в заголовках, 
///содержит коллекцию блоков с идентификаторами заголовков
class CResultArgument_HID : public CResultArgument
{
public:
	typedef vector<Containers::CFindResult::TBlockHIDCollection*> TBlocksCollection;

	CResultArgument_HID(unsigned int uiArgumentID = SS::Core::NdxSE::NdxProcessor::EMPTY_VALUE, float fKoeff = 1.0);
	virtual ~CResultArgument_HID(void);
	
	///добавляет очередную коллекцию блоков к результату
	void AddResultBlocks(void);
	///добавляет иденфтикатор заголовка по идентификатору блока
	void AddIndexHID(unsigned int uiIndex, unsigned int uiHeaderID);
	///возвращает коллекцию блоков результата
	TBlocksCollection* GetResultBlocksCollection(void){return &m_BlockHIDCollection;};

protected:

	///коллекция идентификаторов заголовков по по блокам
	TBlocksCollection m_BlockHIDCollection;
};

//--------------------------------------------------------------------//

}
}
}
}
}