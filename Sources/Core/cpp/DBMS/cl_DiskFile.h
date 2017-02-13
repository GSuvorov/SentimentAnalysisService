#pragma once

#include <string>
#include "fn_Generic.h"

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "cl_DiskFile.h"


/*! \class DiskFileC
	*  \brief Работа с дисковыми файлами */
class DiskFileC   {

	public: //

		/// Конструктор
		DiskFileC();

		/// Деструктор
		~DiskFileC();

	public:

		/// Установить/изменить путь к файлу и имя файла
		VOID SetPathAndName(CONST STLSTR * sFilePath, CONST _TCHAR * sFileName);

		/// Получить имя файла (только имя)
		CONST STLSTR * GetName(VOID) { return(&m_FName); }

		/// Получить путь к файлу (только путь)
		CONST STLSTR * GetPath(VOID) { return(&m_FPath); }

		/// Получить полный путь к файлу (путь и имя)
		CONST STLSTR * GetFullPath(VOID) { return(&m_FFullPath); }

		/// Проверить существование файла
		BOOL ChkDirExistence(VOID);

		/// Открыть или создать файл
		HRESULT Open(VOID);

		/// Сбросить файловые буферы
		HRESULT Flush(VOID);

		/// Установить размер открытого файла нулевым
		HRESULT Clear(VOID);

		/// Закрыть файл
		HRESULT Close(BOOL bClearAfterClose = FALSE);

		/// Удалить файл
		HRESULT Delete(VOID);

		/// Переименовать из *.* в ~*.*
		HRESULT RenameBack(VOID);

		/// Переименовать из *.*_tmp в *.*
		HRESULT RenameTmp(VOID);

		/// Переместиь файл
		HRESULT Move(CONST _TCHAR * sNewFileName);

		/// Дополнить файл блоком из величины nFillValue размером nBytesNum32
		HRESULT Fill(UINT32 nBytesNum32, BYTE nFillValue);

		/// Сдвинуть текущую позицию в файле на nBytesNum32 байт от nFromPosition
		HRESULT SetPosition(UINT32 nBytesNum32, DWORD nFromPosition = FILE_BEGIN);

		/// Получить позицию в файле
		UINT32 GetPosition(VOID);

		/// Получить размер файла
		UINT32 GetSize(VOID);

		/// Читать из файла по указанному смещению
		HRESULT ReadAt(UINT32 nBeginOffset, LPVOID pData, INT32 nDataLen);

		/// Читать из файла по текущему смещению
		HRESULT Read(LPVOID pData, DWORD nDataLen);

		/// Писать в файл по указанному смещению
		HRESULT WriteAt(UINT32 nBeginOffset, LPCVOID pData, DWORD nDataLen);

		/// Писать в файл по смещению (current_offset - nDataLen)
		HRESULT WriteAtBackOffset(LPCVOID pData, INT32 nDataLen);

		/// Писать в конец файла
		HRESULT WriteAtEnd(LPCVOID pData, DWORD nDataLen);

		/// Писать форматированный текст в файл
		HRESULT  fprintf(CHAR * FormatStr, ...);

	public:

		/// Имя файла
		STLSTR m_FName;

		/// Путь к файлу
		STLSTR m_FPath;

		/// Полный путь к файлу
		STLSTR m_FFullPath;

	private: //

		/// Описатель файла
		HANDLE      m_FHandle;

//		UINT32      m_FPos64;
//		DWORD       m_FSectorSize;

		/// Признак старших версий OS
		BOOL        m_CoolOsFlag;

		/// Внутренняя функция записи
		HRESULT _Write(LPCVOID pData, DWORD nDataLen)   {
			DWORD WrittenDataLen;

			//return(WriteFile(m_FHandle,pData,nDataLen,&WrittenDataLen,NULL) &&  nDataLen==WrittenDataLen);

			if(!WriteFile(m_FHandle,pData,nDataLen,&WrittenDataLen,NULL)/* ||  nDataLen!=WrittenDataLen*/)   {
				return(E_FAIL);
			} else   {
				return(S_OK);
			}
		}

		/// Внутренняя функция чтения
		HRESULT _Read(LPVOID pData, DWORD nDataLen)   {
			DWORD ReaddenDataLen; // Bad english

			//return(ReadFile(m_FHandle,pData,nDataLen,&ReaddenDataLen,NULL) && nDataLen==ReaddenDataLen);

			if(!ReadFile(m_FHandle,pData,nDataLen,&ReaddenDataLen,NULL)/* || nDataLen!=ReaddenDataLen*/)   {
				return(E_FAIL);
			} else   {
				return(S_OK);
			}
		}
};
