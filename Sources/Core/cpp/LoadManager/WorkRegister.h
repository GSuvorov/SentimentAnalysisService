#pragma once

struct HKEY__;
#include "..\ASCInterface\services.h"

namespace SS
{
namespace Core
{
namespace CommonServices
{

	
///СЕРВИС ПОЛУЧЕНИЯ ПАРАМЕТРОВ СИСТЕМЫ
class CWorkRegister
{
public:
	CWorkRegister();
	~CWorkRegister();

	///возвращает значение параметра по его имени (юникод версия)
	std::wstring GetParameter(std::wstring wParametrName, const wchar_t* wszPath);

	///установка рабочей директории
	void SetWorkDir(std::wstring wPathToDir);

protected:

	///HANDLE для синхронизации потоков
	CManagerLifeTimeForCriticalSection m_CriticalSection;

	///корневой путь для получения параметров из реестра
	const std::wstring csPath;

	bool m_blParametersHadBeenLoaded;

	///путь к папке с базами данных
	std::wstring	m_sPathDataBase;
	///путь к папке с системой
	std::wstring	m_sPathWordsystem;
	///путь к папке с временными файлами
	std::wstring	m_sPathTemporaryFiles;
	///пароль для открытия соединения с базой данных
	std::wstring m_sPassword;
	///путь к модулям ядра
	std::wstring m_sPathCore;
	///рабочая директория
	std::wstring m_sWorkPath;
	///имя сервера (для сетеой версии)
	std::wstring m_sServer;
	///путь к директории плагинов
	std::wstring m_sDocPluginsPath;
	///имя датасорса
	std::wstring m_sDataSource;

	///загрузка параметра из реестра по его имени
	std::wstring	LoadParameter(std::wstring sNameParameter);
	///загрузка параметра из INI файла по его имени
	std::wstring	LoadParameterFromINIFile(std::wstring sNameParameter, const wchar_t* wszPath);
	///сохранение параметра
	void	SaveParameter(std::wstring sNameParameter,std::wstring sValue);
	///получение параметра из реестра
	std::wstring	QueryValue(HKEY hPredefinedKey, std::wstring sKeyName,std::wstring sValueName);
	///создание параметра в реестре
	void	CreateNewKey(HKEY hPredefinedKey, std::wstring sNewKeyName);
	///установление значения параметра в реестре
	void	SetKeyValue(HKEY hPredefinedKey, std::wstring sKeyName, std::wstring sValueName, std::wstring sValueSetting, long lValueType);

	///пересчитает при необходимости из относительного пути в нормальный
	void UpdateForRelativePath(std::wstring* pString, const wchar_t* wszPath);


	/// проверяет и загружалает параметры из ss.ini если не загружались
	void CheckAndLoadParameters(const wchar_t* wszPath);


};

}
}
}
