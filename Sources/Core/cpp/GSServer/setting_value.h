#pragma once

namespace SS
{
namespace CommonServices
{

//----------------------------------------------------------------//

///одиночное значение настройки
template <class T>
class CSingleValue
{
public:
	///копирует значение по входному
	virtual void CopyFrom(void* pValue, bool bAdd = false){if(pValue) m_Value=*((T*)pValue);};
	///копирует значение во входное
	virtual void CopyTo(void* pValue){if(pValue) *((T*)pValue)=m_Value;};
	
	virtual void operator=(const CSingleValue& Value)
	{
		m_Value=Value.m_Value;
	};

	///переводит значение в бинарный вид
	virtual void ToBinary(std::vector<unsigned char>* pBuffer)
	{
		if(!pBuffer) return;

		unsigned int uiOldSize=(unsigned int)pBuffer->size();
		pBuffer->resize(pBuffer->size()+sizeof(T));
		unsigned char* pCurrPos=&(pBuffer->front())+uiOldSize;

		//значение
		memcpy(pCurrPos, (void*)&m_Value, sizeof(T));
	};

	///наполняет значение по буферу
	virtual unsigned int FromBinary(unsigned char* pucBuffer, unsigned int uiBufferSize)
	{
		if(!pucBuffer) return 0;
		if(!uiBufferSize) return 0;
		if(sizeof(T)>uiBufferSize) return 0;
	
		//значение
		memcpy((void*)&m_Value, pucBuffer, sizeof(T));
		return sizeof(T);
	};

protected:
	///значение настройки
	T m_Value;
};

//----------------------------------------------------------------//

///строковая настройка
class CStringValue : public CSingleValue<std::wstring>
{
public:
	///переводит значение в бинарный вид
	void ToBinary(std::vector<unsigned char>* pBuffer)
	{
		if(!pBuffer) return;

		unsigned int uiOldSize=(unsigned int)pBuffer->size();
		pBuffer->resize(pBuffer->size()+m_Value.size()*sizeof(wchar_t));
		unsigned char* pCurrPos=&(pBuffer->front())+uiOldSize;

		//значение
		memcpy(pCurrPos, (void*)m_Value.c_str(), m_Value.size()*sizeof(wchar_t));
		//кладем завершающий 0
		pBuffer->push_back(0);
		pBuffer->push_back(0);
	};
	///наполняет значение по буферу
	unsigned int FromBinary(unsigned char* pucBuffer, unsigned int uiBufferSize)
	{
		if(!pucBuffer) return 0;
		if(!uiBufferSize) return 0;
		m_Value.assign((const wchar_t*)pucBuffer);
		return GetBinarySize();
	};
	
	///возвращает указатель на свое значение
	std::wstring* GetValue(void){return &m_Value;};
	///возвращает бинарный размер значения
	unsigned int GetBinarySize(void){return (unsigned int)m_Value.size()*sizeof(wchar_t)+2;};
};

//----------------------------------------------------------------//

///значение массив-значений заданного типа
template <class T>
class CArrayValue : public CSingleValue< std::vector<T> >
{
public:
	///копирует значение по входному
	void CopyFrom(void* pValue, bool bAdd = false)
	{
		if(!pValue) return;

		//проверяем флаг добавления
		if(!bAdd){
			m_Value.assign(((std::vector<T> *)pValue)->begin(), ((std::vector<T> *)pValue)->end());
		}else{
			m_Value.push_back(*((T*)pValue));
		}
	};
	///копирует значение во входное
	void CopyTo(void* pValue)
	{
		if(pValue)
		{
				((std::vector<T> *)pValue)->assign(m_Value.begin(), m_Value.end());
		}
	};

	void operator=(const CArrayValue& Value)
	{
		m_Value.assign(Value.m_Value.begin(), Value.m_Value.end());
	};

	///переводит значение в бинарный вид
	void ToBinary(std::vector<unsigned char>* pBuffer)
	{
		if(!pBuffer) return;

		unsigned int uiCount=(unsigned int)m_Value.size();
		unsigned int uiBinarySize=sizeof(T)*uiCount+sizeof(unsigned int);
		unsigned int uiOldSize=(unsigned int)pBuffer->size();
		pBuffer->resize(pBuffer->size()+uiBinarySize);
		unsigned char* pCurrPos=&(pBuffer->front())+uiOldSize;

		//значение
		memcpy(pCurrPos, (void*)&uiCount, sizeof(unsigned int));
		pCurrPos+=sizeof(unsigned int);
		memcpy(pCurrPos, (void*)&m_Value[0], sizeof(T)*uiCount);
	};

	///наполняет значение по буферу
	unsigned int FromBinary(unsigned char* pucBuffer, unsigned int uiBufferSize)
	{
		if(!pucBuffer) return 0;
		if(!uiBufferSize) return 0;

		unsigned char* pCurrPos=pucBuffer;
		unsigned int uiCount=0;
		memcpy((void*)&uiCount, pCurrPos, sizeof(unsigned int));
		pCurrPos+=sizeof(unsigned int);

		if(sizeof(T)*uiCount>uiBufferSize) return 0;
		
		if(uiCount!=0){
			//значение
			m_Value.resize(uiCount);
			memcpy((void*)&m_Value[0], pCurrPos, sizeof(T)*uiCount);
		}

		return sizeof(T)*uiCount + sizeof(unsigned int);
	};
	
};

//----------------------------------------------------------------//

///значение массив строк
class CStringArrayValue : public CArrayValue<CStringValue>
{
public:
	///копирует значение по входному
	void CopyFrom(void* pValue, bool bAdd = false)
	{
		if(!pValue) return;
	
		if(!bAdd){
			std::vector<std::wstring>* pStringArray=(std::vector<std::wstring>*)pValue;	
			m_Value.clear();
			for(unsigned int i=0; i<pStringArray->size(); i++){
				m_Value.push_back(CStringValue());
				m_Value.back().GetValue()->assign(pStringArray->at(i));
			}
		}else{
			m_Value.push_back(CStringValue());
			m_Value.back().GetValue()->assign(((std::wstring*)pValue)->c_str());
		}
	};
	///копирует значение во входное
	void CopyTo(void* pValue)
	{
		if(!pValue) return;
		std::vector<std::wstring>* pStringArray=(std::vector<std::wstring>*)pValue;	
		pStringArray->clear();
		for(unsigned int i=0; i<m_Value.size(); i++){
			pStringArray->push_back(std::wstring());
			pStringArray->back().assign(*m_Value[i].GetValue());
		}

	};

	///переводит значение в бинарный вид
	void ToBinary(std::vector<unsigned char>* pBuffer)
	{
		if(!pBuffer) return;

		unsigned int uiBinarySize=sizeof(unsigned int);
		unsigned int uiOldSize=(unsigned int)pBuffer->size();
		pBuffer->resize(pBuffer->size()+uiBinarySize);
		unsigned char* pCurrPos=&(pBuffer->front())+uiOldSize;

		//значение
		unsigned int uiCount=(unsigned int)m_Value.size();
		memcpy(pCurrPos, (void*)&uiCount, sizeof(unsigned int));
		for(unsigned int i=0; i<uiCount; i++){
			m_Value[i].ToBinary(pBuffer);
		}
	};

	///наполняет значение по буферу
	unsigned int FromBinary(unsigned char* pucBuffer, unsigned int uiBufferSize)
	{
		if(!pucBuffer) return 0;
		if(!uiBufferSize) return 0;

		int iLeftSize=uiBufferSize;
		unsigned char* pCurrPos=pucBuffer;
		unsigned int uiCount=0;
		memcpy((void*)&uiCount, pCurrPos, sizeof(unsigned int));
		pCurrPos+=sizeof(unsigned int);
		iLeftSize-=sizeof(unsigned int);
		
		//значение
		unsigned int uiReadedSize=0;
		unsigned int i=0; 
		if(uiCount){
			do{
				m_Value.push_back(CStringValue());
				uiReadedSize=m_Value.back().FromBinary(pCurrPos, (unsigned int)iLeftSize);
				pCurrPos+=uiReadedSize;
				iLeftSize-=(int)uiReadedSize;
			}while(++i<uiCount && iLeftSize>0);
		}
		return uiBufferSize-(unsigned int)iLeftSize;
	};
	
};

//----------------------------------------------------------------//



}
}