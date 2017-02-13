#include "StdAfx.h"
#include ".\cashed_files.h"

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

CACashedFile::CACashedFile(CDataUnitFactory* pDataStorageFactory)
:CBasisFile(pDataStorageFactory), m_IndexModeFileCash(cuiCommonFileBufferSize, cuiMaxBufferNumber), 
m_uiCurrOffset(EMPTY_OFFSET), 
m_cuiCashMinSizeToFlush(cuiCashMinSizeToFlush)
{
}

CACashedFile::~CACashedFile(void)
{
}

HRESULT CACashedFile::QueryInterface(REFIID pIID, void** pBase)
{
	if(pIID==IID_NdxFile){
		*pBase=(INdxFile*)this;
	}else if(pIID==IID_NdxCashedFile){
		*pBase=(INdxCashedFile*)this;
	}else{
		*pBase=NULL;
	}

	return S_OK;
}

bool CACashedFile::Open(const wchar_t* pszPath)
{
	if(CBasisFile::Open(pszPath)){
		m_IndexModeFileCash.Init(this);
		m_IndexModeFileCash.SetFileOffset(CBasisFile::GetDataUnitSize());
		return true;
	}else{
		return false;
	}
}

bool CACashedFile::Close(void)
{
	//сбрасываем все что осталось
	m_IndexModeFileCash.FlushToFile();
	m_uiCurrOffset=EMPTY_OFFSET;

	return CBasisFile::Close();
}

bool CACashedFile::Flush(void)
{
	if(m_IndexModeFileCash.GetFilledSize()>=m_cuiCashMinSizeToFlush){
		//если кэш набрал нужный размер, сбрасываем его в файл
		m_IndexModeFileCash.FlushToFile();
	}

	//теперь все на диск
	return CBasisFile::Flush();
}

void CACashedFile::Append(unsigned char* pBuffer, unsigned int uiSize)
{
	//добавляем данные в кэш
	m_IndexModeFileCash.AddData(pBuffer, uiSize);
	m_uiCurrOffset=m_IndexModeFileCash.GetFileOffset()+m_IndexModeFileCash.TellPos();
}

void	CACashedFile::Read(unsigned char* pBuffer, unsigned int uiSize)
{
	//проверяем в какой части файла находятся запрашиваемые данные
	if(m_IndexModeFileCash.GetFileOffset()>TellPos()){
		//если текущая позиция находится в файле

		//определяем сколько данных есть в файле
		unsigned int uiAvailableFileSize=m_IndexModeFileCash.GetFileOffset()-TellPos();
		
		if(uiAvailableFileSize>=uiSize){
			//если данных достаточно, читаем их из файла
			CBasisFile::Read(pBuffer, uiSize);
			//смещаем текущую позицию
			m_uiCurrOffset+=uiSize;
		}else{
			//если данных не достаточно, читаем их из файла, а затем из кэша
			CBasisFile::Read(pBuffer, uiAvailableFileSize);

			//читаем остальное из кэша
			m_IndexModeFileCash.SeekPos(0, SEEK_SET);
			m_IndexModeFileCash.Read(pBuffer+uiAvailableFileSize, uiSize-uiAvailableFileSize);

			//смещаем текущую позицию
			m_uiCurrOffset=m_IndexModeFileCash.GetFileOffset()+m_IndexModeFileCash.TellPos();
		}

	}else{
		//если текущая позиция находится в кэше
		//читаем в кэше
		m_IndexModeFileCash.Read(pBuffer, uiSize);
		//смещаем текущую позицию
		m_uiCurrOffset=m_IndexModeFileCash.GetFileOffset()+m_IndexModeFileCash.TellPos();
	}
}

void CACashedFile::Write(unsigned char* pBuffer, unsigned int uiSize)
{
	//проверяем в какой части файла находятся запрашиваемые данные
	if(m_IndexModeFileCash.GetFileOffset()>TellPos()){
		//если текущая позиция находится в файле

		//определяем сколько данных есть в файле
		unsigned int uiAvailableFileSize=m_IndexModeFileCash.GetFileOffset()-TellPos();
		
		if(uiAvailableFileSize>=uiSize){
			//если данных достаточно, пишем буфер в файл
			CBasisFile::Write(pBuffer, uiSize);
			//смещаем текущую позицию
			m_uiCurrOffset+=uiSize;
		}else{
			//если данных не достаточно, пишем в файл кусок, а затем в кэш
			CBasisFile::Write(pBuffer, uiAvailableFileSize);
			//пишем остальное в кэш
			m_IndexModeFileCash.SeekPos(0, SEEK_SET);
			m_IndexModeFileCash.Write(pBuffer+uiAvailableFileSize, uiSize-uiAvailableFileSize);
			//смещаем текущую позицию
			m_uiCurrOffset=m_IndexModeFileCash.GetFileOffset()+m_IndexModeFileCash.TellPos();
		}
	}else{
		//если текущая позиция находится в кэше
		//пишем в кэш
		m_IndexModeFileCash.Write(pBuffer, uiSize);
		//смещаем текущую позицию
		m_uiCurrOffset=m_IndexModeFileCash.GetFileOffset()+m_IndexModeFileCash.TellPos();
	}
}

void	CACashedFile::SeekPos(unsigned int uiOffset, unsigned int uiOrigin)
{
	if(uiOffset==EMPTY_OFFSET){
		ERR_TO_CONSOLE(erFileIO, L"Invalid offset!!!");
		ToConsole(L"uiOrigin", uiOrigin);
		ToConsole(GetDataUnitName());
		return;
	}

	switch(uiOrigin){
	case SEEK_SET:
		//абсолютное позиционирование от начала файла
		if(uiOffset>=m_IndexModeFileCash.GetFileOffset()){
			//если требуемая позиция находится в кэше, позиционируемся в нем	
			m_IndexModeFileCash.SeekPos(uiOffset-m_IndexModeFileCash.GetFileOffset(), SEEK_SET);
		}else{
			//если требуемая позиция находится в файле, позиционируемся в нем	
			CBasisFile::SeekPos(uiOffset, SEEK_SET);
		}

		//обновляем текущую позицию в файле
		m_uiCurrOffset=uiOffset;
		break;
	case SEEK_CUR:
		//относительное позиционирование от текущей позиции в файле
		if(m_uiCurrOffset!=EMPTY_OFFSET){
			//если текущая позиция в файле валидна...
			//определяем будущую абсолюную позицию от начала файла
			unsigned int uiNewOffset=(unsigned int)((int)m_uiCurrOffset+(int)uiOffset);

			if(uiNewOffset>=m_IndexModeFileCash.GetFileOffset()){
				//если требуемая позиция находится в кэше, позиционируемся в нем	
				//(смещение файла не вычитается так как смещение относительное)
				m_IndexModeFileCash.SeekPos(uiOffset, SEEK_CUR);
			}else{
				//если требуемая позиция находится в файле, позиционируемся в нем	
				CBasisFile::SeekPos(uiOffset, SEEK_CUR);
			}

			//обновляем текущую позицию в файле
			m_uiCurrOffset=uiNewOffset;
		}
		break;
	case SEEK_END:
		ERR_TO_CONSOLE(erFileIO, L"Not supported");
		break;
	default:
		ERR_TO_CONSOLE(erFileIO, L"Undefined origin!!!");
		break;
	}
}

void CACashedFile::Clear(void)
{
	CBasisFile::Clear();
	m_IndexModeFileCash.Clear();
	m_IndexModeFileCash.SetFileOffset(0);
	m_uiCurrOffset=0;
}

unsigned int CACashedFile::GetDataUnitSize()
{
	return (m_IndexModeFileCash.GetFileOffset()+m_IndexModeFileCash.GetFilledSize());
}

void	CACashedFile::FlushFilledBuffers(void)
{
	//сбрасываем заполненые буфера кэша
	m_IndexModeFileCash.FlushFilledBuffersToFile();
}

void CACashedFile::View(void)
{
	m_IndexModeFileCash.View();
	CBasisFile::View();
}

void CACashedFile::ViewFilePos(void)
{
	m_IndexModeFileCash.View();
	CBasisFile::View();
}

//--------------------------------------------------------------------//

CRACashedFile::CRACashedFile(CDataUnitFactory* pDataStorageFactory)
:CACashedFile(pDataStorageFactory), m_ReadFileCash(cuiReadFileBufferSize)
{
}

CRACashedFile::~CRACashedFile(void)
{
}

bool	CRACashedFile::Open(const wchar_t* pszPath)
{
	if(CACashedFile::Open(pszPath)){
		m_ReadFileCash.Init(this);
		m_ReadFileCash.SetFileOffset(0);
		return true;
	}else{
		return false;
	}
}

bool	CRACashedFile::Close(void)
{
	m_ReadFileCash.Clear();
	return CACashedFile::Close();
}

//void	CRACashedFile::Read(unsigned char* pBuffer, unsigned int uiSize)
//{
//	//проверяем в какой части файла находятся запрашиваемые данные
//	if(m_IndexModeFileCash.GetFileOffset()>TellPos()){
//		//если текущая позиция находится в файле
//
//		//определяем сколько данных есть в файле
//		unsigned int uiAvailableFileSize=CBasisFile::GetDataUnitSize()-TellPos();
//		
//		if(uiAvailableFileSize>=uiSize){
//			//если данных достаточно, читаем их из файла
//			ReadFromCash(pBuffer, uiSize);
//			//смещаем текущую позицию
//			m_uiCurrOffset+=uiSize;
//		}else{
//			//если данных не достаточно, читаем их из файла, а затем из кэша добавления
//			ReadFromCash(pBuffer, uiAvailableFileSize);
//			//читаем остальное из кэша добавления
//			m_IndexModeFileCash.SeekPos(0, SEEK_SET);
//			m_IndexModeFileCash.Read(pBuffer+uiAvailableFileSize, uiSize-uiAvailableFileSize);
//			//смещаем текущую позицию
//			m_uiCurrOffset=m_IndexModeFileCash.GetFileOffset()+m_IndexModeFileCash.TellPos();
//		}
//	}else{
//		//если текущая позиция находится в индексном кэше
//		//читаем из кэша
//		m_IndexModeFileCash.Read(pBuffer, uiSize);
//		//смещаем текущую позицию
//		m_uiCurrOffset=m_IndexModeFileCash.GetFileOffset()+m_IndexModeFileCash.TellPos();
//	}
//}
//
//void CRACashedFile::ReadFromCash(unsigned char* pBuffer, unsigned int uiSize)
//{
//	//читаем из кэша
//	unsigned int uiDataSizeInCash=0;
//	unsigned int uiWorkedSize=0;
//	unsigned char* pucCurrBufPos=pBuffer;
//	do{
//		//получаем размер данных в кэше
//		uiDataSizeInCash=m_ReadFileCash.GetFilledSize()-m_ReadFileCash.TellPos();
//		
//		if(uiWorkedSize+uiDataSizeInCash<uiSize){
//			//если в кэше находится меньше, чем нужно буферу
//			//читает столько сколько есть в кэше
//			if(uiDataSizeInCash){
//				m_ReadFileCash.Read(pucCurrBufPos, uiDataSizeInCash);
//				pucCurrBufPos+=uiDataSizeInCash;
//				uiWorkedSize+=uiDataSizeInCash;
//			}
//			
//			//сдвигаемся в файле и кэшируемся заново
//			m_ReadFileCash.SetFileOffset(m_ReadFileCash.GetFileOffset()+m_ReadFileCash.GetFilledSize());
//			m_ReadFileCash.FillFromFile();
//		}else{
//			//если в кэше находится достаточно, чтобы можно было прочитать буфер, читаем
//			m_ReadFileCash.Read(pucCurrBufPos, uiSize-uiWorkedSize);
//			//выставляем размер чтоб выйти
//			uiWorkedSize=uiSize;
//		}
//	}while(uiWorkedSize<uiSize);
//}
//
//void	CRACashedFile::SeekPos(unsigned int uiOffset, unsigned int uiOrigin)
//{
//	if(uiOffset==EMPTY_OFFSET){
//		TO_CONSOLE(L"Invalid offset!!!");
//		ToConsole(L"uiOrigin", uiOrigin);
//		ToConsole(GetDataUnitName());
//		return;
//	}
//
//	switch(uiOrigin){
//	case SEEK_SET:
//
//		//абсолютное позиционирование от начала файла
//		if(uiOffset>=m_IndexModeFileCash.GetFileOffset()){
//			//если требуемая позиция находится в кэше, позиционируемся в нем	
//			m_IndexModeFileCash.SeekPos(uiOffset-m_IndexModeFileCash.GetFileOffset(), SEEK_SET);
//		}else{
//			//если требуемая позиция находится в файле, позиционируемся в нем	
//			CBasisFile::SeekPos(uiOffset, SEEK_SET);
//			//даем позицию кэшу чтения
//			m_ReadFileCash.SetFileOffset(uiOffset);
//			//заполняем кэш чтения данными из файла
//			m_ReadFileCash.FillFromFile();
//		}
//
//		//обновляем текущую позицию в файле
//		m_uiCurrOffset=uiOffset;
//		break;
//	case SEEK_CUR:
//		//относительное позиционирование от текущей позиции в файле
//		if(m_uiCurrOffset!=EMPTY_OFFSET){
//			//если текущая позиция в файле валидна...
//			//определяем будущую абсолюную позицию от начала файла
//			unsigned int uiNewOffset=(unsigned int)((int)m_uiCurrOffset+(int)uiOffset);
//
//			if(uiNewOffset>=m_IndexModeFileCash.GetFileOffset()){
//				//если требуемая позиция находится в кэше, позиционируемся в нем	
//				//(смещение файла не вычитается так как смещение относительное)
//				m_IndexModeFileCash.SeekPos(uiOffset, SEEK_CUR);
//			}else{
//				//если требуемая позиция находится в файле, позиционируемся в нем	
//				CBasisFile::SeekPos(uiOffset, SEEK_CUR);
//				//даем позицию кэшу чтения
//				m_ReadFileCash.SetFileOffset(uiNewOffset);
//				//заполняем кэш чтения данными из файла
//				m_ReadFileCash.FillFromFile();
//			}
//
//			//обновляем текущую позицию в файле
//			m_uiCurrOffset=uiNewOffset;
//		}
//		break;
//	case SEEK_END:
//		TO_CONSOLE(L"Not supported");
//		break;
//	default:
//		TO_CONSOLE(L"Undefined origin!!!");
//		break;
//	}
//}

//--------------------------------------------------------------------//

}
}
}
}
