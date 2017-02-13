#include "StdAfx.h"
#include ".\arg_res_int.h"

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

using namespace SS::Core::NdxSE::NdxProcessor::Containers;

//--------------------------------------------------------------------//

CResultArgumentWithIntervals::CResultArgumentWithIntervals(unsigned int uiArgumentID, float fKoeff)
:CResultArgument(uiArgumentID, fKoeff)
{
}

CResultArgumentWithIntervals::~CResultArgumentWithIntervals(void)
{
	for(TResultIntervalCollection::iterator itRIC=m_ResultIntervalCollection.begin(); itRIC!=m_ResultIntervalCollection.end(); itRIC++){
		if((*itRIC)){
			for(unsigned int i=0; i<(*itRIC)->size(); i++){
				if((*itRIC)->at(i)) delete (*itRIC)->at(i);
			}
			delete (*itRIC);
			(*itRIC)=NULL;
		}
	}

	m_ResultIntervalCollection.clear();
}

//--------------------------------------------------------------------//

}
}
}
}
}