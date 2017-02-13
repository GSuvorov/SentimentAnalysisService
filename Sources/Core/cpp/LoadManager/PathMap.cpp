#include "stdafx.h"
#include "PathMap.h"

#include "..\ASCInterface\defines.h"
USES_LOAD_MANAGER;


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CPathMap::CPathMap() :
		m_iMaxID(0)
{
}

CPathMap::~CPathMap()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CPathMap interface

bool CPathMap::IsEmpty()
{
	return m_mPathIDs.empty();
}

int CPathMap::AddPath(const wchar_t* szPath)
{
	wstring wstr;
	NormalizePath(szPath, &wstr);

	int id;
	TWorkPathMap::iterator it = m_mPathIDs.find(wstr);
	if(it == m_mPathIDs.end())
	{
		id = m_iMaxID++;
		m_mPathIDs.insert(TWorkPathMap::value_type(wstr, id));
		AddToIDMap(id, wstr);
	}
	else
	{
		id = (*it).second;
	}

	return id;
}

int CPathMap::AddPathByID(const wchar_t* szPath, int id)
{
	wstring wstr;
	NormalizePath(szPath, &wstr);

	TWorkPathMap::iterator it = m_mPathIDs.find(wstr);
	if(it == m_mPathIDs.end())
	{
		if(id >= m_iMaxID)
			m_iMaxID = id + 1;
		m_mPathIDs.insert(TWorkPathMap::value_type(wstr, id));
		AddToIDMap(id, wstr);
	}
	else
	{
		if(id != (*it).second)
		{
			SAVE_LOG(L"Ошибка: Одинаковые пути имеют разные id");
			return -1;
		}
	}

	return id;
}

const wchar_t* CPathMap::GetPath(int id)
{
	TIDMap::iterator it = m_mIDPaths.find(id);
	if(it != m_mIDPaths.end())
		return (*it).second.c_str();

	SAVE_LOG(L"Данный id отсутствует в списке путей");
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////
// CPathMap functions

void CPathMap::NormalizePath(LPCWSTR szPath, wstring* pNormalPath)
{
	pNormalPath->reserve((int) wcslen(szPath) + 1);
	for(const wchar_t* sz = szPath; *sz; sz++)
	{
		if(*sz == L' ')
			continue;
		if(*sz == L'/')
			*pNormalPath += L'\\';
		else
			*pNormalPath += towlower(*sz);
	}

	while(pNormalPath->at(0) == L'\\' || pNormalPath->at(0) == L'.')
		pNormalPath->erase(pNormalPath->begin());
}

void CPathMap::AddToIDMap(int id, wstring strPath)
{
	pair<TIDMap::iterator, bool> pr = m_mIDPaths.insert(TIDMap::value_type(id, strPath));
	if(!pr.second)
		SAVE_LOG(L"Данный id соответствует двум разным путям");
}
