#pragma once

#include "..\ASCInterface\defines.h"
#include "..\ASCInterface\IInterface.h"
#include ".\setting_factory.h"
#include <map>

namespace SS
{
namespace CommonServices
{

//----------------------------------------------------------------//

///менеждер управления настройками
class CSettingsManager 
{
public:
	typedef std::map<std::wstring, CBasisSetting*> TSettingsCollection;

	CSettingsManager(void);
	virtual ~CSettingsManager(void);

	///сбрасывает все настройки 
	void ResetSettings(void);

protected:
	///операция с настройкой
	void OperateParameter(bool bGetOrSet, const wchar_t* wszName, void* pValue, SS::Interface::Core::CommonServices::EParameterType peType, bool bIsArray);
	///вовзращает значение настройки
	template <class T> T GetParameter(const wchar_t* wszName, SS::Interface::Core::CommonServices::EParameterType peType)
	{
		T tValue;
		OperateParameter(true, wszName, (void*)&tValue, peType, false);
		return tValue;
	};
	///устанавливает значение настройки
	template <class T> void SetParameter(T tValue, const wchar_t* wszName, SS::Interface::Core::CommonServices::EParameterType peType)
	{
		OperateParameter(false, wszName, (void*)&tValue, peType, false);
	};
	

	///загружает настройки из файла
	void LoadSettings(void);
	///сохраняет настройки в файл
	void SaveSettings(void);
	///очищает настройки 
	void ClearSettings(void);
	///возвращает настройку по имени
	CBasisSetting* GetSetting(const wchar_t* wszSettingName);
	///добавляет настройку
	bool AddSetting(CBasisSetting* pSetting);
	///пишет буфер в файл
	bool WriteToFile(std::vector<unsigned char>* pSettingsBuffer);
	///читает буфер из файл
	bool ReadFromFile(std::vector<unsigned char>* pSettingsBuffer);
	///формирует путь к файлу настроек
	void MakeSettingFilePath(std::wstring* pwsFilePath);
	///проверяет актуальность загруженных настроек
	bool CheckUpToDate(void);
	///кодирует буфер байт
	void EncodeDecodeBuffer(std::vector<unsigned char>* pSettingsBuffer);
	///возвращает загрузчик
	virtual SS::Interface::Core::ResourceManagers::ILoadManager* GetLoader(void) = 0;

	///коллекция настроек
	TSettingsCollection m_SettingsCollection;
	///фабрика настроек
	CSettingFactory m_SettingFactory;
	///путь к файлу настроек
	std::wstring m_wsSettingsFilePath;
	///время последнего изменения файла настроек
	FILETIME m_LastFileTime;
	///флаг загрузки
	bool m_bSettingsLoaded;
};

//----------------------------------------------------------------//

}
}