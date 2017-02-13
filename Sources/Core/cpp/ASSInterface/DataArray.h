#pragma once


namespace SS
{
namespace Helpers
{

template <class T>
class CDataArray
{
public:
	CDataArray() :
			m_pData(NULL),
			m_dwSize(0),
			m_dwCapacity(0)
	{
	}

	~CDataArray()
	{
		delete [] m_pData;
	}

public:
	void Allocate(DWORD dwSize)
	{
		m_dwSize = dwSize;
		if(dwSize > m_dwCapacity)
		{
			delete [] m_pData;
			m_pData = new T[dwSize];
			m_dwCapacity = dwSize;
		}
	}

	void Copy(const T* pData, DWORD dwSize)
	{
		Allocate(dwSize);
		memcpy(m_pData, pData, dwSize * sizeof(T));
	}

	void SetElementRange(DWORD dwPos, DWORD dwCount, T Value)
	{
		if(dwPos + dwCount - 1 >= m_dwSize)
			SS_THROW(L"Trying to access CDataArray element out of range");

		for(DWORD dw = 0; dw < dwCount; dw++)
			m_pData[dwPos + dw] = Value;
	}

	T* GetBuffer()
	{
		return m_pData;
	}

	DWORD GetSize()
	{
		return m_dwSize;
	}

	DWORD GetSizeInBytes()
	{
		return m_dwSize * sizeof(T);
	}

	void SetSize(DWORD dwNewSize)
	{
		if(dwNewSize > m_dwCapacity)
			SS_THROW(L"Trying to set too large buffer size");
		m_dwSize = dwNewSize;
	}

private:
	T* m_pData;
	DWORD m_dwSize;
	DWORD m_dwCapacity;
};

}
}
