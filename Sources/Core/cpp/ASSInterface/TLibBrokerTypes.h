#pragma once

//#include "./ILingvobasemanager.h"


//namespace Containers
//{
//	class CMorphoInfo;
//}

template <class T>
class CPreparedCommand : public CCommand<T>/* ,
	public SS::Interface::Core::ResourceManagers::ISubConnection*/
{

public:

	CPreparedCommand() : m_IsOpen(false) {};
	void Prepare(const CSession& session)
	{
		if(!m_IsOpen)
		{
			GetDefaultCommand(&m_wszCommand);
			Create(session,m_wszCommand);
			CCommand<T>::Prepare(0);
			m_IsOpen=true;
		};
	}


	////Открываем SubConnection по типу базы (индексная, русский словарь, английский и т.п.
	////и ссылке на сщность, хранящую в себе все соединения
	//bool SubOpen(SS::Interface::Core::ResourceManagers::IConnection* pConnection)
	//{
	//	if(!m_IsOpen)
	//	{
	//		Prepare(*pConnection->GetSession());
	//		m_IsOpen=true;
	//	};
	//	return true;
	//};

	//void SubClose()
	//{
	//	if(m_IsOpen)
	//	{
	//		CCommand<T>::ReleaseCommand();
	//		CCommand<T>::Close();
	//		m_IsOpen=false;
	//	};
	//};

private:
	bool m_IsOpen;	
	LPCWSTR m_wszCommand;
};

