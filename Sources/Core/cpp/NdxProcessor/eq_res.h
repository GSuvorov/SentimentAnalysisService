#pragma once
#include ".\eq.h"
#include ".\arg_res.h"

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

///формируемая формула результатов с весами
class CResultEquation : public CEquation<CResultArgument, CArgument>
{
public:
	CResultEquation(CEvalFactory* pEvalFactory);
	virtual ~CResultEquation(void);
	
};

//--------------------------------------------------------------------//

}
}
}
}
}