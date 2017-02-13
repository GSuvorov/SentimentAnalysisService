//IGSPluginManager.h
#pragma once
#include "..\ASCInterface\IInterface.h"

// {6E094AF9-18A2-4e4c-96CD-B1F67CFCA5A4}
static const GUID CLSID_GSPluginManager = 
{ 0x6e094af9, 0x18a2, 0x4e4c, { 0x96, 0xcd, 0xb1, 0xf6, 0x7c, 0xfc, 0xa5, 0xa4 } };

// {CACEFA78-D6D6-4a77-BF75-88ECE5E9D92C}
static const GUID IID_GSPluginManager = 
{ 0xcacefa78, 0xd6d6, 0x4a77, { 0xbf, 0x75, 0x88, 0xec, 0xe5, 0xe9, 0xd9, 0x2c } };

// {176A2028-488A-4f9e-8657-EF540515B08A}
static const GUID CLSID_GSPlugin = 
{ 0x176a2028, 0x488a, 0x4f9e, { 0x86, 0x57, 0xef, 0x54, 0x5, 0x15, 0xb0, 0x8a } };

// {2635433F-32B0-424e-8DA0-8C4746491E94}
static const GUID IID_GSPlugin = 
{ 0x2635433f, 0x32b0, 0x424e, { 0x8d, 0xa0, 0x8c, 0x47, 0x46, 0x49, 0x1e, 0x94 } };

namespace SS
{
namespace Interface
{
namespace GSPlugins
{
	///Предоставляет набор методов по обработке документа
	class IGSPluginParser
	{
	public:
		IGSPluginParser(){};
		virtual ~IGSPluginParser(){};
	public:
		///Анализирует документ wzSearchString
		virtual bool Parse(const wchar_t* wzSearchString) = 0;
		///Возвращает текст из документа
		virtual void GetXML(std::wstring* pwText) = 0;
		///Возвращает html страницу из документа
		virtual void GetHTML(std::wstring* pwHTML) = 0;
		///Установить необходимое количество результатов
		virtual void SetResultCount(unsigned int uiResultCount) = 0;
		///Показать заданное количество результатов
		virtual unsigned int GetResultCount() = 0;
	};

	///Интерфейс gs-плагина
	class IGSPlugin : virtual public  IGSPluginParser,
							  public  IBase
	{
	public:
		IGSPlugin(){};
		virtual ~IGSPlugin(){};
	public:
		///Получить имя плагина
		virtual const wchar_t* GetName() = 0;
		///Выбрать плагин
		virtual void Select(bool bSet) = 0;
		///Проверить статус
		virtual bool IsSelected() = 0;
	};
	
	///Интерфейс для управления gs-плагинами
	class IGSPluginManager : public IBase
	{
	public:
		IGSPluginManager(){};
		virtual ~IGSPluginManager(){};
	public:
		///Загрузить плагины
		virtual bool LoadPlugins(const wchar_t*  wszPluginsPath = NULL) = 0;
		///Получить первый
		virtual IGSPlugin* GetFirstPlugin() = 0;
		///Получить следующий
		virtual IGSPlugin* GetNextPlugin() = 0;
	};
}	
}
}




