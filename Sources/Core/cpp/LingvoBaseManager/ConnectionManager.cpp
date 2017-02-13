#include "StdAfx.h"
#include ".\connectionmanager.h"
#include <sqloledb.h>

namespace SS
{
namespace Core
{
namespace ResourceManagers
{

   void CConnectionManager::Register(SS::Interface::Core::Common::IConnection * pConnection)
   {
      SS_TRY
      {
         m_Connections.push_back(pConnection);
      }
      SS_CATCH( L"" )
   }

   bool CConnectionManager::Open()
   {
      bool RetVal = true;
      
      SS_TRY
      {
         for( TConnections::iterator I = m_Connections.begin(); I != m_Connections.end(); ++I )
            if( !(*I)->Open() )
               RetVal = false;
      }
      SS_CATCH( L"" )

      return RetVal;
   }
   
   bool CConnectionManager::Close()
   {
      bool RetVal = true;

      SS_TRY
      {
         for( TConnections::iterator I = m_Connections.begin(); I != m_Connections.end(); ++I )
            if( !(*I)->Close() )
               RetVal = false;
      }
      SS_CATCH( L"" )

      return RetVal;
   }

}
}
}