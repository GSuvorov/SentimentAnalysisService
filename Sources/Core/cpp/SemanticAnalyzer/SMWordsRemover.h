#pragma once
#include "SMSentenceExpert.h"
namespace SS
{
namespace Semantic
{
namespace PostAnalysisExperts
{
namespace PostSyntaxAnalysis
{
/// удаляет юниты из группы
class CSMWordsRemover :	public CSMSentenceExpert
{
public:
	CSMWordsRemover(void);
	~CSMWordsRemover(void);
	bool AnalizeSentence();
private:
	bool IsContainSyntaxCategories(IUnit * pUnit, Types::SyntaxCategories oSyntaxCategories);
	bool Remove();
	void RemoveWord(IUnit * pUnit, const wchar_t * wzRemoveWord, Types::SyntaxCategories oSyntaxCategories);
	void RemoveWord(const wchar_t * wzRemoveWord, Types::SyntaxCategories oSyntaxCategories);
	bool IsPartOfSpeech(IUnit * pUnit);
	bool IsLegalLexem(IUnit * pUnit);
	bool IsDeleteCurrentUnit(bool bDel);
	//устанавливает необязательность юниту
	void SetObligatory(IUnit * pUnit);
	//определяет есть ли семантика имени у юнита
	bool IsNameSemantic(IUnit * pUnit);
	void CheckAndRemoveAdjectiveForNames(const IUnit * pUnit);
	void CheckNameSemantic(IUnit * pUnit);
	bool IsNotMostInDegree(SS::Core::Features::CMorphoFeature & oMorphoFeature);
};
}
}
}
}