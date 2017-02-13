/*
	Разные приблуды для работы с отладочным выводом
	Используемые макросы

	__DEBUGOUT__ - включен или выключен отладочный вывод
*/
#pragma once
#include "./console.h"

namespace dbg
{
class printer
{
public:
	p(const wchar_t* _val){}
	p(const DWORD* _val){}
	p(const unsigned int _val){}
	p(const int _val){}
	p(const unsigned __int64 _val){}
	p(const __int64 _val){}

private:
	size_t				counter_;
	static const size_t	buffer_size_ = 1024;
	wchar_t*			buffer_[buffer_size_];
};


}

#define	dbreak DebugBreak();

#ifdef __DIAGNOSTIC__
#define	C_	{
#define	_C_
#define	_C	}
#else
#define	C_	{
#define	_C_
#define	_C	}
#endif
