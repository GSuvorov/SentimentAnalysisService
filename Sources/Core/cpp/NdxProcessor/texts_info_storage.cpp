#include "StdAfx.h"
#include ".\texts_info_storage.h"
#include ".\const.h"
#include ".\console.h"

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

using namespace SS::Core::NdxSE::NdxProcessor;
using namespace SS::Interface::Core::NdxSE::NdxDataStorage;
using namespace SS::Interface::Core::CommonContainers;

//--------------------------------------------------------------------//

CTextsInfoStorage::CTextsInfoStorage(INdxVectorStorage* pNdxVectorStorage)
:m_pNdxVectorStorage(pNdxVectorStorage)
{
}

CTextsInfoStorage::~CTextsInfoStorage(void)
{
}

unsigned int CTextsInfoStorage::AddTextInfo(ITextFeature* pTextFeature)
{
	if(ISNULL(m_pNdxVectorStorage)) return EMPTY_VALUE;
	

	if(!m_pNdxVectorStorage->IsOpen()){
		TO_CONSOLE(L"TextsInfo not opened!!!");
		return 0;
	}
	
	unsigned char* pTextInfoBuffer=NULL;
	unsigned int uiTextInfoBufferSize=0;
	unsigned int uiTextInfoIndex=EMPTY_VALUE;

	if(pTextFeature){
		//устанавливаем номер текста текстфичи преде добавлением
		pTextFeature->SetTextNumber(m_pNdxVectorStorage->GetIndexesNumber());

		//получаем буфер инфы по тексту
		pTextFeature->GetTextFeatureBuff((char**)&pTextInfoBuffer, &uiTextInfoBufferSize);
		
		if(ISNULL(pTextInfoBuffer)) return EMPTY_VALUE;
		if(ISNULL(uiTextInfoBufferSize)) return EMPTY_VALUE;
		
		//добавляем буфер в файл
		m_pNdxVectorStorage->AppendWholeData(pTextInfoBuffer, uiTextInfoBufferSize, &uiTextInfoIndex);
	}else{
		//для пустой фичи
		unsigned int uiTemp=EMPTY_VALUE;
		m_pNdxVectorStorage->AppendWholeData((unsigned char*)&uiTemp, sizeof(unsigned int), &uiTextInfoIndex);
	}

	return uiTextInfoIndex;
}

void CTextsInfoStorage::GetTextInfo(unsigned int uiTextIndex, ITextFeature* pTextFeature)
{
	if(ISNULL(pTextFeature)) return;
	if(ISNULL(m_pNdxVectorStorage)) return;

	if(!m_pNdxVectorStorage->IsOpen()){
		TO_CONSOLE(L"TextsInfo not opened!!!");
		return;
	}
	
	if(uiTextIndex==EMPTY_VALUE){
		TO_CONSOLE(L"Invalid text index!");
		return;
	}

	unsigned char* pTextInfoBuffer=NULL;
	unsigned int uiTextInfoBufferSize=0;

	//узнаем требуемый размер буфера
	m_pNdxVectorStorage->MoveToAndGetDataSize(uiTextIndex, &uiTextInfoBufferSize);
	
	if(uiTextInfoBufferSize){
		//если размер валидный читаем данные
		pTextInfoBuffer=new unsigned char[uiTextInfoBufferSize];
		m_pNdxVectorStorage->ReadWholeData(pTextInfoBuffer, uiTextInfoBufferSize);	
	}else{
		return;
	}
	

	if(uiTextInfoBufferSize==sizeof(unsigned int) && (*(unsigned int*)pTextInfoBuffer)==EMPTY_VALUE){
		//если эта буфер пустой фичи
		//устанавливаем фиче номер текста и все
		pTextFeature->SetTextNumber(uiTextIndex);
	}else{
		//заполняем инфу по тексту из буфера
		pTextFeature->SetTextFeatureBuff((char*)pTextInfoBuffer, uiTextInfoBufferSize);
		
		if(pTextFeature->GetTextNumber()!=uiTextIndex){
			ERR_TO_CONSOLE(erNotEqual, L"TextFeature Synchronize error!!! TextFeature TextNumber not equal with input");
			ToConsole(L"TextFeature", pTextFeature->GetTextNumber());
			ToConsole(L"uiTextIndex", uiTextIndex);
			pTextFeature->SetTextNumber(uiTextIndex);
		}
	}

	//удаляем временный буфер
	if(pTextInfoBuffer) delete pTextInfoBuffer;
}

void CTextsInfoStorage::DeleteText(unsigned int uiTextIndex)
{
	if(!m_pNdxVectorStorage->IsOpen()){
		TO_CONSOLE(L"TextsInfo not opened!!!");
		return;
	}

	if(uiTextIndex==EMPTY_VALUE){
		TO_CONSOLE(L"Invalid text index!");
		return;
	}

	//удаляем по индексу
	m_pNdxVectorStorage->Delete(uiTextIndex);
}

bool CTextsInfoStorage::IsTextDeleted(unsigned int uiTextIndex)
{
	if(!m_pNdxVectorStorage->IsOpen()){
		TO_CONSOLE(L"TextsInfo not opened!!!");
		return true;
	}

	if(uiTextIndex==EMPTY_VALUE){
		TO_CONSOLE(L"Invalid text index!");
		return true;
	}

	return m_pNdxVectorStorage->IsDeleted(uiTextIndex);
}

//--------------------------------------------------------------------//

}
}
}
}
}