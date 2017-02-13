#pragma once

#ifndef YYSTYPE
#	define YYSTYPE wstring
#endif
#include "ConfigReader.tab.h"

int ParseTranslitConfig(void* pParam);
