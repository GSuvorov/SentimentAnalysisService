#pragma once

// {B8394127-C767-4759-BE07-9B1A0B7DB39F}
static const GUID IID_SearchResultEx = 
{ 0xb8394127, 0xc767, 0x4759, { 0xbe, 0x7, 0x9b, 0x1a, 0xb, 0x7d, 0xb3, 0x9f } };

#include "..\ASCInterface\ICommonContainers.h"

namespace SS
{
namespace Interface
{
namespace Core
{
namespace CommonContainers
{

class ISearchResultEx : public ISearchResult
{
public:
	virtual ~ISearchResultEx(){};
public:
	virtual void AddTextBlock(ITextBlock * pTextBlock) = 0;

};

}
}
}
}