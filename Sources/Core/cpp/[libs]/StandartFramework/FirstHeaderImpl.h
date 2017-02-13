#ifndef FirstHeaderImplH
#define FirstHeaderImplH

#define APL_THROW( arg ){                                                                       \
   std::basic_stringstream<TCHAR> Stream__; Stream__ << arg;                                    \
   throw std::exception( ConvertToBuf<std::string>(Stream__.str()).c_str() );                   \
}

#define APL_LOG( arg ) DEBUG_MSG( arg << std::endl << _T("\t") << __WFUNCTION__ << std::endl )                                                                         \

#include "Auxiliary.h" //Для функции Convert

#endif