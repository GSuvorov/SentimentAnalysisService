#pragma once

#include "..\ASCInterface\IDBMS.h"
#include <string>

namespace SS
{
namespace Interface
{
namespace Core
{
namespace NdxSE
{

//--------------------------------------------------------------------//

///настройки поиска поискового движка
class CNdxSearchSettings
{
public:
	///настройки поиска
	struct SSearchSettings{ 
		SSearchSettings()
			: m_StorageUsingMode(esumAll)
			, m_SearchMode(esmBooleanSearch)
			, m_EvalRelevanceMode(erWordProximityWithStatistic)
		{
		}
		///режимы поиска
		enum EStorageUsingMode
		{
			///поиск в одном хранилище обычный
			esumSingleStorage,
			///поиск c учетом текстов и заголовков
			esumHeadersAndTextStorage,
			///поиск c учетом таблиц и заголовков
			esumHeadersAndTablesStorage,
			///все режимы вместе
			esumAll
		} m_StorageUsingMode;

		///режимы поиска
		enum ESearchMode
		{
			///неопределен
			esmUndefined,
			///булев поиск
			esmBooleanSearch,
			///нечеткий поиск
			esmFuzzySearch
		} m_SearchMode;

		///режим вычисления релевантности
		enum EEvalRelevanceMode
		{
			///неопределен
			erUndefined,
			///статистика по текстам
			erTextsStatistic,
			///близость слов
			erWordProximity,
			///близость слов с учетом статистики
			erWordProximityWithStatistic,
			///вычислять всю релевантность
			erAll
		} m_EvalRelevanceMode;
	} m_SearchSettings;

	///настройки шаблонного поиска
	struct SPatternSearchSettings{ 
		///флаг учета последовательности индексов в интервалах, можно принудительно отключать учет последовательность
		bool m_bCheckIndexSequence;
		///пороговый вес по близости
		unsigned int m_uiIndexProximityLimitWeight;
		///необходимая часть от общего веса слов в процентах, 
		///которую должно образовать слова объединения в результате поиска
		unsigned int m_uiUnionLimitWordWeightPart;
		///максимальное количество "лишних слов", среди слов объединения
		unsigned int m_uiUnionSurplusWordsMaxCount;
		//при помощи коэффициентов можно повышать либо понижать вклад определенного веса в результат
		///коэффициент веса близости слов объединения
		unsigned int m_uiUnionDistanceWeightKoef;
		///коэффициент суммарного веса слов объединения
		unsigned int m_uiUnionWordsWeightKoef;
		///коэффициент общего веса объединения, он повышает роль веса объединений в общем весе результата
		unsigned int m_uiUnionWeightKoef;
		///предельный вес с которого слово объединения, становится обязательным
		unsigned int m_uiGrandWordWeightLimit;
	} m_PatternSearchSettings;

	///настройки поиска с учетом статистики
	struct SSmartSearchSettings{
		///нормирующий веса слов коэффициент
		float m_fWordsNormKoef;
		///пороговое значение редкого слова
		float m_fRarityWordLimit;
		///значение кворума редких слов, необходимый процент редких слов в результате поиска
		float m_fRarityWordsQuorum;
		///коэффициент важности лингвитсического веса слов
		float m_fWordsLingvoWeightKoef;
		///максимальная длина интервала содержаещго слова-ответы
		unsigned int m_uiAnswerIntervalLimitLength;
		///максимальная длина результирующего интервала слов
		unsigned int m_uiCommonIntervalLimitLength;
	} m_SmartSearchSettings;

	///настройки извлечения результатов
	struct SResultsSettings{
		///величина расширения границ найденных интервалов
		unsigned int m_uiIntervalExpandRange;
		///размер области интервалы которой объединяются
		unsigned int m_uiIntervalJoinRange;
		///максимальное количество интервалов находящихся в одном тексте, 
		///по достижении этого порога поиск в текущем тексте завершается
		unsigned int m_uiIntervalInTextMaxCount;
	} m_ResultsSettings;
};

//--------------------------------------------------------------------//

///значение мета-поля
class CNdxMetaFieldValue
{
public:
	CNdxMetaFieldValue(SS::Interface::Core::DBMS::EFieldDataTypes ValueType) :
		m_ValueType(ValueType), m_pValue(NULL), m_uiValueByteSize(0)
	{ 
		Init(); 
	};
	CNdxMetaFieldValue(const CNdxMetaFieldValue &x) :
		m_ValueType(SS::Interface::Core::DBMS::EFieldDataTypes::TYPE_INT32), m_pValue(NULL), m_uiValueByteSize(0)
	{
		SetValue(x.GetValue(), x.GetValueType()); 
	};
	virtual ~CNdxMetaFieldValue(void) 
	{ 
		Reset(); 
	};

	///возвращает тип поля
	SS::Interface::Core::DBMS::EFieldDataTypes GetValueType(void) const {return m_ValueType;};

	///возвращает указатель на значение
	void* GetValue(void) const {return (m_ValueType==SS::Interface::Core::DBMS::TYPE_INT32 || m_ValueType==SS::Interface::Core::DBMS::TYPE_UINT32)?(void*)&m_uiValueByteSize:m_pValue;};

	///устанавливает значение поля, в соответствии с типом
	void SetValue(void* pValue, SS::Interface::Core::DBMS::EFieldDataTypes eValueType)
	{
		if(m_ValueType!=eValueType) {
			return;
		}
		
		if(m_ValueType==SS::Interface::Core::DBMS::TYPE_INT32 || m_ValueType==SS::Interface::Core::DBMS::TYPE_UINT32){
			m_uiValueByteSize=*(reinterpret_cast<unsigned int*>(pValue));
		}else if(m_ValueType==SS::Interface::Core::DBMS::TYPE_WSTRING){
			((std::wstring*)m_pValue)->assign(((std::wstring*)pValue)->c_str());
		}else{
			memcpy(reinterpret_cast<char*>(m_pValue), reinterpret_cast<char*>(pValue), m_uiValueByteSize);
		}
	};

	///оператор присвоения
	void operator=(const CNdxMetaFieldValue& RightValue)
	{
		SetValue(RightValue.GetValue(), RightValue.GetValueType()); 
	};

	///оператор меньше
	bool operator<(const CNdxMetaFieldValue& RightValue) const
	{
		if(m_ValueType!=RightValue.GetValueType()){
			return true;
		}

		switch(m_ValueType)
		{
		case SS::Interface::Core::DBMS::TYPE_INT32: case SS::Interface::Core::DBMS::TYPE_UINT32: 
			return m_uiValueByteSize<RightValue.m_uiValueByteSize?true:false;
			break;
		case SS::Interface::Core::DBMS::TYPE_WSTRING:
			return wcscmp((const wchar_t*)GetValue(), (const wchar_t*)RightValue.GetValue())<0?true:false;
			break;
		case SS::Interface::Core::DBMS::TYPE_BYTE_10:
			return memcmp(GetValue(), RightValue.GetValue(), 10)<0?true:false;
			break;
		default:
			return true;
			break;
		}

	};

	void ToString(std::wstring& szValue) const
	{
		wchar_t buf[100];

		switch(m_ValueType)
		{
		case SS::Interface::Core::DBMS::TYPE_INT32: case SS::Interface::Core::DBMS::TYPE_UINT32: 
			swprintf(buf, L"\n\tFieldValue: %u\n\tFieldType: TYPE_INT32", m_uiValueByteSize);
			szValue.assign(buf);
			break;
		case SS::Interface::Core::DBMS::TYPE_WSTRING:
			szValue.append(L"\n\tFieldValue: ");
			if(m_pValue) szValue.append(((std::wstring*)m_pValue)->c_str());
			szValue.append(L"\n\tFieldType: TYPE_WSTRING");
			break;
		case SS::Interface::Core::DBMS::TYPE_BYTE_10:
			swprintf(buf, L"\n\tFieldType: TYPE_BYTE_10");
			szValue.assign(buf);
			break;
		default:
			swprintf(buf, L"Unsupported type!!!");
			szValue.assign(buf);
			break;
		}

	};

protected:

	///инициализирует значение
	void Init(void)
	{
		switch(m_ValueType)
		{
		case SS::Interface::Core::DBMS::TYPE_INT32:  case SS::Interface::Core::DBMS::TYPE_UINT32:
			m_uiValueByteSize=0/*4*/;
			break;
		case SS::Interface::Core::DBMS::TYPE_WSTRING:
			m_uiValueByteSize=0/*500*/;
			m_pValue=new std::wstring();
			break;
		case SS::Interface::Core::DBMS::TYPE_BYTE_10:
			m_uiValueByteSize=10;
			break;
		default:
			m_uiValueByteSize=0;
			break;
		}

		if(m_uiValueByteSize)
			m_pValue=(void*)(new char[m_uiValueByteSize]);
	};

	///сбрасывает значение
	void Reset(void)
	{
		if(m_pValue)
		{
			if(m_ValueType==SS::Interface::Core::DBMS::TYPE_WSTRING){
				delete (std::wstring*)m_pValue;
			}else{
				delete[] (char*)m_pValue;
			}
			m_pValue=NULL;
		}
	};

	///тип значения
	const SS::Interface::Core::DBMS::EFieldDataTypes m_ValueType;
	///значение
	void* m_pValue;
	///бинарный размер значения поля !!!для 4 байтного значения хранит само значение, а m_pValue = NULL!!!
	unsigned int m_uiValueByteSize;

};

//--------------------------------------------------------------------//

///информация для фильтрации по мета-данным поиска
class CNdxMetaField
{
public:
	CNdxMetaField(wchar_t* pwszFieldName, 
	SS::Interface::Core::DBMS::EFieldDataTypes FieldType,
	SS::Interface::Core::DBMS::EFieldModificatorTypes FieldModificator)
	:m_FieldModificator(FieldModificator), m_bEnable(true), m_bIsSingleValue(false),
	m_SingleValue(FieldType), m_LowerValue(FieldType), m_UpperValue(FieldType)
	{
		SetFieldName(pwszFieldName);
	};

	CNdxMetaField(const CNdxMetaField& DataField)
	:m_FieldModificator(DataField.GetFieldModificator()), m_bEnable(DataField.IsEnable()), m_bIsSingleValue(false),
	m_SingleValue(DataField.GetFieldType()), m_LowerValue(DataField.GetFieldType()), m_UpperValue(DataField.GetFieldType())
	{
		SetFieldName(DataField.m_wszFieldName);
		SetValue(DataField.GetValue(), DataField.GetFieldType());
		SetLowerValue(DataField.GetLowerValue(), DataField.GetFieldType()); 
		SetUpperValue(DataField.GetUpperValue(), DataField.GetFieldType()); 
	};

	///возвращает название поля
	const wchar_t* GetFieldName(void){return m_wszFieldName;};
	///возвращает тип поля
	SS::Interface::Core::DBMS::EFieldDataTypes GetFieldType(void) const {return m_SingleValue.GetValueType();};
	///возвращает модификатор поля
	SS::Interface::Core::DBMS::EFieldModificatorTypes GetFieldModificator(void) const {return m_FieldModificator;};

	///устанавливает флаг активности поля
	void Enable(void){m_bEnable=true;};
	///устанавливает флаг активности поля
	void Disable(void){m_bEnable=false;};
	///возвращает флаг активности поля
	bool IsEnable(void) const {return m_bEnable;};
	///возвращает флаг активности поля
	bool IsSingleValue(void) const {return m_bIsSingleValue;};
	
	///возвращает указатель на значение
	void* GetValue(void) const {return m_SingleValue.GetValue();};
	///возвращает указатель на минимально возможное значение
	void* GetLowerValue(void) const {return m_LowerValue.GetValue();};
	///возвращает указатель на максимально возможное значение
	void* GetUpperValue(void) const {return m_UpperValue.GetValue();};

	///устанавливает значение поля, в соответствии с типом
	void SetValue(void* pValue, SS::Interface::Core::DBMS::EFieldDataTypes eFieldType){m_SingleValue.SetValue(pValue, eFieldType); m_bIsSingleValue=true;};
	///устанавливает значение поля, в соответствии с типом
	void SetLowerValue(void* pValue, SS::Interface::Core::DBMS::EFieldDataTypes eFieldType){m_LowerValue.SetValue(pValue, eFieldType); m_bIsSingleValue=false;};
	///устанавливает значение поля, в соответствии с типом
	void SetUpperValue(void* pValue, SS::Interface::Core::DBMS::EFieldDataTypes eFieldType){m_UpperValue.SetValue(pValue, eFieldType); m_bIsSingleValue=false;};

	///оператор присвоения
	void operator=(const CNdxMetaField& RightDF)
	{
		SetValue(RightDF.GetValue(), RightDF.GetFieldType()); 
		SetLowerValue(RightDF.GetLowerValue(), RightDF.GetFieldType()); 
		SetUpperValue(RightDF.GetUpperValue(), RightDF.GetFieldType()); 
	};

	///конвертирует строку в тип поля таблицы DBMS 
	static SS::Interface::Core::DBMS::EFieldDataTypes StringToFieldDataTypes(wchar_t* wszFieldType)
	{
		std::wstring sFieldType(wszFieldType);
		
		if(sFieldType==L"TYPE_INT32"){
			// 32-х_битовое_целое 
			return 	SS::Interface::Core::DBMS::TYPE_INT32;
		}
		
		if(sFieldType==L"TYPE_UINT32"){
			// 32-х_битовое_целое 
			return 	SS::Interface::Core::DBMS::TYPE_UINT32;
		}

		if(sFieldType==L"TYPE_BYTE_10"){
			// 10-и_байтовая_структура
			return 	SS::Interface::Core::DBMS::TYPE_BYTE_10;
		}
		if(sFieldType==L"TYPE_STRING"){
			// Строка std::string
			return 	SS::Interface::Core::DBMS::TYPE_STRING;
		}
		if(sFieldType==L"TYPE_VECTOR"){
			// Вектор std::vector<BYTE>
			return 	SS::Interface::Core::DBMS::TYPE_VECTOR;
		}
		if(sFieldType==L"TYPE_WSTRING"){
			// Строка std::wstring
			return 	SS::Interface::Core::DBMS::TYPE_WSTRING;
		}
		
		// по умолчанию
		return 	SS::Interface::Core::DBMS::TYPE_INT32;
	}

	///конвертирует строку в модификатор поля таблицы DBMS
	static SS::Interface::Core::DBMS::EFieldModificatorTypes StringToFieldModificatorTypes(wchar_t* wszFieldMod)
	{
		std::wstring sFieldMod(wszFieldMod);
		
		if(sFieldMod==L"MOD_COMMON"){
			// Обычные значения. (произвольные значения c возможностью поиска).
			return 	SS::Interface::Core::DBMS::MOD_COMMON;
		}

		if(sFieldMod==L"MOD_IS_UNIC"){
			// Значения в указанном поле будут уникальными. 
			//Методы поиска будут возвращать не более одной записи, 
			//а метод добавления вернет S_FALSE в случае, 
			//когда заданное значение поля в таблице уже есть. 
			//Экономия памяти за счет отсутствия ссылок "NEXT". (Пример использования: индексы и порядковые номера)    
			return 	SS::Interface::Core::DBMS::MOD_IS_UNIC;
		}

		if(sFieldMod==L"MOD_NO_FIND"){
			// Нет поиска по данному полю. Экономия памяти за счет отсутствия ссылок "NEXT". 
			//(Пример использования: словарные статьи и предложения, связанные со многими ключами)   
			return 	SS::Interface::Core::DBMS::MOD_NO_FIND;
		}

		if(sFieldMod==L"MOD_IS_DATA"){
			// Нет поиска по данному полю. Возможно изменение полей с типами данных INT32 и BYTE_10. 
			//Значения поступают в таблицу минуя HASH-функцию (повторяющиеся значения будут записаны многократно !!!). 
			//Экономия памяти за счет отсутствия ссылок "NEXT". (Пример использования: не повторяющиеся, не ключевые данные)   
			return 	SS::Interface::Core::DBMS::MOD_IS_DATA;
		}
		
		// по умолчанию
		return 	SS::Interface::Core::DBMS::MOD_COMMON;
	}

	virtual void ToString(std::wstring& szValue) const
	{
		wchar_t buf[100];
		swprintf(buf, L"FieldName: %s", m_wszFieldName);
		szValue.append(buf);
		
		std::wstring wsValue;

		szValue.append(L"\nValue:\n");
		m_SingleValue.ToString(wsValue);
		szValue.append(wsValue);

		szValue.append(L"\nLowerValue:\n");
		m_LowerValue.ToString(wsValue);
		szValue.append(wsValue);

		szValue.append(L"\nUpperValue:\n");
		m_UpperValue.ToString(wsValue);
		szValue.append(wsValue);
	};

protected:
	///устанавливает название поля, имя поля не больше 10 символов
	void SetFieldName(const wchar_t* wszFieldName)
	{
		if(wcslen(wszFieldName)>=10){
			return;
		}
		wcscpy(m_wszFieldName, wszFieldName);
	};

	///название поля, имя поля не больше 10 символов
	wchar_t m_wszFieldName[10];
	///модификатор поля
	const SS::Interface::Core::DBMS::EFieldModificatorTypes m_FieldModificator;
	///флаг активности поля
	bool m_bEnable;
	///флаг установки точного значения
	bool m_bIsSingleValue;
	///точное значение поля
	CNdxMetaFieldValue m_SingleValue;
	///для интервала нижнее значение поля
	CNdxMetaFieldValue m_LowerValue;
	///для интервала верхнее значение поля
	CNdxMetaFieldValue m_UpperValue;
};

///коллекция полей мета-данных
typedef std::vector<CNdxMetaField> TNdxMetaFieldCollection;

//--------------------------------------------------------------------//

///кусок дерева путей текстов
struct STextPathChunk
{
	STextPathChunk(
		///кусок пути
		const wchar_t* pMyPathChunk
		///идентификатор куска
		, unsigned int uiMyIndex)
	:m_pMyPathChunk(NULL), m_uiMyIndex(uiMyIndex), 
	m_uiMyOffset(0xffffffff)
	{
		if(pMyPathChunk){
			m_pMyPathChunk=new wchar_t[wcslen(pMyPathChunk)+1];
			wcscpy((wchar_t*)m_pMyPathChunk, pMyPathChunk);
		}
	}

	virtual ~STextPathChunk(void)
	{
		if(m_pMyPathChunk)
			delete[] m_pMyPathChunk;
	}
	
	///кусок пути
	const wchar_t* m_pMyPathChunk;
	///идентификатор куска
	/**для папки он равен 0xFFFFFFFF, для зипа 0xFFFFFFFE, а для документа это абс. номер текста*/
	const unsigned int m_uiMyIndex;
	///смещение куска в файле
	unsigned int m_uiMyOffset;
};

//--------------------------------------------------------------------//

}
}
}
}
