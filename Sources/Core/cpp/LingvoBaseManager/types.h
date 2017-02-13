#pragma once

#include "..\ASSInterface\ILingvoBaseManager.h"
#include "..\ASCInterface\idbms.h"

namespace SS
{
namespace Core
{
namespace ResourceManagers
{
namespace Types
{

	/*! \typedef TLSubConnections
	*  \brief   Описывает список подсоединений
	*/
	typedef std::list<SS::Interface::Core::ResourceManagers::ISubConnection*> TLSubConnections;
	/*! \typedef THMSubConnections
	*  \brief    Описывает карту соответствия между именем типа базы и списком подсоединений
	*/
	typedef stdext::hash_map<std::wstring, TLSubConnections> THMSubConnections;
	/*! \typedef TLConnections
	*  \brief    Описывает список соединений SQL - группы
	*/
	typedef std::list<SS::Interface::Core::ResourceManagers::IConnection**> TLConnections;
	/*! \typedef THMConnections
	*  \brief    Описывает карту соответствия между именем типа базы и списком ссылок на указатели на соединение SQL-группы
	*/
	typedef stdext::hash_map<std::wstring, TLConnections> THMConnections;

	/*! \typedef TLDBMSConnections
	*  \brief    Описывает список соединений DBMS - группы
	*/
	typedef std::list<SS::Interface::Core::DBMS::IDataBase**> TLDBMSConnections;
	/*! \typedef THMDBMSConnections
	*  \brief    Описывает карту соответствия между именем типа базы и списком ссылок на указатели на соединение DBMS-группы
	*/
	typedef stdext::hash_map<std::wstring, TLDBMSConnections> THMDBMSConnections;
}
}
}
}