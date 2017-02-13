#pragma once

#include "..\ASCInterface\IInterface.h"
#include <string>

// {9494A659-DD50-4b13-BBF7-00144760238D}
static const GUID CLSID_PluginManager = 
{ 0x9494a659, 0xdd50, 0x4b13, { 0xbb, 0xf7, 0x0, 0x14, 0x47, 0x60, 0x23, 0x8d } };
// {4AA69700-6D93-4cb6-BBE1-B02F97B6F26D}
static const GUID IID_PluginManager = 
{ 0x4aa69700, 0x6d93, 0x4cb6, { 0xbb, 0xe1, 0xb0, 0x2f, 0x97, 0xb6, 0xf2, 0x6d } };

// {E519668C-5C57-4584-8285-D8A58B1DC720}
static const GUID CLSID_DocPlugin = 
{ 0xe519668c, 0x5c57, 0x4584, { 0x82, 0x85, 0xd8, 0xa5, 0x8b, 0x1d, 0xc7, 0x20 } };
// {F997B4EC-FDF6-4761-BE91-3F5B6922489D}
static const GUID IID_DocPlugin = 
{ 0xf997b4ec, 0xfdf6, 0x4761, { 0xbe, 0x91, 0x3f, 0x5b, 0x69, 0x22, 0x48, 0x9d } };


namespace SS
{
namespace Interface
{
namespace DocumentPlugins
{

enum EPluginBlockType
{
	pbtEnd,
	pbtText,
	pbtTable,
	pbtTitle,
};

class IDocParser
{
public:
	virtual bool Parse(LPCWSTR szFilePath) = 0;
	virtual void GetEntireText(std::wstring* pText) = 0;
	virtual EPluginBlockType GetFirstBlock(std::wstring* pText) = 0;
	virtual EPluginBlockType GetNextBlock(std::wstring* pText) = 0;
};

class IDocPlugin : 
		public virtual IBase,
		public virtual IDocParser
{
public:
	virtual LPCWSTR* GetExtensionList() = 0;
	virtual LPCWSTR GetName() = 0;
	virtual void Select(bool bSet) = 0;
	virtual bool IsSelected() = 0;
};

class IPluginManager : public virtual IBase
{
public:
	virtual IDocParser* GetParser() = 0;
	virtual void LoadPlugins(LPCWSTR szPluginPath) = 0;
	virtual IDocPlugin* GetFirstPlugin() = 0;
	virtual IDocPlugin* GetNextPlugin() = 0;
};

}
}
}
