#include "StdAfx.h"
#include ".\norm_storage.h"
#include ".\console.h"
#include ".\const.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Storages
{

using namespace SS::Interface::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//

CDocNormStorage::CDocNormStorage(INdxFile* pNdxDocNormStorage)
:m_pNdxDocNormStorage(pNdxDocNormStorage), m_uiTotalWordsCount(0)
{
	if(ISNULL(m_pNdxDocNormStorage)) return;

	if(!m_pNdxDocNormStorage->IsOpen()){
		TO_CONSOLE(L"NdxDocNormStorage not opened");
		ToConsole(m_pNdxDocNormStorage->GetDataUnitName());
		return;
	}
}

unsigned int CDocNormStorage::GetDocNorm(unsigned int uiDocId)
{
	unsigned int uiDocNorm=0;

	if(ISNULL(m_pNdxDocNormStorage)) return uiDocNorm;

	if(!m_pNdxDocNormStorage->IsOpen()){
		TO_CONSOLE(L"NdxDocNormStorage not opened");
		ToConsole(m_pNdxDocNormStorage->GetDataUnitName());
		return uiDocNorm;
	}
	
	//сдвигаем на один так как в позиции 0 лежит количестиво документов
	unsigned int uiPos=(uiDocId+1)*sizeof(unsigned int);

	if(uiPos>=m_pNdxDocNormStorage->GetDataUnitSize()){
		TO_CONSOLE(L"Too big uiDocId");
		return uiDocNorm;
	}

	//позиционируемся на нужную норму по идентификатору текста
	m_pNdxDocNormStorage->SeekPos(uiPos, SEEK_SET);
	
	//читаем норму
	m_pNdxDocNormStorage->Read((unsigned char*)&uiDocNorm, sizeof(unsigned int));
	
	return uiDocNorm;
}

void CDocNormStorage::AddDocNorm(unsigned int uiDocNorm)
{
	if(ISNULL(m_pNdxDocNormStorage)) return;

	if(!m_pNdxDocNormStorage->IsOpen()){
		TO_CONSOLE(L"NdxDocNormStorage not opened");
		ToConsole(m_pNdxDocNormStorage->GetDataUnitName());
		return;
	}
	
	//обновляем общее количество слов
	if(uiDocNorm!=EMPTY_VALUE){ 
		unsigned int uiTotalWordsCount=0;
		if(m_pNdxDocNormStorage->GetDataUnitSize()!=0){
			m_pNdxDocNormStorage->SeekPos(0, SEEK_SET);
			m_pNdxDocNormStorage->Read((unsigned char*)&uiTotalWordsCount, sizeof(unsigned int));
		}else{
			m_pNdxDocNormStorage->Append((unsigned char*)&uiTotalWordsCount, sizeof(unsigned int));
		}
		uiTotalWordsCount+=uiDocNorm;
		m_pNdxDocNormStorage->SeekPos(0, SEEK_SET);
		m_pNdxDocNormStorage->Write((unsigned char*)&uiTotalWordsCount, sizeof(unsigned int));
	}

	//добавляем норму
	m_pNdxDocNormStorage->Append((unsigned char*)&uiDocNorm, sizeof(unsigned int));
}

unsigned int CDocNormStorage::GetTotalWordsCount(void)
{
	unsigned int uiTotalWordsCount;
	m_pNdxDocNormStorage->SeekPos(0, SEEK_SET);
	m_pNdxDocNormStorage->Read((unsigned char*)&uiTotalWordsCount, sizeof(unsigned int));
	return uiTotalWordsCount;
}

//--------------------------------------------------------------------//

}
}
}
}
}