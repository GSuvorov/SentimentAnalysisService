#pragma once


#include "TListEnumerator.h"
#include <string>

namespace SS
{
namespace Core
{
namespace Types
{
namespace Search
{

	/*! \typedef TAbsentWords
	*  \brief    Определяет коллекцию незнакомых слов
	*/
	typedef SS::Core::Types::TListEnumerator<std::string> TAbsentWords;
}
}
}
}
