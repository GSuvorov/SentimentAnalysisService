#ifndef FirstHeaderImplH
#define FirstHeaderImplH

//SS_THROW не всегда кидает исключения, в противном случае она просто игнорирует комманду SS_THROW
//Такое поведение врядле можно назвать корретным. В таких случаях кидается стандратный тип исключения
//std::exception 
#include "../ASCInterface/defines.h"

SET_LOAD_MANAGER_DEFINE;
USES_LOAD_MANAGER;
///////////////////////////////////////////////////////////////////////////////
// !!! Необходимо в одном из модулей CPP определить SET_LOAD_MANAGER_IMPLEMENTATION; !!!
///////////////////////////////////////////////////////////////////////////////

#ifndef USES_IBASE
#define APL_THROW( arg ){                                                                       \
   std::basic_stringstream<TCHAR> Stream__; Stream__ << arg;                                    \
   if( !(!spLoadManager ) ) {                                                                   \
      SS_THROW( const_cast<wchar_t*>(ConvertToBuf<std::wstring>(Stream__.str()).c_str()) );     \
   } else {                                                                                     \
      throw std::exception( ConvertToBuf<std::string>(Stream__.str()).c_str() );                \
   }                                                                                            \
}
#else
#define APL_THROW( arg ){                                                                       \
   std::basic_stringstream<TCHAR> Stream__; Stream__ << arg;                                    \
   SS_THROW( const_cast<wchar_t*>(ConvertToBuf<std::wstring>(Stream__.str()).c_str()) );        \
}
#endif

#define APL_LOG( arg ){                                                                         \
   std::basic_stringstream<TCHAR> Stream__; Stream__ << arg <<                                  \
       std::endl << _T("\t") << __WFUNCTION__ << std::endl;                                     \
   SAVE_LOG(SS_WARNING AND const_cast<wchar_t*>(                                                \
       ConvertToBuf<std::wstring>(Stream__.str()).c_str()) );                                   \
}

#include "Auxiliary.h" //Для функции Convert

#endif