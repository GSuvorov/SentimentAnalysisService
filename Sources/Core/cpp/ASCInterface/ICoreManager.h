#pragma once

#include "IInterface.h"


namespace SS
{
namespace Interface
{
namespace Core
{
	class ICoreManager : public virtual IBase
	{
	public:

		IBaseManager* IBaseManager() = 0;

		ILoadManager* ILoadManager() = 0;

		ILingvoBaseManager* ILingvoBaseManager() = 0;
	};
}
}
}
