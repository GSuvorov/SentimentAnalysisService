#pragma once

// {2D135352-2CD1-44ee-8D16-136E958B9ABF}
static const GUID CLSID_Translit = 
{ 0x2d135352, 0x2cd1, 0x44ee, { 0x8d, 0x16, 0x13, 0x6e, 0x95, 0x8b, 0x9a, 0xbf } };
// {675A3CCA-E9E8-4af9-B8A7-5FDB8B68B3A6}
static const GUID IID_Translit = 
{ 0x675a3cca, 0xe9e8, 0x4af9, { 0xb8, 0xa7, 0x5f, 0xdb, 0x8b, 0x68, 0xb3, 0xa6 } };

#include "ICommon.h"
#include <string>


namespace SS
{
namespace Interface
{

class ITranslit : public virtual SS::Interface::IBase
{
public:
	virtual void Rus2Lat(LPCWSTR szText, std::wstring* pResult) = 0;
	virtual void Lat2Rus(LPCWSTR szText, std::wstring* pResult) = 0;
	virtual void CheckOrthography(LPCWSTR szText, std::wstring* pResult) = 0;
};

}
}
