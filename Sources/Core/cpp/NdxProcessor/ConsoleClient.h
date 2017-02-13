#pragma once

#include ".\console.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{

//--------------------------------------------------------------------//

///абстрактная поддержка работы с консолью
class CConsoleClient
{
public:
	///сериализация в строку
	virtual void ToString(wstring& wszValue, wstring wszOffset){TO_CONSOLE(L"Not overriden!");};
	///сериализация в строку
	virtual void ToString(wstring& wszValue){TO_CONSOLE(L"Not overriden!");};
	///отображает содержимое объекта класса
	virtual void View(void)
	{
		wstring wszValue;
		ToString(wszValue);
		ToConsole(wszValue.c_str());
	};
};

//--------------------------------------------------------------------//

}
}
}
}