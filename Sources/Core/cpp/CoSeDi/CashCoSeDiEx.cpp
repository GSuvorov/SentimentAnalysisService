#include "StdAfx.h"
#include "..\ASSInterface\TBaseNames.h"

#include ".\cashcosediex.h"

SS::CoSeDi::SQL::CCashCoSeDiEx::CCashCoSeDiEx(void) :
		m_oCashCoSeDiRus(SS::MANAGER::Constants::c_szCoSeDiRus), 
		m_oCashCoSeDiEng(SS::MANAGER::Constants::c_szCoSeDiEng)
{
}

SS::CoSeDi::SQL::CCashCoSeDiEx::~CCashCoSeDiEx(void)
{
}

bool SS::CoSeDi::SQL::CCashCoSeDiEx::Clear()
{
	SS_TRY
		m_oCashCoSeDiRus.Clear();
		m_oCashCoSeDiEng.Clear();
	SS_CATCH(L"")

	return true;
}

bool SS::CoSeDi::SQL::CCashCoSeDiEx::Load()
{
	SS_TRY
		m_oCashCoSeDiRus.Load();
		m_oCashCoSeDiEng.Load();
	SS_CATCH(L"")

	return true;
}

bool SS::CoSeDi::SQL::CCashCoSeDiEx::ReLoad()
{
	SS_TRY
		m_oCashCoSeDiRus.ReLoad();
		m_oCashCoSeDiEng.ReLoad();
	SS_CATCH(L"")

	return true;
}

bool SS::CoSeDi::SQL::CCashCoSeDiEx::ToBinary()
{
	SS_TRY
	{
			m_oCashCoSeDiRus.ToBinary();		
			m_oCashCoSeDiEng.ToBinary();
	}
	SS_CATCH(L"")

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SS::CoSeDi::DBMS::CCashCoSeDiEx::CCashCoSeDiEx(void) :
		m_oCashCoSeDiRus(SS::MANAGER::Constants::c_szCoSeDiRus), 
		m_oCashCoSeDiEng(SS::MANAGER::Constants::c_szCoSeDiEng)
{
}

SS::CoSeDi::DBMS::CCashCoSeDiEx::~CCashCoSeDiEx(void)
{
}

bool SS::CoSeDi::DBMS::CCashCoSeDiEx::Clear()
{
	SS_TRY
		m_oCashCoSeDiRus.Clear();
		m_oCashCoSeDiEng.Clear();
	SS_CATCH(L"")

	return true;
}

bool SS::CoSeDi::DBMS::CCashCoSeDiEx::Load()
{
	bool bRetVal = false;
	SS_TRY

		bRetVal = m_oCashCoSeDiRus.Load();
		bRetVal |= m_oCashCoSeDiEng.Load();

	SS_CATCH(L"")

	return bRetVal;
}

bool SS::CoSeDi::DBMS::CCashCoSeDiEx::ReLoad()
{
	SS_TRY
		m_oCashCoSeDiRus.ReLoad();
		m_oCashCoSeDiEng.ReLoad();
	SS_CATCH(L"")

	return true;
}

bool SS::CoSeDi::DBMS::CCashCoSeDiEx::ToBinary()
{
	SS_TRY
		m_oCashCoSeDiRus.ToBinary();		
		m_oCashCoSeDiEng.ToBinary();
	SS_CATCH(L"")

	return true;
}
