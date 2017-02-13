#pragma once

#include "..\ASCInterface\ITextFeature.h"
using namespace SS::Interface::Core::CommonContainers;

#include <string>

#include <assert.h>


namespace CommonContainers
{
namespace Define
{
		
//размер в байтах составляющих буфера и самого буфера

#define SIZEUI							sizeof (unsigned int)
#define SIZEI							sizeof (int)

#define SIZEPath						256
#define SIZETextName					256
#define SIZEAccessRights				256
#define SIZEBaseName					256

#define SIZEFirstSentenceAbsNumber		SIZEUI
#define SIZETextNumber					SIZEUI
#define SIZETextSize					SIZEUI
#define SIZETextFileSize				SIZEUI
#define SIZEPluginID					SIZEUI

#define SIZETextFileTime				sizeof (FILETIME)	
	
#define SIZEWithoutTitle (SIZEPath + SIZETextName + SIZEAccessRights + SIZEBaseName + SIZETextFileTime + (SIZEUI * 5))
	
}
}

namespace SS
{
namespace Core
{
namespace BlackBox
{

template <bool bAllocate>
class CTextFeatureBuff
{
private:
	int m_iPos;
	int m_iSize;
	BYTE* m_pBuff;
public:
	CTextFeatureBuff() :
			m_iPos(0),
			m_iSize(0),
			m_pBuff(NULL)
	{
	}

	~CTextFeatureBuff()
	{
		if(bAllocate && m_pBuff)
			delete m_pBuff;
	}

public:
	void Allocate(int iSize)
	{
		assert(bAllocate);
		if(m_pBuff) 
			delete m_pBuff;
		
		m_pBuff = new BYTE[iSize];
		m_iSize = iSize;
		m_iPos = 0;
	}

	void SetBuff(BYTE* pBuff, int iSize)
	{
		assert(!bAllocate);
		m_pBuff = pBuff;
		m_iSize = iSize;
		m_iPos = 0;
	}

	void Concatenate(void* pSource, int iSize)
	{
		assert(m_iPos + iSize <= m_iSize);
		if(pSource)
			memcpy(m_pBuff + m_iPos, pSource, iSize);
		m_iPos += iSize;
	}

	void Copy(void* pDest, int iSize)
	{
		assert(m_iPos + iSize <= m_iSize);
		memcpy(pDest, m_pBuff + m_iPos, iSize);
		m_iPos += iSize;
	}

	int GetSize()
	{
		return m_iPos;
	}

	int GetCapacity()
	{
		return m_iSize;
	}

	BYTE* GetBuff()
	{
		return m_pBuff;
	}

};

///инкапсулятор характеристик текста
/**реализует функциональность интерфейса SS::Interface::Core::CommonContainers::ITextFeature*/

class CTextFeature : public SS::Interface::Core::CommonContainers::ITextFeature
{
	CTextFeatureBuff<true> m_oOutputBuff;
	CTextFeatureBuff<false> m_oInputBuff;
protected:
	wchar_t			m_szPath[SIZEPath];							// путь к тексту
	wchar_t			m_szTextName[SIZETextName];					// имя текста
	char			m_szAccessRights[SIZEAccessRights];			// права доступа
	char			m_szBaseName[SIZEBaseName];					// имя базы
	unsigned int	m_FirstSentenceAbsNumber;					//абсолютный номер первого предложения текста
	unsigned int	m_TextNumber;								// номер текста
	unsigned int	m_TextSize;									// размер текста
	FILETIME		m_TextFileTime;								// время последнего изменения файла с текстом
	unsigned int	m_TextFileSize;								// размер файла текста
	unsigned int	m_PluginID;									// ID плагина

private:
	//СОБСТВЕННЫЕ МЕТОДЫ
	void Init();
	void Del();

public:
	CTextFeature(void);
	virtual ~CTextFeature(void);

	//МЕТОДЫ IBase
	virtual ULONG Release();
	virtual HRESULT QueryInterface(REFIID pIID, void** pBase);	//приведение к интерфейсу с поданным REFIID. 
	//pBase будет NULL, если данный интерфейс не поддерживается

	///вернет размер, занимаемый итемом в байтах
	unsigned int GetMemorySize();

	//МЕТОДЫ ITextFeature
	virtual void GetTextFeatureBuff(char** ppBuff, unsigned int* pSize);
	virtual void SetTextFeatureBuff(char* pBuff, unsigned int Size);
	///получить путь к тексту
	virtual const wchar_t* GetTextPath();
	///записать путь к тексту
	virtual void SetTextPath(const wchar_t* szPath);
	///получить имя текста
	virtual const wchar_t* GetTextName();
	///записать имя текста
	virtual void SetTextName(const wchar_t* szName);
	///получить права доступа
	virtual const char* GetAccessRights();
	///записать права доступа
	virtual void SetAccessRights(const char* szAccessRights);
	///получить имя базы
	virtual const char* GetBaseName();
	///записать имя базы
	virtual void SetBaseName(const char* szName);
	///очистка значений характеристики
	virtual void Clear();
	///установление абсолютного номера первого предложения текста
	virtual void SetFirstSentenceAbsNumber(unsigned int uiValue)
	{
		m_FirstSentenceAbsNumber = uiValue;
	};
	///получение абсолютного номера первого предложения текста
	virtual unsigned int GetFirstSentenceAbsNumber()
	{
		return m_FirstSentenceAbsNumber;
	};
	///установление номера текста
	virtual void SetTextNumber(unsigned int uiValue)
	{
		m_TextNumber = uiValue;
	}
	///получение номера текста
	virtual unsigned int GetTextNumber()
	{
		return m_TextNumber;
	}
	///установление размера текста
	virtual void SetTextSize(unsigned int uiValue)
	{
		m_TextSize = uiValue;
	}
	///получение размера текста
	virtual unsigned int GetTextSize()
	{
		return m_TextSize;				
	}
	///вернет время последнего изменения файла с текстом
	virtual const FILETIME GetTextFileTime() const
	{
		return (m_TextFileTime);
	}
	///устанавливает время последнего изменения файла с текстом
	virtual void SetTextFileTime(const FILETIME* pFileTime)
	{
		m_TextFileTime = (*pFileTime);
	}
	///устанавливает размер файла текста
	virtual void SetTextFileSize(const unsigned int ulFileSize)
	{
		m_TextFileSize = ulFileSize;
	}
	///возвращает размер файла текста
	virtual const unsigned int GetTextFileSize() const
	{
		return m_TextFileSize;
	}
	///устанавливает ID плагина
	virtual void SetPluginID(const unsigned int uiPluginID)
	{
		m_PluginID = uiPluginID;
	}
	///возвращает ID плагина
	virtual const unsigned int GetPluginID() const
	{
		return m_PluginID;
	}

	virtual ITitle* GetFirstTitle()
	{
		return NULL;
	}

	virtual void AddTitle(int nBegin, int nEnd, int iType)
	{
		SS_THROW(L"Запрещено использовать этот метод");
	}
};

}
}
}
