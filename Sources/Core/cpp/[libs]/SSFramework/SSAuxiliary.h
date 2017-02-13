#ifndef SSAuxiliary_H
#define SSAuxiliary_H
#include "../LikePointer.hpp"

#include "../../ASCInterface/IInterface.h"
#include <objbase.h>


///////////////////////////////////////////////////////////////////////////////
// Модуль содержит вспомогательные функции и макросы системного характера
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Предоставляет более удобный синтаксис для создания функции QueryInterface
// в определении класса
///////////////////////////////////////////////////////////////////////////////
#define BEGIN_QI_MAP() \
HRESULT QueryInterface(REFIID pIID, void** pBase) \
{

/// Необходимый интерфейс наследован
#define QI_MAP_INHERIT_ENTRY( IID, Type ) \
	if(pIID==(IID)) \
		*pBase = static_cast<Type*>(this); \
	else

/// Необходимый интерфейс можно получить путём приведения переменной класса
#define QI_MAP_MEMBER_ENTRY( IID, Type, Member ) \
	if(pIID==(IID)) \
		*pBase = static_cast<Type*>(&Member); \
	else
	
#define END_QI_MAP() \
	{\
		*pBase = NULL;  \
		return E_NOINTERFACE;  \
	}\
	return S_OK;  \
}

#define RELEASE_IMPL() ULONG Release(){ delete this;  return 0; }               


namespace SS { namespace Lib {

///////////////////////////////////////////////////////////////////////////////
//Класс позволяет создавать ILoadManager в необходимом количестве
//После использования у созданых ILoadManager'ов необходимо вызвать 
//Release, благодаря использованию auto_ptr_ex, вызывается автоматически
///////////////////////////////////////////////////////////////////////////////
class CLoadManagerCreater
{
   typedef SS::Interface::IBase TIBase;
   typedef HRESULT (*CREATE_INSTANCE)(const GUID* pGuid, void** pBase);

public:
   typedef SS::Interface::Core::ResourceManagers::ILoadManager TLoadManager;
   typedef auto_ptr_ex<TLoadManager, auto_ptr_ex_release_strategy> TAutoPtr;

private:
   HMODULE m_hLoadManager;
   CREATE_INSTANCE m_CreateInstanceFunc;

private:
   std::basic_string<TCHAR> GetStringFromGUID( REFGUID guid ) const
   {
      WCHAR Str[40];
      std::basic_string<TCHAR> Tmp;

      APL_CHECK( StringFromGUID2(guid, Str, APL_ARRSIZE(Str)) );
      
      return Convert( Str, Tmp);
   }

public:
   explicit CLoadManagerCreater( const std::basic_string<TCHAR> &FileName = _T("LoadManager.dll") );
   ~CLoadManagerCreater();

   //Создать LoadManager
   //Необходимо вызывать Release после использоания, (вызывается автоматически благодаря auto_ptr_ex)
   TAutoPtr Create() const;   

   //Создание произвольного интерфейса
   //Если pLoadManager != 0 то у интерфейса вызывается SetLoadManager
   template< class T >
   auto_ptr_ex<T, auto_ptr_ex_release_strategy> GreateInstance( const CLSID &ClassID, const IID &InterfaceID, TLoadManager *pLoadManager = NULL ) const
   {
      TIBase *pBase = 0;
      T *pT = 0;

      if( (m_CreateInstanceFunc)(&ClassID, (void**)&pBase) != S_OK || pBase == NULL )
         APL_THROW( _T("Ошибка при получении CLSID ") << GetStringFromGUID(ClassID) );

      if( pBase->QueryInterface(InterfaceID, (void**)&pT) != S_OK || pT == NULL )
         APL_THROW( _T("Ошибка при получении IID ") << GetStringFromGUID(InterfaceID) );
      
      if( pLoadManager != 0 )
         pT->SetLoadManager( pLoadManager );

      return auto_ptr_ex<T, auto_ptr_ex_release_strategy>( pT );
   }
};
///////////////////////////////////////////////////////////////////////////////

}} //namespace SS::Lib 

#endif