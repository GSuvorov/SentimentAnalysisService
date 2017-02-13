#pragma once
#include "..\ASSInterface\ILingvoBaseManager.h"
#include <atldbcli.h>


namespace SS
{
namespace Core
{
namespace ResourceManagers
{
   class CConnectionManager: public SS::Interface::Core::ResourceManagers::IConnectionManager
   {
      typedef std::vector<SS::Interface::Core::Common::IConnection *> TConnections;
   
   private:
      TConnections m_Connections;

   public:
      void Register(SS::Interface::Core::Common::IConnection * pConnection);
      bool Open();
      bool Close();


   public:

      RELEASE_IMPL()

      BEGIN_QI_MAP()
         QI_MAP_INHERIT_ENTRY( IID_ConnectionManager, SS::Interface::Core::ResourceManagers::IConnectionManager )
         //QI_MAP_INHERIT_ENTRY( IID_Connection, SS::Interface::Core::Common::IConnection )
         QI_MAP_INHERIT_ENTRY( IID_Base, SS::Interface::IBase )
      END_QI_MAP()
   };

   
}
}
}
