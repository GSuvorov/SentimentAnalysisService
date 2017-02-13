#pragma once

// {507751B9-555B-4b47-BCBF-87FFDD239277}
static const GUID CLSID_QueryDeformer = 
{ 0x507751b9, 0x555b, 0x4b47, { 0xbc, 0xbf, 0x87, 0xff, 0xdd, 0x23, 0x92, 0x77 } };

// {435F7CB4-BA4F-403e-95D1-09953694D6BD}
static const GUID IID_QueryDeformer = 
{ 0x435f7cb4, 0xba4f, 0x403e, { 0x95, 0xd1, 0x9, 0x95, 0x36, 0x94, 0xd6, 0xbd } };

#include "IBlackBoxItems.h"
#include "ICommon.h"


namespace SS
{
namespace Interface
{
namespace QueryDeformer
{

class IQueryDeformer : 
		public virtual SS::Interface::IBase,
		public virtual SS::Interface::Core::Common::CAnalyseParams
{
public:
	virtual void TransformQuery(SS::Interface::Core::BlackBox::IText* pText) = 0;
};

}
}
}
