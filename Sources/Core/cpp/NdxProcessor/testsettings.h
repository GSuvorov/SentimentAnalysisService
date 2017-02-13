#pragma once

namespace tsett
{
///	секция ини-файла в которой находятся все настройки
static const wchar_t*	wszSettings			= L"SETTINGS";
///	инвадидное значение
static const wchar_t*	wszInvalidValue		= L"__INVALID_VALUE__";
///	пустая строка
static const wchar_t*	wszEmptyString		= L"";
///	слеш
static const wchar_t*	wszSlash			= L"\\";
///	ini-file extension 
static const wchar_t*	wszIniExt			= L".ini";

static const wchar_t*	wcszKey_PathWork = L"PATH_WORK";
static const wchar_t*	wcszKey_PathDatabase = L"PATH_DATABASE";
static const wchar_t*	wcszKey_NameInd = L"NAME_IND";
static const wchar_t*	wcszKey_TxtQueries = L"QUERIES_FILE_NAME";

///	Класс для работы с настройками
///	Чтение и запись в ини-файл происходит только в Load и Save
class CSettings
{
public:
	///	ctr
	CSettings()
	{
		std::wstring	sCompName = GetCompName();
		std::wstring	sExeDir = GetExeDir();
		//	проверка имени компа
		if (sCompName.length() == 0) return;
		//	проверка директории где лежит EXE
		if (sExeDir.length() == 0) return;
		//	составляем имя ини-файла
		m_sFullIniFileName = 
			sExeDir+
			std::wstring(wszSlash)+
			sCompName+
			std::wstring(wszIniExt);
	}

	///	dtr
	~CSettings(void){}

	///	\param[in]	_Dest		Строка куда считывать результат
	///	\param[in]	_KeySource	Из какого ключа считывать данные
	///	\return	Успешность чтения. В случае неуспеха _dest = ""
	bool	SettingRead(std::wstring&	_Dest, const wchar_t*  _KeySource)
	{
		//	обнуляем приёмник
		_Dest.assign(wszEmptyString);
		//	проверка существования имени ини-файла
		if (m_sFullIniFileName.length() == 0) return false;

		DWORD dwBytesWritten = ::GetPrivateProfileStringW(
			wszSettings,
			_KeySource,
			wszInvalidValue,
			m_wcBuffer,
			m_dwBufferSize,
			m_sFullIniFileName.c_str());

		if ((dwBytesWritten == (m_dwBufferSize - 1)) || (dwBytesWritten == (m_dwBufferSize - 2)))
		{
			_Dest.assign(wszEmptyString);
			return false;
		}

		_Dest.assign(m_wcBuffer, dwBytesWritten);
		return true;
	}

	///	Сохранение-создание ини-файла по заданному пути.
	///	\param[in]	_KeyDest	Ключ в ини-файле куда записывать строку
	///	\param[in]	_Source		Из какой строки считывать данные
	///	\return	Успешность.
	bool	SettingWrite(const wchar_t*  _KeyDest, std::wstring const&	_Source)
	{
		//	проверка существования имени ини-файла
		if (m_sFullIniFileName.length() == 0) return false;

		return (FALSE != ::WritePrivateProfileStringW(
			wszSettings,
			_KeyDest,
			_Source.c_str(),
			m_sFullIniFileName.c_str()));
	}

	///	Получение директории где лежит EXE-файл, без слеша
	///	\return	Директория без слеша где лежит EXE-файл
	static std::wstring	GetExeDir()
	{
		wchar_t			buf[MAX_PATH];
		unsigned int	name_len = ::GetModuleFileNameW(NULL, buf, MAX_PATH);
		if (name_len == 0) return std::wstring(wszEmptyString);

		//	ищем слеш, отсекаем имя файла
		for (unsigned int i = name_len; i > 0; --i)
		{
			name_len = i-1;
			if (buf[name_len] == L'\\') break;
		}

		if(name_len == 0) return std::wstring(wszEmptyString);
		return std::wstring(buf, name_len);
	}

	///	Получение имени компьютера
	///	\return	Результат ::GetComputerName(), имя компьютера
	static std::wstring	GetCompName()
	{
		wchar_t	buf[MAX_COMPUTERNAME_LENGTH + 1];
		DWORD size = MAX_COMPUTERNAME_LENGTH+1;
		BOOL	res = ::GetComputerNameW(buf, &size);
		if (res == FALSE) return std::wstring(wszEmptyString);
		return std::wstring(buf, size);
	}

private:
	///	Путь к ини-файлу настроек
	std::wstring			m_sFullIniFileName;
	///	Размер буфера для чтения
	static const DWORD		m_dwBufferSize = 1024;
	///	буфер для чтения
	wchar_t					m_wcBuffer[m_dwBufferSize];
};
}	//namespace tsett