#pragma once

#include "../ASSInterface/ILexicaLib.h"
#include "../ASSInterface/ISemanticAnalyze.h"
#include "../ASSInterface/ISyntaxAnalyzer.h"
#include "../ASSInterface/IRev.h"
#include "../ASSInterface/IDictionaryManager.h"
#include "../ASSInterface/ICoSeDi.h"
#include "../ASSInterface/IQueryDeformer.h"

namespace SS
{
namespace LinguisticProcessor
{

class CLinguisticWorkingInterfaces
	: public SS::Core::CommonServices::CBaseCoreClass
    , public SS::Interface::Core::Common::CAnalyseParams
{
protected: // AnalyseInterface
	SS::Interface::Core::CoSeDi::ICoSeDiManager*					m_pCoSeDiManager;
	/// модуль морфологического анализа
	SS::Interface::Core::Dictionary::IDictionary*				m_pDictionary;
	/// лексический анализ
	SS::Interface::Core::LexicalAnalyze::ILexica*				m_pLexica;
	/// синтаксический анализ
	SS::Interface::Core::ISyntaxAnalyzer*						m_pSyntaxAnalyzer;
	/// семантический анализ
	SS::Interface::Core::ISemanticAnalyze*						m_pSemanticAnalyze;
	/// модуль оценки релевантности
	SS::Interface::Core::Rev::IRelevanceEvaluation*				m_pRelevanceEvaluation;
	/// модуль преобразования запросов (для последовательности запросов типа: Иванов Иван Иванович. Где он родился?)
	SS::Interface::QueryDeformer::IQueryDeformer*				m_pQueryDeformer;
protected: // TextStorages
	// IText
	SS::Interface::Core::BlackBox::IBlackBoxTextStorage*			m_pBlackBoxTextStorage;
	SS::Interface::Core::BlackBox::IText*						m_pText;
	SS::Interface::Core::BlackBox::IText*						m_pTextFeatureText;
	// IQuery
	SS::Interface::Core::BlackBox::Query::IQuery*				m_pQuery;
	SS::Interface::Core::BlackBox::IBlackBoxQueryStorage*		m_pBlackBoxQueryStorage;
	SS::Interface::Core::Rev::ILinguisticRelevance*				m_pLinguisticRelevance;
protected: // params
	SS::Core::Types::IndexationSearch::TLinguisticProcessorMode m_oMode;		
public:
	CLinguisticWorkingInterfaces(void);
	~CLinguisticWorkingInterfaces(void);
public:
	void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
public:
	void SetMode(SS::Core::Types::IndexationSearch::TLinguisticProcessorMode* pMode)
	{
		m_oMode.GetAnalyseDepthParams()->SetIsMorpho	(pMode->GetAnalyseDepthParams()->IsMorpho());
		m_oMode.GetAnalyseDepthParams()->SetIsSemantic	(pMode->GetAnalyseDepthParams()->IsSemantic());
		m_oMode.GetAnalyseDepthParams()->SetIsSyntax	(pMode->GetAnalyseDepthParams()->IsSyntax());
		m_oMode.GetAnalyseDepthParams()->SetIsSynonims	(pMode->GetAnalyseDepthParams()->IsSynonims());
	}
};

}
}
