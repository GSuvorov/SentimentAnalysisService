#pragma once
#include ".\basis_containers.h"
#include ".\consoleclient.h"
#include ".\const.h"
#include "..\ASCInterface\ICommonContainers.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Extracting
{

///буфер буферов указателей указателей на значения по аргументам
typedef vector<SS::Core::NdxSE::NdxProcessor::Containers::TValuePtrsBuffer*> TArgumentsValuesBuffer;

//--------------------------------------------------------------------//

///аргумент - хранит значение и свой вес
class CArgument : public CConsoleClient
{
public:
	CArgument(unsigned int uiArgumentID = SS::Core::NdxSE::NdxProcessor::EMPTY_VALUE, float fKoeff = 1.0, SS::Interface::Core::CommonContainers::SQueryIndexAttributes ArgumentAttributes = SS::Interface::Core::CommonContainers::SQueryIndexAttributes());
	virtual ~CArgument(void);
	
	///устанавливает значение аргумента
	virtual void SetValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue){m_pCurrentValue=pValue;};
	///возвращает значение аргумента
	virtual SS::Core::NdxSE::NdxProcessor::Containers::CValue* GetValue(void){return m_pCurrentValue;};
	///возвращает идентификатор аргумента
	unsigned int GetArgumentID(void){return m_uiArgumentID;};
	///возвращает коэффициент при аргументе
	float GetKoeff(void){return m_fKoeff;}
	///устанавливает коэффициент при аргументе
	void SetKoeff(float fKoeff){m_fKoeff=fKoeff;}
	///возвращает аттрибуты
	SS::Interface::Core::CommonContainers::SQueryIndexAttributes GetAttributes(void){return m_Attributes;};
	///устанавливает аттрибуты
	void SetAttributes(SS::Interface::Core::CommonContainers::SQueryIndexAttributes Value){m_Attributes=Value;};

	///сериализация в строку
	void ToString(wstring& szValue);

protected:
	///идентификатор аргумента
	const unsigned int m_uiArgumentID;
	///коэффициент при аргументе
	float m_fKoeff;
	///текущее значение аргумента
	SS::Core::NdxSE::NdxProcessor::Containers::CValue* m_pCurrentValue;
	///аттрибуты аргмента
	SS::Interface::Core::CommonContainers::SQueryIndexAttributes m_Attributes;
};

//--------------------------------------------------------------------//

///аргумент - хранит ортовое значение и веса его орт
class CArgumentEx : public CArgument
{
public:
	typedef vector<float> TValueOrtsWeightsCollection;

	CArgumentEx(unsigned int uiArgumentID = SS::Core::NdxSE::NdxProcessor::EMPTY_VALUE, float fKoeff = 1.0, SS::Interface::Core::CommonContainers::SQueryIndexAttributes ArgumentAttributes = SS::Interface::Core::CommonContainers::SQueryIndexAttributes());
	virtual ~CArgumentEx(void);

	///устанавливает значение аргумента
	void SetValue(SS::Core::NdxSE::NdxProcessor::Containers::CCompositeValue* pValue);
	///возвращает значение аргумента
	SS::Core::NdxSE::NdxProcessor::Containers::CCompositeValue* GetValue(void){return (SS::Core::NdxSE::NdxProcessor::Containers::CCompositeValue*)m_pCurrentValue;};
	///возвращает коллекцию весов ортов составлющих значение аргумента
	TValueOrtsWeightsCollection* GetValueOrtsWeights(void);

	///сериализация в строку
	void ToString(wstring& szValue);

protected:
	///устанавливает значение аргумента
	void SetValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue){m_pCurrentValue=pValue;};

	///коллекция весов ортов значения аргумента, доступ к весу по номеру орта
	TValueOrtsWeightsCollection* m_pValueOrtsWeights;	
};

//--------------------------------------------------------------------//

class CComplexArgument;

///пара аргумент-значение
class CArgValuesPair
{
public:
	CArgValuesPair(CComplexArgument* pArgument, SS::Core::NdxSE::NdxProcessor::Containers::CValuesContainer* pValuesContainer)
		:m_pArgument(pArgument), m_pValuesContainer(pValuesContainer), m_pLastUsedValue(NULL){};
	virtual ~CArgValuesPair(void)
	{
		//удаляем контайнер значений 
		//так как его больше некому удалять,
		//он же удалит все порожденные им значения
		if(m_pValuesContainer){
			delete m_pValuesContainer;
			m_pValuesContainer=NULL;
		}
		m_pLastUsedValue=NULL;
	};
	
	///возвращает значение
	SS::Core::NdxSE::NdxProcessor::Containers::CValuesContainer* GetValuesContainer(void){return m_pValuesContainer;};
	///возвращает аргумент
	CComplexArgument* GetArgument(void){return m_pArgument;};
	///устанавливает последнее используемое значение контейнера
	void SetLastUsedValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue){m_pLastUsedValue=pValue;};
	///устанавливает последнее используемое значение контейнера
	SS::Core::NdxSE::NdxProcessor::Containers::CValue* GetLastUsedValue(void){return m_pLastUsedValue;};
	///выставляет значения коллекции результата по последнему использованому значению
	void SetFreshValues(SS::Core::NdxSE::NdxProcessor::Containers::TValuePtrsBuffer* pValuePtrsBuffer)
	{
		if(ISNULL(m_pValuesContainer)) return; 
		if(ISNULL(pValuePtrsBuffer)) return; 

		//START_TICK(L"SetFreshValues");
		m_pValuesContainer->SetFreshValues(m_pLastUsedValue, pValuePtrsBuffer);
		//STOP_TICK(L"SetFreshValues");
	};

protected:
	///аргумент
	CComplexArgument* m_pArgument;
	///контайнер значений
	SS::Core::NdxSE::NdxProcessor::Containers::CValuesContainer* m_pValuesContainer;
	///последнее используемое значение контейнера
	SS::Core::NdxSE::NdxProcessor::Containers::CValue* m_pLastUsedValue;
};

typedef list<CArgValuesPair*> TArgValCollection;

//--------------------------------------------------------------------//

}
}
}
}
}
