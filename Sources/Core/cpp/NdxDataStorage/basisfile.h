#pragma once

#include ".\const.h"
#include ".\console.h"
#include "..\ASDInterface\INdxDataStorage.h"
#include ".\data_unit.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxDataStorage
{

class CDataUnitFactory;

//скрывает варнинг о наследовании
#pragma warning( disable : 4250 )

//--------------------------------------------------------------------//

///базовый файл индексной базы
class CBasisFile : public CDataUnit, public virtual SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile
{
friend class CDataUnitFactory;
friend class CDataStorageFactory;

protected:
	CBasisFile(CDataUnitFactory* pDataStorageFactory);
	virtual ~CBasisFile(void);
	
public:
	//-----------IBase
	///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
	HRESULT QueryInterface(REFIID pIID, void** pBase); 
	//-----------Eof-IBase

	//-----------INdxDataUnit
	///открывает юнит данных в текущем режиме
	bool Open(const wchar_t* pszPath);
	///закрывает юнит данных 
	bool Close(void);
	///сбрасывает юнит данных 
	bool Flush(void);
	///очищает  юнит данных 
	void Clear(void);
	///возвращает размер юнита данных 
	unsigned int GetDataUnitSize(void);
	//-----------Eof-INdxDataUnit

	//-----------INdxFile
	///читает из файла по текущей позиции
	void Read(unsigned char* pBuffer, unsigned int uiSize);
	///пишет в файл по текущей позиции
	void Write(unsigned char* pBuffer, unsigned int uiSize);
	///добавляет к файлу
	void Append(unsigned char* pBuffer, unsigned int uiSize);
	///изменяет текущую позицию в файле
	void SeekPos(unsigned int uiOffset, unsigned int uiOrigin);
	///возвращает текущую позицию в файле
	unsigned int TellPos(void);
	///резервирует место в файле
	void Reserve(unsigned int uiSize);
	///переименовывает, перемещает файл
	bool Rename(const wchar_t* pszNewName, const wchar_t* pszPath);
	///проверяет хендл файла на валидность
	bool	IsBad(void);
	///возвращает хендл файла
	HANDLE	GetFileHandle(void){return m_FHandle;};
	//-----------Eof-INdxFile
	bool IsCDROM(VOID);

	///отбражает содержимое объекта класса
	virtual void	View(void);
	///отбражает текущую позицию в файле
	virtual void	ViewFilePos(void);


protected:
	///файловый хендл
    HANDLE	m_FHandle;

};

//--------------------------------------------------------------------//

#pragma warning( default : 4250 )

}
}
}
}