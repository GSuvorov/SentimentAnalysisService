#pragma once

#include "..\ASCInterface\ICommonServices.h"

namespace SS
{
namespace Core
{
namespace CommonServices
{

class CFileFinder : public SS::Interface::Core::CommonServices::IFileFinder
{
public:
	CFileFinder(void);
	~CFileFinder(void);
	/// ищет файлы в папке
	/** 
		\param sRootPath - корневая папка
		\param sSubPath - вложенная папка
		\param vIgnoreExt - игнорировать расширения (формат - ".ext")
		\param vRelPaths - относительные пути к файлам в папке sRootPath
		\return bool - true, если без проблем
	*/
	bool GetFiles(std::wstring& sRootPath, std::wstring& sSubPath, std::vector<std::wstring>* vIgnoreExt, std::vector<std::wstring>* vRelPaths);

	///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
	HRESULT QueryInterface(REFIID pIID, void** pBase); 
	///освобождение
	ULONG Release();

};

}
}
}