#pragma once

#include ".\byte_align.h"
#include ".\basis_containers.h"
#include ".\list_containers.h"
#include ".\const.h"

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

///базовое хранилище позиций 
class CPosCnt : public CCompositeValue, public CBinaryContainer, public CUnidirListItem<CPosCnt>
{
public:
	CPosCnt(void):m_ucFreqIncreaseKoef(0), m_uiLastPosition(EMPTY_VALUE){};

	///возвращает коэффициент повышени€ частотности
	unsigned char GetFreqIncreaseKoef(void){return m_ucFreqIncreaseKoef;};
	///устанавливает коэффициент повышени€ частотности
	void SetFreqIncreaseKoef(unsigned char ucValue){m_ucFreqIncreaseKoef=ucValue;};

protected:
	///коэффициент повышени€ частотности, дл€ позиций хранилища 
	///вирутально повышает количество позиций в контайнере
	unsigned char m_ucFreqIncreaseKoef;
	///крайн€€ абсолютна€ позици€
	unsigned int m_uiLastPosition;
};

//--------------------------------------------------------------------//

///хранит колекцию контайнеров позиций в бинарном виде
class CPosBinaryContainer
{
public:
	///контайнер бинарного буфера позиций
	CBinaryBufferContainer* GetPosBinaryContainer(void){return &m_PosBinaryContainer;};

protected:
	///контайнер бинарного буфера используетс€ 
	///дл€ хранени€ бинарного содержимого контайнеров позиций
	CBinaryBufferContainer m_PosBinaryContainer;
};

//--------------------------------------------------------------------//

}
}
}
}
}