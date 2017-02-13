#pragma once

#include <map>
#include <string>
using namespace std;


class CPathMap
{
public:
	CPathMap();
	~CPathMap();

public:
	bool IsEmpty();
	int AddPath(const wchar_t* szPath);
	int AddPathByID(const wchar_t* szPath, int id);
	const wchar_t* GetPath(int id);

private:
	void NormalizePath(LPCWSTR szPath, wstring* pNormalPath);
	void AddToIDMap(int id, wstring strPath);

private:
	typedef map<wstring, int> TWorkPathMap;
	TWorkPathMap m_mPathIDs;

	typedef map<int, wstring> TIDMap;
	TIDMap m_mIDPaths;

	int m_iMaxID;
};
