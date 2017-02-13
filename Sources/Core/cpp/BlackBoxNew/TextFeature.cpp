#include "StdAfx.h"
#include "TextFeature.h"

using namespace CommonContainers::Define;
using namespace SS::Interface::Core::CommonContainers;


namespace SS
{
namespace Core
{
namespace BlackBox
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CTextFeature::CTextFeature()
{
	SS_TRY;

	Init();

	SS_CATCH(L"");
}

CTextFeature::~CTextFeature()
{
	SS_TRY;

	Del();

	SS_CATCH(L"");
}

/////////////////////////////////////////////////////////////////////////////////////
// IBase functions

ULONG CTextFeature::Release()
{
	delete this;
	return 0;
}

HRESULT CTextFeature::QueryInterface(REFIID pIID, void** pBase)		//приведение к интерфейсу с поданным REFIID. 
{
	if (pIID==IID_TextFeature)
	{
		*pBase = (ITextFeature*)this;
	}
	else if( pIID == IID_Base )
	{
		*pBase=static_cast< SS::Interface::IBase* >( this );
	}
	else 
		*pBase = NULL;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////
// CTextFeature functions

void CTextFeature::Init()
{
	SS_TRY;

	*m_szPath = 0;
	*m_szTextName = 0;			
	*m_szAccessRights = 0;		
	*m_szBaseName = 0;

	m_FirstSentenceAbsNumber = 0;
	m_TextNumber = 0;
	m_TextSize = 0;
	m_TextFileSize = 0;
	m_PluginID = 0;

	SS_CATCH(L"");
}

void CTextFeature::Del()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CTextFeature functions

void CTextFeature::Clear()
{
	SS_TRY
	{
		Del();
		Init();
	}
	SS_CATCH(L"")
}

void CTextFeature::GetTextFeatureBuff(char** ppBuff, unsigned int* pSize)
{
	SS_TRY;

	m_oOutputBuff.Allocate(SIZEWithoutTitle);

	m_oOutputBuff.Concatenate(m_szPath, SIZEPath);
	m_oOutputBuff.Concatenate(m_szTextName, SIZETextName);
	m_oOutputBuff.Concatenate(m_szAccessRights, SIZEAccessRights);
	m_oOutputBuff.Concatenate(m_szBaseName, SIZEBaseName);
	m_oOutputBuff.Concatenate(&m_FirstSentenceAbsNumber, SIZEFirstSentenceAbsNumber);
	m_oOutputBuff.Concatenate(&m_TextNumber, SIZETextNumber);
	m_oOutputBuff.Concatenate(&m_TextSize, SIZETextSize);
	m_oOutputBuff.Concatenate(&m_TextFileSize, SIZETextFileSize);
	m_oOutputBuff.Concatenate(&m_PluginID, SIZEPluginID);
	m_oOutputBuff.Concatenate(&m_TextFileTime, SIZETextFileTime);

	*ppBuff = (char*) m_oOutputBuff.GetBuff();
	*pSize = m_oOutputBuff.GetSize();

	SS_CATCH(L"");
}

void CTextFeature::SetTextFeatureBuff(char* pBuff, unsigned int Size)
{
	SS_TRY;

	if(!pBuff)
		SS_THROW(__WFUNCTION__ AND L"Буфер не инициализирован !!!");

	m_oInputBuff.SetBuff((BYTE*) pBuff, Size);

	m_oInputBuff.Copy(m_szPath, SIZEPath);
	m_oInputBuff.Copy(m_szTextName, SIZETextName);
	m_oInputBuff.Copy(m_szAccessRights, SIZEAccessRights);
	m_oInputBuff.Copy(m_szBaseName, SIZEBaseName);
	m_oInputBuff.Copy(&m_FirstSentenceAbsNumber, SIZEFirstSentenceAbsNumber);
	m_oInputBuff.Copy(&m_TextNumber, SIZETextNumber);
	m_oInputBuff.Copy(&m_TextSize, SIZETextSize);
	m_oInputBuff.Copy(&m_TextFileSize, SIZETextFileSize);
	m_oInputBuff.Copy(&m_PluginID, SIZEPluginID);
	m_oInputBuff.Copy(&m_TextFileTime, SIZETextFileTime);

	SS_CATCH(L"");
}

const wchar_t* CTextFeature::GetTextPath()										// получить путь к тексту
{
	SS_TRY;

	return m_szPath;

	SS_CATCH(L"")
}

void CTextFeature::SetTextPath(const wchar_t* szPath)							// записать путь к тексту
{
	SS_TRY;

	if(szPath)
		wcsncpy(m_szPath, szPath, SIZEPath);

	SS_CATCH(L"");
}

const wchar_t* CTextFeature::GetTextName()										// получить имя текста
{
	SS_TRY;

	return m_szTextName;

	SS_CATCH(L"");
}

void CTextFeature::SetTextName(const wchar_t* szName)							// записать имя текста
{
	SS_TRY;

	if(szName)
		wcsncpy(m_szTextName, szName, SIZETextName);

	SS_CATCH(L"");
}

const char* CTextFeature::GetAccessRights()									// получить права доступа
{
	SS_TRY;

	return m_szAccessRights;

	SS_CATCH(L"");
}

void CTextFeature::SetAccessRights(const char* szAccessRights)				// записать права доступа
{
	SS_TRY;

	if(szAccessRights)
		strncpy(m_szAccessRights, szAccessRights, SIZEAccessRights);

	SS_CATCH(L"");
}

const char* CTextFeature::GetBaseName()										// получить имя базы
{
	SS_TRY;

	return m_szBaseName;

	SS_CATCH(L"");
}

void CTextFeature::SetBaseName(const char* szName)							// записать имя базы
{
	SS_TRY;

	if(szName)
		strncpy(m_szBaseName, szName, SIZEBaseName);

	SS_CATCH(L"");
}

//вернет размер, занимаемый итемом в байтах
unsigned int CTextFeature::GetMemorySize()
{
	SS_TRY;

	return  (UINT) wcslen(m_szPath) +
			(UINT) wcslen(m_szTextName) +
			(UINT) strlen(m_szAccessRights) +
			(UINT) strlen(m_szBaseName) +
			sizeof(*this);

	SS_CATCH(L"");
}

}
}
}
