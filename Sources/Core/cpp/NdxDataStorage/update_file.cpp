#include "StdAfx.h"
#include ".\update_file.h"
#include ".\data_storages_factory.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxDataStorage
{

using namespace SS::Interface::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//

CUpdateFile::CUpdateFile(CDataUnitFactory* pDataStorageFactory) :
	CFilesCollection(pDataStorageFactory),
	m_cuiMaxWorkedDataSize(cuiMaxWorkedBoundDataSize), m_pDataFile(NULL), m_pOffsetFile(NULL),
	m_cuiDeletedDataPercentageToShrink(200), m_cuiDataSizeToShrink(1000000000), m_uiDeletedOffsetsNumber(0),
	m_uiMaxWorkedDataSize(0), m_uiWorkedDataSize(0)
{
}

CUpdateFile::~CUpdateFile(void)
{
}

void CUpdateFile::ViewFilePos(void)
{
	if(ISNULL(GetOffsetsFile())) return;
	if(ISNULL(GetDataFile())) return;
	if(ISNULL(GetFreeOffsetsFile())) return;
	
	//GetOffsetsFile()->ViewFilePos();
	//GetDataFile()->ViewFilePos();
	//GetFreeOffsetsFile()->View();
}

bool CUpdateFile::Open(const wchar_t* pszPath)
{
	if(ISNULL(pszPath)) return false;

	//создаем файл смещений
	AddFile();
	//файл свободных смещений
	AddFile();
	//создаем файл данных
	AddFile();
	
	return CFilesCollection::Open(pszPath);
}

bool CUpdateFile::Close(void)
{
	//перед закрытием шринкуем
	if(m_uiDeletedOffsetsNumber!=0) Shrink();
	
	//закрываем как обычно
	return CFilesCollection::Close();
}

void CUpdateFile::GenerateFileName(unsigned int uiFileIndex, wstring* pszFileName)
{
	pszFileName->assign(GetDataUnitName());
	switch((EUpdateStorageFiles)uiFileIndex){
	case eusfOffsets:
		pszFileName->append(L"_off");
		break;
	case eusfFreeOffsets:
		pszFileName->append(L"_of~");
		break;
	case eusfData:
		pszFileName->append(L"_d");
		break;
	default:
		pszFileName->append(L"_udf");
		TO_CONSOLE(L"Unknown index!!!");
		break;
	}
}

INdxDataUnit* CUpdateFile::CreateFile(unsigned int uiFileIndex)
{
	wstring szNewFileName;
	//генерируем имя файла
	GenerateFileName(uiFileIndex, &szNewFileName);

	if(ISNULL(GetDataStorageFactory())) return NULL;

	switch((EUpdateStorageFiles)uiFileIndex){
	case eusfOffsets: case eusfFreeOffsets:
		{
			//создаем файл
			INdxFile* pFile=GetDataStorageFactory()->CreateBasisFile();
			pFile->Create(szNewFileName.c_str(), GetDataUnitExt());
			return pFile;
		}
		break;
	case eusfData:
		{
			//создаем файл
			INdxCashedFile* pFile=GetDataStorageFactory()->CreateCashedFile();
			pFile->Create(szNewFileName.c_str(), GetDataUnitExt());
			return pFile;
		}
		break;
	default:
		TO_CONSOLE(L"Unknown index!!!");
		return NULL;
		break;
	}
}

bool CUpdateFile::Flush(void)
{
	if(ISNULL(GetOffsetsFile())) return false;
	if(ISNULL(GetDataFile())) return false;
	if(ISNULL(GetDataStorageFactory())) return false;
	
	unsigned int uiTotalOffsetsNumber=GetOffsetsFile()->GetDataUnitSize()/sizeof(SDataInfo);
	unsigned int uiDeletedDataPercentage=
		(unsigned int)(((float)m_uiDeletedOffsetsNumber)/(((float)uiTotalOffsetsNumber)/100));

	if(m_uiDeletedOffsetsNumber!=0 &&
		(m_cuiDataSizeToShrink<GetDataFile()->GetDataUnitSize() || 
		m_cuiDeletedDataPercentageToShrink<uiDeletedDataPercentage)){
		//если выполняются критерии, шринкуем
		Shrink();
		ToConsole(L"-------------Shrink-------------");
		ToConsole(L"DataFileSize", GetDataFile()->GetDataUnitSize());
		ToConsole(L"DeletedDataPercentage", uiDeletedDataPercentage);
	}else{
		//если критерии не выполняются, сбрасываем буфера в файлы
		//сбрасываем на диск системные буфера файла смещений
		GetOffsetsFile()->Flush();
		GetDataFile()->Flush();
		//пишем на диск свободные смещения
		MoveDownFreeOffsets();
	}

	return true;
}

bool CUpdateFile::Shrink(void)
{
	//ToConsole(L"CUpdateFile::Shrink");
	//ToConsole(GetDataUnitName().c_str());
	
	if(ISNULL(GetOffsetsFile())) return false;
	if(ISNULL(GetDataFile())) return false;
	if(ISNULL(GetDataStorageFactory())) return false;
	
	try{
		if(!GetOffsetsFile()->GetDataUnitSize()){
			//если смещений нет чистимся и выходим
			Clear();
			return true;
		}
		
		//создаем и открываем новый файл данных
		INdxCashedFile* pNewDataFile=CreateAndOpenNewDataFile();

		//получаем файл смещений
		INdxFile* pOffsetsFile=GetOffsetsFile();
		TDataInfoCollection DataInfoCollection;
		unsigned int uiMaxDataInfoCount=100000;
		unsigned int uiTemp=0;
		unsigned int uiAvailableDataInfoCount=0;
		unsigned int uiOffsetFileSize=pOffsetsFile->GetDataUnitSize();
		//в цикле читаем блоки смещений и шринкуем по ним данные
		unsigned int uiCurrentPos=0;
		pOffsetsFile->SeekPos(0, SEEK_SET);
		do{
			uiAvailableDataInfoCount=(uiOffsetFileSize-uiCurrentPos)/sizeof(SDataInfo);
			if(uiAvailableDataInfoCount>uiMaxDataInfoCount) uiAvailableDataInfoCount=uiMaxDataInfoCount;
			
			//резервируем буфер
			DataInfoCollection.resize(uiAvailableDataInfoCount);
			uiTemp=uiAvailableDataInfoCount*sizeof(SDataInfo);

			//читаем смещения
			pOffsetsFile->Read((unsigned char*)&DataInfoCollection.front(), uiTemp);

			//шринкуем
			ShrinkData(&DataInfoCollection, GetDataFile(), pNewDataFile);

			//переписываем обновленную информацию по смещениям
			pOffsetsFile->SeekPos(uiCurrentPos, SEEK_SET);
			pOffsetsFile->Write((unsigned char*)&DataInfoCollection.front(), uiTemp);
			
			//обновляем позицию
			uiCurrentPos=pOffsetsFile->TellPos();
		}while(uiCurrentPos<uiOffsetFileSize);

		//сбрасываем на диск системные буфера файла смещений
		GetOffsetsFile()->Flush();
		//пишем на диск свободные смещения
		MoveDownFreeOffsets();
		m_uiDeletedOffsetsNumber=0;
		//подменяем файл данных вновь сформированным
		pNewDataFile->Flush();
		ReplaceDataFile(pNewDataFile);
		return true;
	
	}catch(...){
		//удаляем буфер для упорядоченных данных
		ERR_TO_CONSOLE(erTryCatch, L" ");
		throw;
	}
}

void CUpdateFile::ReplaceDataFile(INdxCashedFile* pNewDataFile)
{
	if(ISNULL(pNewDataFile)) return;

	GetDataFile()->Clear();
	GetDataFile()->Close();
	if(!pNewDataFile->Rename(GetDataFile()->GetDataUnitName(), m_szCurrentFilePath.c_str()))
		TO_CONSOLE(L"Rename failed!!!");
	
	GetDataFile()->Release();
	m_FilesCollection[eusfData]=pNewDataFile;
	m_pDataFile=pNewDataFile;
}

INdxCashedFile* CUpdateFile::CreateAndOpenNewDataFile(void)
{
	//создаем и открываем новый файл под данные
	wstring szNewDataFile(GetDataFile()->GetDataUnitName());
	szNewDataFile.append(L"~");
	INdxCashedFile* pNewDataFile=GetDataStorageFactory()->CreateCashedFile();
	pNewDataFile->Create(szNewDataFile.c_str(), GetDataUnitExt());
	unsigned int uiNewDataFileCurrCashSize=0;
	if(!pNewDataFile->Open(m_szCurrentFilePath.c_str())){
		pNewDataFile->Release();
		pNewDataFile=NULL;
		TO_CONSOLE(L"Cann't open new bound data file");
		ToConsole(szNewDataFile.c_str());
	}

	return pNewDataFile;
}

void CUpdateFile::ShrinkData(TDataInfoCollection* pDataInfoCollection, INdxCashedFile* pDataFile, INdxCashedFile* pNewDataFile)
{
	if(ISNULL(pDataInfoCollection)) return;
	if(ISNULL(pDataFile)) return;
	if(ISNULL(pNewDataFile)) return;
	
	TDataInfoCollection::iterator itDataInfo=pDataInfoCollection->begin();
	unsigned int uiNewDataOffset=EMPTY_OFFSET;
	unsigned char* pucData=NULL;
	while(itDataInfo!=pDataInfoCollection->end()){
		
		if(itDataInfo->m_uiDataOffset!=EMPTY_OFFSET){
			//если смещение валидное, работаем с ним

			//пишем в файл смещений новое смещение на данные
			uiNewDataOffset=pNewDataFile->GetDataUnitSize();

			//смещаемся на данные
			GetDataFile()->SeekPos(itDataInfo->m_uiDataOffset, SEEK_SET);
			
			//выделяем буфер под данные
			pucData=(unsigned char*)malloc(itDataInfo->m_uiDataSize);

			//читаем данные из файла в буфер
			GetDataFile()->Read(pucData, itDataInfo->m_uiDataSize);
			
			//добавляем данные в новый файл данных
			pNewDataFile->Append(pucData, itDataInfo->m_uiDataSize);
			
			//обновляем смещение на данные
			itDataInfo->m_uiDataOffset=uiNewDataOffset;
	
			//освобождаем буфер под данные
			if(pucData) free(pucData);
		}else{
			//для невалидных данных сбрасываем размер
			itDataInfo->m_uiDataSize=EMPTY_VALUE;
		}
		
		//переходим к следующему смещению
		itDataInfo++;
	}
}

void	CUpdateFile::Clear(void)
{
	CFilesCollection::Clear();

	m_FreeOffsets.clear();
	m_uiMaxWorkedDataSize=0;
	m_uiWorkedDataSize=0;
}

void CUpdateFile::MoveUpFreeOffsets(void)
{
	if(ISNULL(GetFreeOffsetsFile())) return;
	INdxFile* pFreeOffsetsFile=GetFreeOffsetsFile();
	unsigned int uiFileSize=pFreeOffsetsFile->GetDataUnitSize();

	if(uiFileSize){
		pFreeOffsetsFile->SeekPos(0, SEEK_SET);

		pFreeOffsetsFile->Read((unsigned char*)&m_uiDeletedOffsetsNumber, sizeof(unsigned int));
		uiFileSize-=sizeof(unsigned int);
		
		if(uiFileSize!=0){
			//читаем коллекцию свободных смещений
			m_FreeOffsets.resize(uiFileSize/sizeof(unsigned int));	
			pFreeOffsetsFile->Read((unsigned char*)&m_FreeOffsets.front(), uiFileSize);
		}
	}
}

void CUpdateFile::MoveDownFreeOffsets(void)
{
	if(ISNULL(GetFreeOffsetsFile())) return;
	INdxFile* pFreeOffsetsFile=GetFreeOffsetsFile();
	pFreeOffsetsFile->Clear();

	pFreeOffsetsFile->Write((unsigned char*)&m_uiDeletedOffsetsNumber, sizeof(unsigned int));

	//пишем коллекцию свободных смещений
	if(m_FreeOffsets.size()){
		pFreeOffsetsFile->Write((unsigned char*)&m_FreeOffsets.front(), (unsigned int)(m_FreeOffsets.size()*sizeof(unsigned int)));
	}
	
	pFreeOffsetsFile->Flush();
}

void CUpdateFile::AddFreeOffset(unsigned int uiOffset)
{
	if(uiOffset!=EMPTY_OFFSET){
		m_FreeOffsets.push_back(uiOffset);
		//ToConsole(L"AddFreeOffset", (unsigned int)m_FreeOffsets.size());
	}
}

unsigned int CUpdateFile::SeekToFreeOffset(void)
{
	if(ISNULL(GetOffsetsFile())) return EMPTY_OFFSET;
	unsigned int uiOffset=EMPTY_OFFSET;

	if(m_FreeOffsets.empty()){
		//пока новое смещение только в конце
		uiOffset=GetOffsetsFile()->GetDataUnitSize();
	}else{
		uiOffset=m_FreeOffsets.back();
		m_FreeOffsets.pop_back();
		//ToConsole(L"SeekToFreeOffset", (unsigned int)m_FreeOffsets.size());
	}

	GetOffsetsFile()->SeekPos(uiOffset, SEEK_SET);	
	return uiOffset;
}

void	CUpdateFile::StartRead(unsigned int uiOffset, unsigned int* puiDataSizeInFile)
{
	if(ISNULL(GetOffsetsFile())) return;
	if(ISNULL(GetDataFile())) return;
	if(ISNULL(puiDataSizeInFile)) return;

	unsigned int uiDataOffset=EMPTY_OFFSET;
	unsigned int uiReadedDataSize=EMPTY_VALUE;
	unsigned char ucStep=0;

	try{
	
		START_TICK(L"SeekPos");
		//смещаемся по входному смещению
		GetOffsetsFile()->SeekPos(uiOffset, SEEK_SET);
		ucStep++;

		//читаем смещение на данные в файле данных
		GetOffsetsFile()->Read((unsigned char*)&uiDataOffset, sizeof(unsigned int));
		ucStep++;

		//читаем размер данных в файле данных
		GetOffsetsFile()->Read((unsigned char*)&uiReadedDataSize, sizeof(unsigned int));
		ucStep++;
		STOP_TICK(L"SeekPos");
		
		//инитим размеры для чтения
		m_uiMaxWorkedDataSize=uiReadedDataSize;
		m_uiWorkedDataSize=0;
		*puiDataSizeInFile=uiReadedDataSize;
		ucStep++;
		
		START_TICK(L"DataFile_SeekPos");
		//смещаемся на данные
		GetDataFile()->SeekPos(uiDataOffset, SEEK_SET);
		STOP_TICK(L"DataFile_SeekPos");
		ucStep++;
	
	}catch(...){
		wchar_t buf[500]=L"";
		switch(ucStep){
		case 0:
			wcscpy(buf, L"GetOffsetsFile()->SeekPos");
			break;
		case 1:
			wcscpy(buf, L"GetOffsetsFile()->Read1");
			break;
		case 2:
			wcscpy(buf, L"GetOffsetsFile()->Read2");
			break;
		case 3:
			wcscpy(buf, L"*puiDataSizeInFile=uiReadedDataSize");
			break;
		case 4:
			wcscpy(buf, L"GetDataFile()->SeekPos");
			break;
		default:
			wcscpy(buf, L"Unknown place");
			break;
		}
		ERR_TO_CONSOLE(erTryCatch, buf);
		ToConsole(GetDataUnitName());	
		//GetDataFile()->ViewFilePos();
		//GetOffsetsFile()->ViewFilePos();
		ToConsole(L"uiOffset", uiOffset);	
		ToConsole(L"uiDataOffset", uiDataOffset);	
		ToConsole(L"uiReadedDataSize", uiReadedDataSize);	
		ToConsole(L"m_uiMaxWorkedDataSize", m_uiMaxWorkedDataSize);	

		throw;
	}
}

void	CUpdateFile::Read(unsigned char* pData, unsigned int uiDataSize)
{
	if(ISNULL(GetDataFile())) return;
	if(ISNULL(pData)) return;
	if(ISNULL(uiDataSize)) return;

	if(ISNULL(m_uiMaxWorkedDataSize)){
		ERR_TO_CONSOLE(erFileIO, L"StartRead was not called!!!");
		return;
	}
	
	if(m_uiMaxWorkedDataSize<=m_uiWorkedDataSize){ 
		ERR_TO_CONSOLE(erFileIO, L"No more Data!!!");
		ToConsole(L"m_uiMaxWorkedDataSize", m_uiMaxWorkedDataSize);
		ToConsole(L"m_uiWorkedDataSize", m_uiWorkedDataSize);
		ToConsole(L"GetOffsetsFile()", GetOffsetsFile()->TellPos());
		ToConsole(L"GetDataFile()", GetDataFile()->TellPos());
		ToConsole(GetDataUnitName());
		return;
	}

	try{

		GetDataFile()->Read(pData, uiDataSize);
		m_uiWorkedDataSize+=uiDataSize;

	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, GetDataUnitName());
		//GetDataFile()->ViewFilePos();
		ToConsole(pData, uiDataSize);	
		ToConsole(L"uiDataSize", uiDataSize);	
		ToConsole(L"m_uiMaxWorkedDataSize", m_uiMaxWorkedDataSize);	
		ToConsole(L"m_uiWorkedDataSize", m_uiWorkedDataSize);	
		throw;
	}

}

void	CUpdateFile::StopRead(unsigned int* puiReadedDataSize)
{
	if(ISNULL(puiReadedDataSize)) return;

	if(ISNULL(m_uiMaxWorkedDataSize)){
		ERR_TO_CONSOLE(erFileIO, L"StartRead was not called!!!");
		return;
	}
	
	*puiReadedDataSize=m_uiWorkedDataSize;

	//сбрасываем размеры
	m_uiMaxWorkedDataSize=0;
	m_uiWorkedDataSize=0;
}

void	CUpdateFile::StartAppend(unsigned int* puiOffset)
{
	if(ISNULL(GetOffsetsFile())) return;
	if(ISNULL(GetDataFile())) return;
	if(ISNULL(puiOffset)) return;

	m_uiMaxWorkedDataSize=m_cuiMaxWorkedDataSize;
	m_uiWorkedDataSize=0;
	unsigned int uiDataOffset=EMPTY_OFFSET;	
	
	try{

		//получаем свободное смещение в файле смещений и позиционируемся на нем
		(*puiOffset)=SeekToFreeOffset();

		//определяем смещение данных
		uiDataOffset=GetDataFile()->GetDataUnitSize();
		
		//заносим смещение на данные в файл смещений
		GetOffsetsFile()->Write((unsigned char*)&uiDataOffset, sizeof(unsigned int));

	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, GetDataUnitName());
		//GetDataFile()->ViewFilePos();
		//GetOffsetsFile()->ViewFilePos();
		ToConsole(L"uiDataOffset", uiDataOffset);	
		ToConsole(L"puiOffset", *puiOffset);	
		throw;
	}

}

void	CUpdateFile::Append(unsigned char* pData, unsigned int uiDataSize)
{
	if(ISNULL(GetDataFile())) return;
	if(ISNULL(pData)) return;
	if(ISNULL(uiDataSize)) return;

	if(ISNULL(m_uiMaxWorkedDataSize)){
		ERR_TO_CONSOLE(erFileIO, L"StartAppend was not called!!!");
		return;
	}
	
	if(m_uiMaxWorkedDataSize<=m_uiWorkedDataSize){ 
		ERR_TO_CONSOLE(erFileIO, L"Too much appended data!!!");
		return;
	}

	try{
	
		GetDataFile()->Append(pData, uiDataSize);
		m_uiWorkedDataSize+=uiDataSize;

	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, GetDataUnitName());
		//GetDataFile()->ViewFilePos();
		ToConsole(pData, uiDataSize);	
		ToConsole(L"uiDataSize", uiDataSize);	
		ToConsole(L"m_uiMaxWorkedDataSize", m_uiMaxWorkedDataSize);	
		ToConsole(L"m_uiWorkedDataSize", m_uiWorkedDataSize);	
		throw;
	}
}

void	CUpdateFile::StopAppend(unsigned int* puiAppendedDataSize)
{
	if(ISNULL(GetOffsetsFile())) return;
	if(ISNULL(puiAppendedDataSize)) return;

	if(ISNULL(m_uiMaxWorkedDataSize)){
		ERR_TO_CONSOLE(erFileIO, L"StartAppend was not called!!!");
		return;
	}
	try{

		//заносим размер добавленных данных в файл смещений
		GetOffsetsFile()->Write((unsigned char*)&m_uiWorkedDataSize, sizeof(unsigned int));
		*puiAppendedDataSize=m_uiWorkedDataSize;

		//сбрасываем размеры
		m_uiMaxWorkedDataSize=0;
		m_uiWorkedDataSize=0;

	}catch(...){
		//сбрасываем размеры
		m_uiMaxWorkedDataSize=0;
		m_uiWorkedDataSize=0;

		ERR_TO_CONSOLE(erTryCatch, GetDataUnitName());
		//GetOffsetsFile()->ViewFilePos();
		ToConsole(L"m_uiWorkedDataSize", m_uiWorkedDataSize);	
		throw;
	}
}

void CUpdateFile::DeleteData(unsigned int uiOffset)
{
	if(uiOffset==EMPTY_OFFSET) return;

	unsigned int uiDataOffset=EMPTY_OFFSET;
	unsigned int uiDataSize=EMPTY_VALUE;
	
	try{

		//получаем реальное смещение на данные
		GetOffsetsFile()->SeekPos(uiOffset, SEEK_SET);

		//выставляем пустым смещение на данные и их размер
		GetOffsetsFile()->Write((unsigned char*)&uiDataOffset, sizeof(unsigned int));

		//обновляем размер удаленных данных
		GetOffsetsFile()->Read((unsigned char*)&uiDataSize, sizeof(unsigned int));
		++m_uiDeletedOffsetsNumber;

		//добавляем смещение как свободное
		AddFreeOffset(uiOffset);

	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, GetDataUnitName());
		//GetOffsetsFile()->ViewFilePos();
		ToConsole(L"uiOffset", uiOffset);	
		throw;
	}

}

void	CUpdateFile::SeekPosInData(unsigned int uiOffset, unsigned int uiOrigin)
{
	if(ISNULL(GetDataFile())) return;
	if(uiOffset==EMPTY_OFFSET){
		TO_CONSOLE(L"uiOffset==EMPTY_OFFSET!!!");
		return;
	}

	GetDataFile()->SeekPos(uiOffset, uiOrigin);
}

void	CUpdateFile::WriteData(unsigned char* pBuffer, unsigned int uiSize)
{
	if(ISNULL(GetDataFile())) return;
	if(ISNULL(pBuffer)) return;
	
	GetDataFile()->Write(pBuffer, uiSize);

}

//--------------------------------------------------------------------//

}
}
}
}