#pragma once

#include "..\ASCInterface\IInterface.h"
#include "..\ASCInterface\IDBMS.h"

//--------------------------------------------------------------------//

// {3BFF5350-43FE-4cdc-BE3B-AFBA7E313F6A}
static const GUID IID_NdxDataUnit = 
{ 0x3bff5350, 0x43fe, 0x4cdc, { 0xbe, 0x3b, 0xaf, 0xba, 0x7e, 0x31, 0x3f, 0x6a } };
// {9347830D-AC34-494a-96CC-77612F37D0FD}
static const GUID IID_NdxFile = 
{ 0x9347830d, 0xac34, 0x494a, { 0x96, 0xcc, 0x77, 0x61, 0x2f, 0x37, 0xd0, 0xfd } };
// {688048D0-6852-44f8-B65D-69973100E22D}
static const GUID IID_NdxCashedFile = 
{ 0x688048d0, 0x6852, 0x44f8, { 0xb6, 0x5d, 0x69, 0x97, 0x31, 0x0, 0xe2, 0x2d } };
// {3E8E2375-D5F8-4d2e-8596-273A356C987A}
static const GUID IID_NdxLevelStorage = 
{ 0x3e8e2375, 0xd5f8, 0x4d2e, { 0x85, 0x96, 0x27, 0x3a, 0x35, 0x6c, 0x98, 0x7a } };
// {184C06D1-01F3-41a6-99AF-48A661C7B63E}
static const GUID IID_NdxStorage = 
{ 0x184c06d1, 0x1f3, 0x41a6, { 0x99, 0xaf, 0x48, 0xa6, 0x61, 0xc7, 0xb6, 0x3e } };
// {7B0FD902-A48A-471d-A0FA-EF5ED4509CEC}
static const GUID IID_NdxVectorStorage = 
{ 0x7b0fd902, 0xa48a, 0x471d, { 0xa0, 0xfa, 0xef, 0x5e, 0xd4, 0x50, 0x9c, 0xec } };
// {85974587-0B3C-4e3e-8AEF-71EFE7269E1B}
static const GUID IID_NdxTableStorage = 
{ 0x85974587, 0xb3c, 0x4e3e, { 0x8a, 0xef, 0x71, 0xef, 0xe7, 0x26, 0x9e, 0x1b } };

// {CB80571B-F1C3-4987-81DF-E10D91C65AD2}
static const GUID IID_NdxDataStorageFactory = 
{ 0xcb80571b, 0xf1c3, 0x4987, { 0x81, 0xdf, 0xe1, 0xd, 0x91, 0xc6, 0x5a, 0xd2 } };
// {4864DAD8-7C86-495a-AF37-25B4E2E7DAF0}
static const GUID CLSID_NdxDataStorageFactory  = 
{ 0x4864dad8, 0x7c86, 0x495a, { 0xaf, 0x37, 0x25, 0xb4, 0xe2, 0xe7, 0xda, 0xf0 } };

//--------------------------------------------------------------------//

namespace SS
{
namespace Interface
{
namespace Core
{
namespace NdxSE
{
namespace NdxDataStorage
{

//--------------------------------------------------------------------//

///параметры бакапирования
struct SBackupSettings
{
	SBackupSettings(void):m_CompressMode(cmCompressible), m_WriteMode(wmRandom){};

	///режим сжатия файла при бакапе
	enum ECompressMode
	{
		///файл можно сжимать
		cmCompressible,
		///файл несжимаемый
		cmUncompressible,
	} m_CompressMode;
	
	///режим записи в файл
	enum EWriteMode
	{
		///случайно
		wmRandom,
		///добавление в конец
		wmAppend,
	} m_WriteMode;
	
	void ToString(std::wstring& wsValue)
	{
		wsValue.assign(L"_");

		if(m_WriteMode==wmRandom)
			wsValue.append(L"r");
		else if(m_WriteMode==wmAppend)
			wsValue.append(L"a");

		if(m_CompressMode==cmCompressible)
			wsValue.append(L"c");
		else if(m_CompressMode==cmUncompressible)
			wsValue.append(L"u");
	};
};

//--------------------------------------------------------------------//

///интерфейс работы с базовым юнитом данных индексной базы
class INdxDataUnit : public virtual SS::Interface::IBase
{
public:				
	///создает юнит данных 
	virtual void Create(const wchar_t* pszDataUnitName, const wchar_t* pszDataUnitExt) = 0;
	///открывает юнит данных в текущем режиме
	virtual bool Open(const wchar_t* pszPath) = 0;
	///закрывает юнит данных 
	virtual bool Close(void) = 0;
	///сбрасывает юнит данных 
	virtual bool Flush(void) = 0;
	///очищает  юнит данных 
	virtual void Clear(void) = 0;
	///возвращает размер юнита данных 
	virtual unsigned int GetDataUnitSize(void) = 0;
	///проверяет открыт ли юнит данных 
	virtual bool IsOpen(void) = 0;
	///возвращает имя юнита данных 
	virtual const wchar_t*	GetDataUnitName(void) = 0;
	///возвращает расширение юнита данных 
	virtual const wchar_t* GetDataUnitExt(void) = 0;
	///устанавливает настройки бакапирования
	virtual void SetBackupSettings(SBackupSettings::ECompressMode CompressMode, SBackupSettings::EWriteMode WriteMode) = 0;
};

//--------------------------------------------------------------------//


///интерфейс работы с базовым индексным файлом
class INdxFile : public virtual INdxDataUnit
{
public:				
	///читает из файла по текущей позиции
	virtual void Read(unsigned char* pBuffer, unsigned int uiSize) = 0;
	///пишет в файл по текущей позиции
	virtual void Write(unsigned char* pBuffer, unsigned int uiSize) = 0;
	///добавляет к файлу
	virtual void Append(unsigned char* pBuffer, unsigned int uiSize) = 0;
	///изменяет текущую позицию в файле
	virtual void SeekPos(unsigned int uiOffset, unsigned int uiOrigin) = 0;
	///возвращает текущую позицию в файле
	virtual unsigned int TellPos(void) = 0;
	///резервирует место в файле
	virtual void Reserve(unsigned int uiSize) = 0;
	///переименовывает, перемещает файл
	virtual bool Rename(const wchar_t* pszNewName, const wchar_t* pszPath) = 0;
	///проверяет хендл файла на валидность
	virtual bool IsBad(void) = 0;
	///возвращает хендл файла
	virtual HANDLE	GetFileHandle(void) = 0;
};

//--------------------------------------------------------------------//

///интерфейс работы с индексным файлом поддерживающим кэширование чтение-записи
class INdxCashedFile : public virtual INdxFile
{
public:
	///инициализирует параметры кэша
	virtual void InitCash(unsigned int uiCashBufferSize, unsigned int uiCashMinSizeToFlush) = 0;
	///кэширует файл
	virtual void Cashed(void) = 0;
	///сбрасывает на диск заполненные буфера кэша файла
	virtual void FlushFilledBuffers(void) = 0;
};

//--------------------------------------------------------------------//

///хранение смещения на блок данных, с учетом разнесенности по нескольким файлам
#pragma pack(push,1)  // Fix packing size 
struct SCompositeOffset
{
	///смещение в файле
	unsigned int uiOffset;
	///индекс файла
	unsigned char ucFileIndex;

	///оператор меньше
	bool operator<(const SCompositeOffset& RightValue) const
	{
		if(ucFileIndex==RightValue.ucFileIndex){
			return uiOffset<RightValue.uiOffset?true:false;
		}
		return ucFileIndex<RightValue.ucFileIndex?true:false;
	};

};
#pragma pack(pop)

//--------------------------------------------------------------------//

///информация по индексному уровню
struct SNdxLevelInfo
{
	///тип координаты индекса в тексте
	enum EIndexCoordinateType
	{
		///номер предложения в коллекции
		ictSentenceAbsNumber,
		///номер текста в коллекции
		ictTextAbsNumber,
		///номер параграфа в тексте
		ictParagraphNumber,
		///номер предложения в параграфе
		ictSentenceNumber,
		///номер предложения в тексте
		ictSentenceInTextNumber,
		///номер слова в предложении
		ictWordNumber,
		///номер слова в тексте
		ictWordInTextNumber,
	};

	///тип контроля уровня (тип контролирования, и подконтрольности)
	enum ELevelControlType
	{
		//неопределено
		lctUndefined,	
		//контроль номеров слов	 
		lctWords,
		//контроль количества предложений
		lctSentences,	
		//контроль координат
		lctCoordinates	
	};

	///номер уровня
	unsigned char m_ucLevelNumber;
	///тип координаты, которую хранит уровень
	EIndexCoordinateType m_IndexCoordinateType;
	///бит начала уровня внутри координаты
	unsigned char m_ucStartBit;
	///количество битов координаты на уровень
	unsigned char m_ucBitPerLevel;
	///тип того, что контролирует данный уровень
	ELevelControlType m_eControlType;
	///тип того, чем контролируется данный уровень
	ELevelControlType m_eControlByType;
};

//--------------------------------------------------------------------//
///интерфейс работы с хранилищем уровня индексных данных
class INdxLevelStorage : public virtual INdxDataUnit
{
public:		
	///перемещение по хранилищy индексных данных
	virtual void	SeekPos(SCompositeOffset* pOffset, unsigned int uiOrigin) = 0;
	///возвращает позицию в хранилище индексных данных
	virtual void	TellPos(SCompositeOffset* pOffset) = 0;

	//сессионное чтение из хранилища индексных данных
	///начинает чтение по данной позиции
	virtual bool	StartRead(SCompositeOffset* pOffset, unsigned int* puiDataSizeInFile) = 0;
	///читает в буфер
	virtual void	Read(unsigned char* pBuffer, unsigned int uiSize) = 0;
	///заканчивает чтение, возвращает размер прочитанного
	virtual void	StopRead(unsigned int* puiReadedDataSize) = 0;
	
	//сессионное добавление в хранилище индексных данных
	///начинает добавление неизменяемых данных
	virtual void	StartCompleteDataAppend(SCompositeOffset* pOffset) = 0;
	///начинает добавление изменяемых данных
	virtual void	StartUnCompleteDataAppend(SCompositeOffset* pOffset) = 0;
	///добавление буфера данных
	virtual void	Append(unsigned char* pBuffer, unsigned int uiSize) = 0;
	///заканчивает добавление, возвращает размер добавленного
	virtual void	StopAppend(unsigned int* puiAppendedDataSize) = 0;

	///возвращает размер добавленых изменяемых данных
	virtual unsigned int GetAppendedUnCompleteDataSize(void) = 0;
	///позиционирует в добавленых изменяемых данных
	virtual void	SeekPosInAppendedUnCompleteData(unsigned int uiOffset, unsigned int uiOrigin) = 0;
	///пишет буфер данных в добавленые изменяемых данных
	virtual void	WriteInAppendedUnCompleteData(unsigned char* pBuffer, unsigned int uiSize) = 0;

	///удаление из коллекции файлов
	virtual void	Delete(SCompositeOffset* pOffset) = 0;
	
	///обновляет данные в хранилище индексных данных
	virtual void	UpdateData(void) = 0;
};

//--------------------------------------------------------------------//

///интерфейс работы с табличным хранилищем данных
class INdxTableStorage : public virtual INdxDataUnit
{
public:				
	typedef std::vector<SS::Interface::Core::DBMS::TDataTableFieldDescription> TTableFieldCollection;

	///добавляет поле в таблицу
	virtual HRESULT AddField(SS::Interface::Core::DBMS::TDataTableFieldDescription* pDataTableFieldDescription) = 0;
	///возвращает коллекцию полей таблицы
	virtual TTableFieldCollection* GetFieldsCollection(void) = 0;

	///инициализирует процесс поиска в базе
	virtual HRESULT InitFind(wchar_t* pKeyName, void* pKeyValue, void* ArrayOfPointersToData[], unsigned short* pusFindId = 0) = 0;
	///ищет в таблице несколько раз по неуникальным значениям пока поиск не закончится неудачей
	virtual HRESULT Find(unsigned int uiFindId = 0) = 0;
	///ищет в таблице один раз по уникальному индексу
	virtual HRESULT FindOne(unsigned int uiFindId = 0) = 0;

	///инициализирует процесс интервального поиска в базе
	virtual HRESULT SpanFindInit(wchar_t* pKeyName, void* pLowerKeyValue, void* pUpperKeyValue, 
		std::set<unsigned int>* pSetOfRecordsIndexes, unsigned short* pusFindId = 0) = 0;
	///ищет в таблице записи удовлетворяющий SpanFindInit
	virtual HRESULT SpanFind(unsigned int uiFindId = 0) = 0;

	///возвращает индекс записи таблицы найденой перед вызовом данного метода
	virtual unsigned int GetFoundRecordIndex(unsigned int uiFindId = 0) = 0;
	///удаляет запись таблицы по индексу
	virtual void DeleteRecord(unsigned int uiRecordIndex) = 0;
	///инициализирует процесс сканирования таблицы
	virtual HRESULT InitScan(long nStartRecordIdx, void* ArrayOfPointersToData[]) = 0;
	///возвращает очередную запись таблицы
	virtual HRESULT Scan(void) = 0;
	///возвращает запись таблицы по индексу
	virtual HRESULT ScanOne(unsigned int uiRecordIndex) = 0;
	///инициализирует процесса добавления данных в таблицу.
	virtual HRESULT InitAdd(void* ArrayOfPointersToData[]) = 0;
	///добавляет запись к таблице
	virtual HRESULT Add(void) = 0;
	///инициализирует процесса изменения всех полей таблицы.
	virtual HRESULT InitModify(void* ArrayOfPointersToData[]) = 0;
	///находит и изменяет запись в таблице
	virtual HRESULT FindOneAndChange(void) = 0;
};

//--------------------------------------------------------------------//

///интерфейс работы с векторным хранилищем данных
class INdxVectorStorage : public virtual INdxDataUnit
{
public:				
	//сессионное чтение из векторного хранилища данных
	///смещается на начало данных по индексу, возвращает их размер
	virtual void	MoveToAndGetDataSize(unsigned int uiIndex, unsigned int* puiDataSizeInFile) = 0;
	///читает все данные находящиеся по индексу
	virtual void	ReadWholeData(unsigned char* pData, unsigned int uiDataSize) = 0;
	///читает кусок данных находящихся по индексу
	virtual void	ReadDataChunk(unsigned char* pData, unsigned int uiDataSize) = 0;
	///смещает внутри данных находящихся по индексу, смещение считается от начала
	virtual void	SeekInData(unsigned int uiOffset) = 0;
	
	//сессионное добавление в векторное хранилище данных
	///добавляет данные в хранилище, возвращает индекс, по которому будут находятся данные
	virtual void	AppendWholeData(unsigned char* pData, unsigned int uiDataSize, unsigned int* puiIndex) = 0;
	///добавляет пакет данных, запускает сценарий пакетного добавления
	virtual void	AppendDataChunk(unsigned char* pData, unsigned int uiDataSize) = 0;
	///завершает сценарий пакетного добавления, возвращает индекс, по которому будут находятся данные
	virtual void	AppendDataChunkComplete(unsigned int* puiIndex) = 0;
	
	///удаляет данные по индексу
	virtual void	Delete(unsigned int uiIndex) = 0;
	///проверяет удалены ли данные по индексу
	virtual bool	IsDeleted(unsigned int uiIndex) = 0;

	///возвращает количество индексов
	virtual unsigned int GetIndexesNumber(void) = 0;
};

//--------------------------------------------------------------------//

///интерфейс работы с хранилищем индексных данных
class INdxStorage : public virtual INdxDataUnit
{
public:
	///возвращает хранилище точек входы в индекс
	virtual INdxTableStorage* GetEntryPointsStorage(void) = 0;
	///возвращает хранилище образов документов
	virtual INdxVectorStorage* GetTextImagesStorage(void) = 0;
	///возвращает хранилища образов документов
	virtual void GetTextImagesStorages(INdxFile** ppNdxTextsOffsets, INdxFile** ppNdxTextBlocksOffsets, INdxVectorStorage** ppNdxTextBlocksContent) = 0;
	///возвращает хранилище координат
	virtual INdxLevelStorage* GetCoordinateStorage(void) = 0;

	///добавляет информацию по координатному уровню
	virtual void AddLevelInfo(SNdxLevelInfo* pNdxLevelInfo) = 0;
	///возвращает информацию по координатному уровню по номеру
	virtual SNdxLevelInfo const* GetLevelInfo(unsigned int uiLevelNumber) = 0;
	///возвращает число координатных уровней
	virtual unsigned int GetLevelsNumber(void) = 0;
};

//--------------------------------------------------------------------//

///интерфейс работы с хранилищем индексных данных
class INdxStorageWithStatistic : public virtual INdxStorage
{
public:
	///возвращает хранилище частот индексов
	virtual INdxVectorStorage* GetIndexFrequencyStorage(void) = 0;
	///возвращает хранилище норм документов индекса
	virtual INdxFile* GetDocumentNormStorage(void) = 0;
};

//--------------------------------------------------------------------//

class INdxDataStorageFactory : public virtual SS::Interface::IBase
{
public:				
	///создает базовый индексный файл
	virtual INdxFile* CreateNdxFile(void) = 0;
	///создает индексный файл поддерживающий кэширование чтение-записи
	virtual INdxCashedFile* CreateNdxCashedFile(void) = 0; 
	///создает табличное хранилище данных
	virtual INdxTableStorage* CreateNdxTableStorage(void) = 0;
	///создает векторное хранилище данных
	virtual INdxVectorStorage* CreateNdxVectorStorage(void) = 0;
	///создает хранилище индексных данных
	virtual INdxStorage* CreateNdxStorage(void) = 0;
	///создает хранилище индексных данных с поддержкой статистики
	virtual INdxStorageWithStatistic* CreateNdxStorageWithStatistic(void) = 0;

};

//--------------------------------------------------------------------//

}
}
}
}
}