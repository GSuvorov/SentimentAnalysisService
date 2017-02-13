#pragma once

#include ".\console.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxBaseManager
{

//--------------------------------------------------------------------//

///абстрактная поддержка работы с консолью
class CConsoleClient
{
public:
	///сериализация в строку
	virtual void ToString(string& szValue, string szOffset){TO_CONSOLE("Not overriden!");};
	///сериализация в строку
	virtual void ToString(string& szValue){TO_CONSOLE("Not overriden!");};
	///отображает содержимое объекта класса
	virtual void View(void){TO_CONSOLE("Not overriden!");};
};

//--------------------------------------------------------------------//

}
}
}
}