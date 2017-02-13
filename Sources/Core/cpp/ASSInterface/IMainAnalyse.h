#pragma once

#include ".\TMainAnalyseTypes.h"
#include "..\ASCInterface\TDictionaryIndex.h"
#include "..\ASCInterface\TCoommonTypes.h"

// {12AAEAA0-0679-4ab4-B505-7C0E60B7E58D}
static const GUID CLSID_AnalyseParams = 
{ 0x12aaeaa0, 0x679, 0x4ab4, { 0xb5, 0x5, 0x7c, 0xe, 0x60, 0xb7, 0xe5, 0x8d } };

// {9533F9F6-B3C1-485f-9765-8446000CDB54}
static const GUID IID_AnalyseParams = 
{ 0x9533f9f6, 0xb3c1, 0x485f, { 0x97, 0x65, 0x84, 0x46, 0x0, 0xc, 0xdb, 0x54 } };


namespace SS
{
namespace Interface
{
namespace Core
{
namespace MainAnalyse
{
///параметры процесса анализа текста
/**(тип анализа морфологический, лексический, семантический и т.д.)*/
class IAnalyseParams : public SS::Interface::IBase
{
	
	SS::Core::Types::IndexationSearch::AnalyseDepthParams			m_oAnalyseDepthParams;
	SS::Core::Types::EAnalyseMods									m_oAnalyseMods;
	SS::Core::Types::IndexationSearch::TSearchParams::ESearchArea	m_eSearchArea;
	SS::Core::Types::ECurrentSystemMode								m_eCurrentSystemMode;
	SS::Core::Types::ESyntaxAnalysisMode								m_eSyntaxAnalysisMode;
	SS::Core::Types::ERelevanceParams								m_eRelevanceParams;	
	SS::Core::Types::EAnalyseLanguage								m_eAnalyzeLang;

public:
	IAnalyseParams() 
		: m_eSyntaxAnalysisMode(SS::Core::Types::esamFull)
		, m_oAnalyseMods(SS::Core::Types::eamIndexation)
		, m_eSearchArea(SS::Core::Types::IndexationSearch::TSearchParams::saNormal)
		, m_eCurrentSystemMode(SS::Core::Types::ecsmLocal)
		, m_eRelevanceParams(SS::Core::Types::erpUndefine)
		, m_eAnalyzeLang(SS::Core::Types::ealUndefined)
		, m_bIsLiteMode(false)
		
	{}
	virtual ~IAnalyseParams(){}
	/* Параметры лингвистического анализа */

public:
	bool m_bIsLiteMode;

public:

	///устанавливает режим анализа
	virtual void SetAnalyseMod(SS::Core::Types::EAnalyseMods eAnalyseMod)
	{
		m_oAnalyseMods = eAnalyseMod;
	}
	///возвращает режим анализа
	virtual SS::Core::Types::EAnalyseMods GetAnalyseMod()
	{
		return m_oAnalyseMods;
	}

	/// устанавливает результат анализа
	virtual void SetAnalyseResultEx(SS::Core::Types::IndexationSearch::TSearchParams::ESearchArea eSearchArea)
	{
		m_eSearchArea = eSearchArea;
	}
	/// возвращает результат анализа
	virtual SS::Core::Types::IndexationSearch::TSearchParams::ESearchArea GetAnalyseResultEx()
	{
		return m_eSearchArea;
	}

	/// возвращает текущую систему, в которой выполняется анализ
	virtual SS::Core::Types::ECurrentSystemMode GetCurrentSystemMode()
	{
		return m_eCurrentSystemMode;
	}
	/// устанавливает текущую систему, в которой выполняется анализ
	virtual void SetCurrentSystemMode(SS::Core::Types::ECurrentSystemMode oCurrentSystemMode)
	{
		m_eCurrentSystemMode = oCurrentSystemMode;
	}

	/// устанавливает текущую систему, в которой выполняется анализ
	virtual void SetSyntaxAnalysisMode(SS::Core::Types::ESyntaxAnalysisMode eSyntaxAnalysisMode)
	{
		m_eSyntaxAnalysisMode = eSyntaxAnalysisMode;
	}
	virtual SS::Core::Types::ESyntaxAnalysisMode GetSyntaxAnalysisMode()
	{
		return m_eSyntaxAnalysisMode;
	}

	virtual SS::Core::Types::ERelevanceParams GetRelevanceParams()
	{
		return m_eRelevanceParams;
	}

	virtual void SetRelevanceParams(SS::Core::Types::ERelevanceParams eRelevanceParams)
	{
		m_eRelevanceParams = eRelevanceParams;
	}

	virtual SS::Core::Types::EAnalyseLanguage GetAnalizeLang()
	{
		return m_eAnalyzeLang;
	}

	virtual void SetAnalizeLang(SS::Core::Types::EAnalyseLanguage val)
	{
		m_eAnalyzeLang = val;
	}
	/*	результаты лингвистического анализа*/
public:
	/// возвращает данные о глубине анализа
	virtual SS::Core::Types::IndexationSearch::AnalyseDepthParams* GetAnalyseDepthParams()
	{
		return &m_oAnalyseDepthParams;
	}

};
}
}
}
}