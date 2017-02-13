//DetectWordsGroups.cpp

#include "StdAfx.h"
#include ".\detectwordsgroups.h"

//Конструктор
SS::SyntaxConstructions::SQL::CDetectWordsGroups::CDetectWordsGroups(wchar_t* szBaseName) : SS::CConnectSQL(szBaseName)
{
}

//Создает группу слов для детектирования
bool SS::SyntaxConstructions::SQL::CDetectWordsGroups::CreateDetectWordsGroup(
													SS::SyntaxConstructions::Types::TDetectWordsGroup & oDetectGroup)
{
	SS_TRY
	{		
		using namespace SS::SyntaxConstructions::SQL::SQL_ACCESS::Syntax;
		//Проверяем существует ли группа с таким именем
		CPreparedSDetectWordsGroupsByDetectWordsGroupName oPreparedSDetectWordsGroupsByDetectWordsGroupName;	
		oPreparedSDetectWordsGroupsByDetectWordsGroupName.Prepare(*m_pConnection->GetSession());		
		oPreparedSDetectWordsGroupsByDetectWordsGroupName.SetDetectWordsGroupName(m_wsDetectWordsGroupName);		
		if (oPreparedSDetectWordsGroupsByDetectWordsGroupName.Open() != S_OK)
		{
			oPreparedSDetectWordsGroupsByDetectWordsGroupName.Close();
			SS_THROW(L"Операция завершена неудачно: Не удалось выполнить запрос к базе по названию группы");
		}
		else if (oPreparedSDetectWordsGroupsByDetectWordsGroupName.MoveFirst() == S_OK)
		{
			//Такая группа уже существует
			oPreparedSDetectWordsGroupsByDetectWordsGroupName.Close();
			SS_THROW(L"Операция завершена неудачно: группа слов для детектирования с таким именем уже существует");
		}
		else oPreparedSDetectWordsGroupsByDetectWordsGroupName.Close();

		//Выполняем сохранение данных
		CPreparedIDetectWordsGroups oPreparedIDetectWordsGroups;
		oPreparedIDetectWordsGroups.Prepare(*m_pConnection->GetSession());
		oPreparedIDetectWordsGroups.SetDetectWordsGroupName(m_wsDetectWordsGroupName);		

		for (SS::SyntaxConstructions::Types::TDetectWordsGroup::iterator iter = oDetectGroup.begin();
			 iter != oDetectGroup.end(); ++iter)
		{
			oPreparedIDetectWordsGroups.SetDetectWord((*iter));		
			//Не удалось выполнить сохранение
			if(oPreparedIDetectWordsGroups.Open() != S_OK)
			{
				oPreparedIDetectWordsGroups.Close();
				//Удаляем в базе все то, что успели занести
				DeleteDetectWordsGroup();
				SS_THROW(L"Операция завершена неудачно: Ошибка при внесении информации о группе");
			}
			//Закрываем соединение
			oPreparedIDetectWordsGroups.Close();
		}
	}
	SS_CATCH(L"")
	return true;
}

//Сохраняет в бинарный вид структуры данных 
bool SS::SyntaxConstructions::SQL::CDetectWordsGroups::ToBinary()
{
	SS_TRY
	{
		std::wstring wDataTableName = L"DetectWordsGroups";		
		m_pDataBase->MoveToDataTable(wDataTableName.c_str());
		SS::Interface::Core::DBMS::IDataTable* pDTable = m_pDataBase->OpenDataTable();
		
		if ( (pDTable==NULL) || (!pDTable->IsOpen()) )
		{ SS_THROW(L"ошибка при открытии таблицы Combinatory"); }
		
		//Очистить таблицу
		pDTable->Clear();

		SS::SyntaxConstructions::Types::SDetectWordGroupName oDetectWordGroupName;
		void* TableRecord[] = 
		{
			&oDetectWordGroupName.m_wsDetectWordGroupName,
			&oDetectWordGroupName.m_wsDetectWord			
		};
		
		if (pDTable->AddInit(TableRecord) != S_OK)  
		{ SS_THROW(L"ошибка AddInit для таблицы Combinatory"); }	

		using namespace SS::SyntaxConstructions::SQL::SQL_ACCESS::Syntax;
		//Проверяем существует ли группа с таким именем
		CPreparedSDetectWordsGroups oPrepared;	
		oPrepared.Prepare(*m_pConnection->GetSession());		
		
		if(oPrepared.Open() != S_OK)
		{
			oPrepared.Close();
			SS_THROW(L"Операция завершена неудачно: Не удалось выполнить запрос к базе по названию группы.");
		}
		
		if (oPrepared.MoveFirst() != S_OK)
		{
			oPrepared.Close();
			SAVE_LOG( SS_MESSAGE AND __WFUNCTION__ AND L"Операция завершена неудачно: запрос к базе вернул пустой список слов для детектирования");
			oPrepared.Close();
			pDTable->Close();
			return true;
		} 
		do
		{
			oPrepared.GetDetectWordsGroupName(oDetectWordGroupName.m_wsDetectWordGroupName);
			oPrepared.GetDetectWord(oDetectWordGroupName.m_wsDetectWord);
			pDTable->Add();
		}
		while (oPrepared.MoveNext() == S_OK);
		oPrepared.Close();
		pDTable->Close();
	}
	SS_CATCH(L"")
	return true;
}

//Удаляет текущую группу слов для детектирования
void SS::SyntaxConstructions::SQL::CDetectWordsGroups::DeleteDetectWordsGroup()
{
	SS_TRY
	{
		using namespace SS::SyntaxConstructions::SQL::SQL_ACCESS::Syntax;
		//Удаляем группу
		CPreparedDDetectWordsGroups oPreparedDDetectWordsGroups;
		oPreparedDDetectWordsGroups.Prepare(*m_pConnection->GetSession());
		oPreparedDDetectWordsGroups.SetDetectWordsGroupName(m_wsDetectWordsGroupName);		
		oPreparedDDetectWordsGroups.Open();
		oPreparedDDetectWordsGroups.Close();
	}
	SS_CATCH(L"")
}

//Возвращает все элементы группы слов для детектирования
void SS::SyntaxConstructions::SQL::CDetectWordsGroups::GetWords(
											SS::SyntaxConstructions::Types::TDetectWordsGroup & oDetectGroup)
{
	SS_TRY
	{
		using namespace SS::SyntaxConstructions::SQL::SQL_ACCESS::Syntax;
		//Проверяем существует ли группа с таким именем
		CPreparedSDetectWordsGroupsByDetectWordsGroupName oPreparedSDetectWordsGroupsByDetectWordsGroupName;	
		oPreparedSDetectWordsGroupsByDetectWordsGroupName.Prepare(*m_pConnection->GetSession());		
		oPreparedSDetectWordsGroupsByDetectWordsGroupName.SetDetectWordsGroupName(m_wsDetectWordsGroupName);		
		
		if (oPreparedSDetectWordsGroupsByDetectWordsGroupName.Open() != S_OK)
		{
			std::wstring wsMessage = L"Операция завершена неудачно: Не удалось выполнить запрос к базе по названию группы.";
			wsMessage += L"Имя группы слов для детектирования : ";
			wsMessage += m_wsDetectWordsGroupName;
			oPreparedSDetectWordsGroupsByDetectWordsGroupName.Close();
			SS_THROW((wchar_t*)wsMessage.c_str());
		}
		
		if (oPreparedSDetectWordsGroupsByDetectWordsGroupName.MoveFirst() != S_OK)
		{
			std::wstring wsMessage = L"Операция завершена неудачно: группа слов для детектирования с таким именем отсутствует. ";
			wsMessage += L"Имя группы слов для детектирования : ";
			wsMessage += m_wsDetectWordsGroupName;
			oPreparedSDetectWordsGroupsByDetectWordsGroupName.Close();
			SS_THROW((wchar_t*)wsMessage.c_str());
		} 
		
		do
		{
			std::wstring wsDetectWord;
			oPreparedSDetectWordsGroupsByDetectWordsGroupName.GetDetectWord(wsDetectWord);
			oDetectGroup.insert(wsDetectWord);
		}
		while (oPreparedSDetectWordsGroupsByDetectWordsGroupName.MoveNext() == S_OK);
		oPreparedSDetectWordsGroupsByDetectWordsGroupName.Close();
	}
	SS_CATCH(L"")
}

//Удаляет текущий набор элементов из группы слов для детектирования и устанавливает новый
void SS::SyntaxConstructions::SQL::CDetectWordsGroups::SetWords(SS::SyntaxConstructions::Types::TDetectWordsGroup& oDetectGroup)
{
	DeleteDetectWordsGroup();
	CreateDetectWordsGroup(oDetectGroup);
}

//Определяет входит ли слово в группу для детектирования
bool SS::SyntaxConstructions::SQL::CDetectWordsGroups::IsDetectWord(std::string& sDetectWord)
{
	SS_TRY
		SS_THROW(L"Данная функция не предусмотрена при работе с данным режимом");
	SS_CATCH(L"")
	return false;
}

//*************************************** DBMS ***************************************

//Конструктор
SS::SyntaxConstructions::DBMS::CDetectWordsGroups::CDetectWordsGroups(wchar_t* szBaseName)
	: 
	SS::CConnectDBMS(szBaseName),
	m_TableInited(false)
{
	TableRecord[0] = &m_oDetectWordGroupName.m_wsDetectWordGroupName; 
	TableRecord[1] = &m_oDetectWordGroupName.m_wsDetectWord;
}

//Инициализация соединения с таблицой
void SS::SyntaxConstructions::DBMS::CDetectWordsGroups::InitConnection()
{
	SS_TRY
	{
		std::wstring wTable = L"DetectWordsGroups";
		SS::CConnectDBMS::SetTable(wTable);
				
		if ( m_pDTable->FindInit(L"DetectWordsGroupName", &m_oDetectWordGroupName.m_wsDetectWordGroupName, TableRecord, efmByDetectWordGroupName) != S_OK )   
		{ SS_THROW(L"Ошибка инициализации поиска по полю ID"); }

		if ( m_pDTable->FindInit(L"DetectWord", &m_oDetectWordGroupName.m_wsDetectWord, TableRecord, efmByDetectWord) != S_OK )   
		{ SS_THROW(L"Ошибка инициализации поиска по полю Name"); }
		
		if ( m_pDTable->ScanInit(0, TableRecord) != S_OK )   
		{ SS_THROW(L"Ошибка инициализации поиска по полю Name"); }

		m_TableInited = true;
	}
	SS_CATCH(L"")
}

//Возвращает все элементы группы слов для детектирования
void SS::SyntaxConstructions::DBMS::CDetectWordsGroups::GetWords(
												SS::SyntaxConstructions::Types::TDetectWordsGroup & oDetectGroup)
{
	SS_TRY
	{
		if (!m_TableInited) 
		{ InitConnection(); }

		m_oDetectWordGroupName.m_wsDetectWordGroupName = m_wsDetectWordsGroupName;

		while ( m_pDTable->Find(efmByDetectWordGroupName) == S_OK )   
		{ oDetectGroup.insert(m_oDetectWordGroupName.m_wsDetectWord); }
	}
	SS_CATCH(L"")
}

//Удаляет текущий набор элементов из группы слов для детектирования и устанавливает новый
void SS::SyntaxConstructions::DBMS::CDetectWordsGroups::SetWords(SS::SyntaxConstructions::Types::TDetectWordsGroup& oDetectGroup)
{
	SS_TRY
		SS_THROW(L"Данная функция в текущем режиме не работает");
	SS_CATCH(L"")
}

//Определяет входит ли слово в группу для детектирования
bool SS::SyntaxConstructions::DBMS::CDetectWordsGroups::IsDetectWord(std::string & sDetectWord)
{
	SS_TRY
		SS_THROW(L"Данная функция в текущем режиме не работает");
	SS_CATCH(L"")
	return false;
}

//Cоздает группу слов для детектирования
bool SS::SyntaxConstructions::DBMS::CDetectWordsGroups::CreateDetectWordsGroup(SS::SyntaxConstructions::Types::TDetectWordsGroup& oDetectGroup)
{
	SS_TRY
		SS_THROW(L"Данная функция в текущем режиме не работает");
	SS_CATCH(L"")
	return false;
}

///Удаляет текущую группу слов для детектирования
void SS::SyntaxConstructions::DBMS::CDetectWordsGroups::DeleteDetectWordsGroup()
{
	SS_TRY
		SS_THROW(L"Данная функция в текущем режиме не работает");
	SS_CATCH(L"")
}





