//ICacheModule.h
#pragma once
#include "..\ASCInterface\IInterface.h"

// {6E00C5C3-FA3D-445b-8B39-463A933818F2}
static const GUID CLSID_GlobalSearchBase = 
{ 0x6e00c5c3, 0xfa3d, 0x445b, { 0x8b, 0x39, 0x46, 0x3a, 0x93, 0x38, 0x18, 0xf2 } };

// {0BA3750D-9F6B-4820-A5B7-DF0675430C15}
static const GUID IID_GlobalSearchBase = 
{ 0xba3750d, 0x9f6b, 0x4820, { 0xa5, 0xb7, 0xdf, 0x6, 0x75, 0x43, 0xc, 0x15 } };

namespace SS
{
namespace Interface
{
namespace Cache
{
	///База данных для результатов глобального поиска
	class IGlobalSearchBase : public IBase
	{
	public:
		IGlobalSearchBase(){};
		virtual ~IGlobalSearchBase(){};
		///Добавить запросы
		virtual void AddQuery(/*in*/const wchar_t* wzQuery, /*in*/const wchar_t* wzSemanticQuery, /*out*/unsigned int & uiSemanticQueryID) = 0;
		///Добавить результат работы плагина
		virtual void AddPluginResult(unsigned int uiSemanticQueryID, const wchar_t* wzPluginName, const wchar_t* wzPluginResult) = 0;
		///Получить результат плагина
		virtual const wchar_t* GetPluginResult(unsigned int uiSemanticQueryID, const wchar_t* wzPluginName) = 0;
		///Очистить базу
		virtual void Clear() = 0;
	};
}
}
}