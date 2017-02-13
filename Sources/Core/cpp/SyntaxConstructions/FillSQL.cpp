//FillSQL.cpp
#include "StdAfx.h"
#include ".\fillsql.h"
#include "../ASSInterface/TSyntaxConstructions.h"
#include "../ASSInterface/Construction.h"
#include "./dboSyntax.h"

namespace SS
{
namespace UnitTests
{
namespace SyntaxConstructions
{
	//Заполнение SQL базы из DBMS
	void CFillSQL::Fill()
	{
		//Получить таблицу DBMS
		std::wstring wDataTableName = L"SyntaxConstruction";		
		m_pDataBase->MoveToDataTable(wDataTableName.c_str());
		SS::Interface::Core::DBMS::IDataTable* pDTable = m_pDataBase->OpenDataTable();
		if ( (pDTable==NULL) || (!pDTable->IsOpen()) )
		{ SS_THROW(L"ошибка при открытии таблицы SyntaxConstruction"); }

		//Инициализация DBMS
		SS::SyntaxConstructions::Types::SSyntaxConstruction oSSyntaxConstruction;
		void* TableRecord[] = 
		{
			&oSSyntaxConstruction.m_i32ID,
			&oSSyntaxConstruction.m_wsName,
			&oSSyntaxConstruction.m_wsWord,
			&oSSyntaxConstruction.m_v_Data,
			&oSSyntaxConstruction.m_i32DataSize,
			&oSSyntaxConstruction.m_i32Level
		};
		if ( pDTable->ScanInit(0, TableRecord) != S_OK )   
		{ SS_THROW(L"Ошибка инициализации."); }

		//Инициализация SQL
		SS::SyntaxConstructions::SQL::SQL_ACCESS::Syntax::CPreparedDAllSyntaxConstruction oPreparedDelete;
		SS::SyntaxConstructions::SQL::SQL_ACCESS::Syntax::CPreparedISyntaxConstruction oPreparedInsert;
		
		//Удалить запись из базы
		oPreparedDelete.Prepare(*m_pConnection->GetSession());
		if(oPreparedDelete.Open() != S_OK)
		{ SS_THROW(L"Очистка SQL таблицы не выполнена."); }
		oPreparedDelete.Close();
		
		//Вставить запись в базу
		oPreparedInsert.Prepare(*m_pConnection->GetSession());
		//Объект конструкции
		SS::SyntaxConstructions::Types::CConstructionObject oConstructionObject;	
		while (pDTable->Scan() == S_OK)
		{
			////Установка фич
			oConstructionObject.SetValue(oSSyntaxConstruction.m_v_Data);
			oSSyntaxConstruction.m_v_Data.clear();
			oConstructionObject.GetValue(oSSyntaxConstruction.m_v_Data);
			//Заполнение SQL
			oPreparedInsert.SetID(oSSyntaxConstruction.m_i32ID);
			oPreparedInsert.SetName(oSSyntaxConstruction.m_wsName.c_str());
			oPreparedInsert.SetWord(oSSyntaxConstruction.m_wsWord.c_str());
			oPreparedInsert.SetConstructionData(oSSyntaxConstruction.m_v_Data);
			oPreparedInsert.SetLevel(oSSyntaxConstruction.m_i32Level);

			if (oPreparedInsert.Open() != S_OK)
			{ SS_THROW(L"Вставка не прошла!"); }
			oPreparedInsert.Close();
		}
		pDTable->Close();
	}

}
}
}