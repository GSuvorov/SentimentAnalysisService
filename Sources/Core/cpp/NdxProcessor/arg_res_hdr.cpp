#include "StdAfx.h"
#include ".\arg_res_hdr.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Extracting
{

//--------------------------------------------------------------------//

CResultArgument_HID::CResultArgument_HID(unsigned int uiArgumentID, float fKoeff)
:CResultArgument(uiArgumentID, fKoeff)
{
}

CResultArgument_HID::~CResultArgument_HID(void)
{
	Containers::CFindResult::TBlockHIDCollection* pBlockHIDCollection;
	for(unsigned int i=0; i<m_BlockHIDCollection.size(); i++){
		pBlockHIDCollection=m_BlockHIDCollection[i];
		if(pBlockHIDCollection){
			Containers::CFindResult::TBlockHIDCollection::iterator itHIDCollection=pBlockHIDCollection->begin();
			while(itHIDCollection!=pBlockHIDCollection->end()){
				if(itHIDCollection->second)
					delete itHIDCollection->second;
				itHIDCollection++;
			}
			delete pBlockHIDCollection;
		}
	}
	m_BlockHIDCollection.clear();
}

void CResultArgument_HID::AddResultBlocks(void)
{
	m_BlockHIDCollection.push_back(new Containers::CFindResult::TBlockHIDCollection());
}

void CResultArgument_HID::AddIndexHID(unsigned int uiIndex, unsigned int uiHeaderID)
{
	if(uiIndex==EMPTY_VALUE) return;
	if(m_BlockHIDCollection.empty()) return;

	Containers::CFindResult::TBlockHIDCollection* pBlockHIDCollection=m_BlockHIDCollection.back();
	if(ISNULL(pBlockHIDCollection)) return;
	
	Containers::CFindResult::TBlockHIDCollection::iterator itHIDCollection=pBlockHIDCollection->find(uiIndex);
	if(itHIDCollection==pBlockHIDCollection->end()){
		itHIDCollection=pBlockHIDCollection->insert(
			Containers::CFindResult::TBlockHIDCollection::value_type(uiIndex, 
				new Containers::CFindResult::THIDCollection())).first;
	}

	itHIDCollection->second->push_back(uiHeaderID);			
}

//--------------------------------------------------------------------//

}
}
}
}
}