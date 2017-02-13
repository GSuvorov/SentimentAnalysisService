//GlobalSearchDefines.h
#pragma once

namespace SS
{
namespace GlobalSearch
{
	namespace Asp
	{
		///Тэги сетевых пакетов между asp-страницей и сервером	
		static const wchar_t* c_wzPacket		= L"Packet";
		
		static const wchar_t* c_wzPlugin		= L"Plugin";
		static const wchar_t* c_wzPluginName	= L"Name";
		static const wchar_t* c_wzPluginCheck	= L"Check";

		static const wchar_t* c_wzQuery			= L"Query";
		static const wchar_t* c_wzQueryType		= L"Type";
		static const wchar_t* c_wzPluginList	= L"System";
		static const wchar_t* c_wzSearch		= L"User";

		#define MAX_PLUGIN_COUNT 10
	}

	namespace Common
	{
		///Пакет отправляемый сервером и клиентом в конце передачи
		static const wchar_t* c_wzStopPacket	= L"#-#-#";
		///Корневой тег результата разбора плагинами
		static const wchar_t* c_wzResultTag		= L"<Result>";
	}

	namespace PluginResult
	{
		static const wchar_t* c_wzResult		= L"Result";
		static const wchar_t* c_wzPlugin		= L"Plugin";
		static const wchar_t* c_wzPluginName	= L"Name";
		static const wchar_t* c_wzPage			= L"Page";
		static const wchar_t* c_wzAnswer		= L"Answer";
	}

	namespace Linguistics
	{
		static const wchar_t* c_wzCommonResult			= L"CommonResult";
		static const wchar_t* c_wzCommonResultOpenTag	= L"<CommonResult>";
		static const wchar_t* c_wzCommonResultCloseTag	= L"</CommonResult>";
		static const wchar_t* c_wzSearchResult	= L"SearchResult";
		static const wchar_t* c_wzTextBlock		= L"TextBlock";
		static const wchar_t* c_wzTextBlockRev	= L"Rev";
	}
}
}