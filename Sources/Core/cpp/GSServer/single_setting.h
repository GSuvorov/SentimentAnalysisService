#pragma once

#include ".\basis_setting.h"

namespace SS
{
namespace CommonServices
{

//----------------------------------------------------------------//

///одиночная настройка
template <class T>
class CSetting : public CBasisSetting
{
public:
	CSetting(const wchar_t* wszSettingName, SSettingType SettingType)
		:CBasisSetting(wszSettingName, SettingType){};

	///переводит настройку в бинарный вид
	virtual void ToBinary(std::vector<unsigned char>* pBuffer)
	{
		if(!pBuffer) return;
		
		//название
		m_SettingName.ToBinary(pBuffer);

		//аттрибуты
		unsigned int uiOldSize=(unsigned int)pBuffer->size();
		pBuffer->resize(pBuffer->size() + sizeof(SSettingAttributes));
		unsigned char* pCurrPos=&(pBuffer->front())+uiOldSize;
		memcpy(pCurrPos, (unsigned char*)&m_SettingAttributes, sizeof(SSettingAttributes));
		
		//значение
		m_SettingValue.ToBinary(pBuffer);
	};

	///наполняет настройку по буферу
	virtual void FromBinary(unsigned char* pucBuffer, unsigned int uiBufferSize, unsigned int* puiReadedSize)
	{
		if(!pucBuffer) return;
		if(!uiBufferSize) return;
		if(!puiReadedSize) return;

		(*puiReadedSize)=0;

		//название
		(*puiReadedSize)+=m_SettingName.FromBinary(pucBuffer, uiBufferSize);

		//аттрибуты
		memcpy((unsigned char*)&m_SettingAttributes, pucBuffer+(*puiReadedSize), sizeof(SSettingAttributes));
		(*puiReadedSize)+=sizeof(SSettingAttributes);
		
		//значение
		(*puiReadedSize)+=m_SettingValue.FromBinary(pucBuffer+(*puiReadedSize), uiBufferSize-(*puiReadedSize));
	};

	///возвращает значение настройки
	bool GetSettingValue(void* pOutValue, 
		SS::Interface::Core::CommonServices::EParameterType eSettingType, bool bIsArray)
	{
		//проверяем тип
		if(!(eSettingType==m_SettingAttributes.m_SettingType.m_eSettingType && 
			bIsArray==m_SettingAttributes.m_SettingType.m_bIsArray))
			return false;

		//копируем значение настройки
		m_SettingValue.CopyTo(pOutValue);
		return true;
	};

	///устанавливает значение настройки
	bool SetSettingValue(void* pInValue, 
		SS::Interface::Core::CommonServices::EParameterType eSettingType, bool bIsArray)
	{
		//проверяем тип
		if(eSettingType!=m_SettingAttributes.m_SettingType.m_eSettingType)
			return false;

		if(m_SettingAttributes.m_SettingType.m_bIsArray==bIsArray){
			//копируем значение настройки
			m_SettingValue.CopyFrom(pInValue);
			return true;
		}else if(m_SettingAttributes.m_SettingType.m_bIsArray && !bIsArray){
			//если настройка массив, а добавляется простое значение 
			//добавляем его к массиву настройки
			m_SettingValue.CopyFrom(pInValue, true);
			return true;
		}else{
			return false;
		}
	};
	
	///копирует значение настройки
	void CopyValue(CBasisSetting* pSetting)
	{
		if(!pSetting) return;
		CSetting<T>* p=static_cast<CSetting<T>* >(pSetting);
		m_SettingValue=p->m_SettingValue;
	}

protected:
	///значение настройки
	T m_SettingValue;
};

//----------------------------------------------------------------//

}
}
