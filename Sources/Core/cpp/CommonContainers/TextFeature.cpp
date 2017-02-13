//TextFeature.cpp

#include "StdAfx.h"
#include ".\textfeature.h"

namespace SS
{
namespace Core
{
namespace CommonContainers
{

//конструкторы/деструкторы
CTextFeature::CTextFeature() : m_pItemTextFeature(NULL), m_pBlackBoxTextStorage(NULL)
{
}

CTextFeature::~CTextFeature()
{
	if (m_pItemTextFeature)
	{
		m_pItemTextFeature->Release();
	}	
}

// МЕТОДЫ IBase
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

void CTextFeature::SetLoadManager(ILoadManager* pLoadManager)		// устанавливает указатель на лоад мэнеджер
{
	SS_TRY
		if (!m_pItemTextFeature)
		{
			CBaseCoreClass::SetLoadManager(pLoadManager);
			m_pBlackBoxTextStorage = (IBlackBoxTextStorage*) (GetLoadManager())->GetInterface(L"./core/linguistics/BlackBox.dll", CLSID_BlackBoxTextStorage, IID_BlackBoxTextStorage);	
			m_pItemTextFeature = m_pBlackBoxTextStorage->CreateTextFeature();
		}
	SS_CATCH(L"")
};

//другие методы
ITitle* CTextFeature::GetFirstTitle()
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			return m_pItemTextFeature->GetFirstTitle();
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}

void CTextFeature::AddTitle(int nBegin, int nEnd, int iType)
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			m_pItemTextFeature->AddTitle(nBegin, nEnd, iType);
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}

void CTextFeature::Clear()
{
	SS_TRY
	{
		if (m_pItemTextFeature)
		{
			m_pItemTextFeature->Clear();
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	}
	SS_CATCH(L"")
}

void CTextFeature::GetTextFeatureBuff(char** ppBuff, unsigned int* pSize)
{
	SS_TRY
	{
		if (m_pItemTextFeature)
		{
			m_pItemTextFeature->GetTextFeatureBuff(ppBuff, pSize);
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	}
	SS_CATCH(L"")
}

void CTextFeature::SetTextFeatureBuff(char* pBuff, unsigned int Size)
{
	SS_TRY
	{
		if (m_pItemTextFeature)
		{
			m_pItemTextFeature->SetTextFeatureBuff(pBuff, Size);
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	}
	SS_CATCH(L"")
}

const wchar_t* CTextFeature::GetTextPath()									// получить путь к тексту
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			return m_pItemTextFeature->GetTextPath();
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}

void CTextFeature::SetTextPath(const wchar_t* szPath)						// записать путь к тексту
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			m_pItemTextFeature->SetTextPath(szPath);
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}

const wchar_t* CTextFeature::GetTextName()									// получить имя текста
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			return m_pItemTextFeature->GetTextName();
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}

void CTextFeature::SetTextName(const wchar_t* szName)						// записать имя текста
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			m_pItemTextFeature->SetTextName(szName);
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}

const char* CTextFeature::GetAccessRights()									// получить права доступа
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			return m_pItemTextFeature->GetAccessRights();
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}

void CTextFeature::SetAccessRights(const char* szAccessRights)				// записать права доступа
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			m_pItemTextFeature->SetAccessRights(szAccessRights);
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}

//const char* CTextFeature::GetBaseName()									// получить имя базы
//{
//	SS_TRY
//		//return m_pItemTextFeature->GetBaseName();
//		return NULL;
//	SS_CATCH(L"")
//}
//
//void CTextFeature::SetBaseName(const char* szName)						// записать имя базы
//{
//	SS_TRY
//		//m_pItemTextFeature->SetBaseName(szName);
//	SS_CATCH(L"")
//}

//установление абсолютного номера первого предложения текста
void CTextFeature::SetFirstSentenceAbsNumber(unsigned int uiValue)			
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			m_pItemTextFeature->SetFirstSentenceAbsNumber(uiValue);
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
};
//получение абсолютного номера первого предложения текста
unsigned int CTextFeature::GetFirstSentenceAbsNumber()						
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			return m_pItemTextFeature->GetFirstSentenceAbsNumber();
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
};
//установление номера текста
void CTextFeature::SetTextNumber(unsigned int uiValue)						
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			m_pItemTextFeature->SetTextNumber(uiValue);
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}
//получение номера текста
unsigned int CTextFeature::GetTextNumber()									
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			return m_pItemTextFeature->GetTextNumber();
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}
//установление размера текста
void CTextFeature::SetTextSize(unsigned int uiValue)							
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			m_pItemTextFeature->SetTextSize(uiValue);
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}
//получение размера текста
unsigned int CTextFeature::GetTextSize()										
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			return m_pItemTextFeature->GetTextSize();
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}
//вернет время последнего изменения файла с текстом
const FILETIME CTextFeature::GetTextFileTime() const							
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			return m_pItemTextFeature->GetTextFileTime();
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}
//устанавливает время последнего изменения файла с текстом
void CTextFeature::SetTextFileTime(const FILETIME* pFileTime)				
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			m_pItemTextFeature->SetTextFileTime(pFileTime);
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}
//устанавливает размер файла текста
void CTextFeature::SetTextFileSize(const unsigned int ulFileSize)				
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			m_pItemTextFeature->SetTextFileSize(ulFileSize);
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}
//возвращает размер файла текста
const unsigned int CTextFeature::GetTextFileSize() const						
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			return m_pItemTextFeature->GetTextFileSize();
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}
//устанавливает ID плагина
void CTextFeature::SetPluginID(const unsigned int uiPluginID)					
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			m_pItemTextFeature->SetPluginID(uiPluginID);
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}
//возвращает ID плагина
const unsigned int CTextFeature::GetPluginID() const							
{
	SS_TRY
		if (m_pItemTextFeature)
		{
			return m_pItemTextFeature->GetPluginID();
		}
		else
		{
			SS_THROW(L"Выполните CTextFeature::SetLoadManager()");
		}
	SS_CATCH(L"")
}

}
}
}