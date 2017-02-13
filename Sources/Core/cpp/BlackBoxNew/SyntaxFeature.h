#pragma once

#include "..\ASSInterface\IBlackBoxSyntaxUnit.h"
using namespace SS::Interface::Core::BlackBox;


namespace SS
{
namespace Core
{
namespace BlackBox
{

class CSyntaxFeature : public ISyntaxFeature
{
public:
	CSyntaxFeature();
	virtual ~CSyntaxFeature();

public:
	///устанавливает синтаксическую характеристику
	void SetSyntaxCategory(unsigned int uiSyntaxCategory);
	///устанавливает синтаксическую окончательную характеристику 
	void SetSyntaxFeatureAposteriori(SS::Core::Features::CSyntaxFeatureAposteriori* pSyntaxFeatureAposteriori);
	///устанавливает синтаксическую характеристику
	void SetSyntaxFeatureApriori(SS::Core::Features::CSyntaxFeatureApriori* pSyntaxFeatureApriori);
	///возвращает синтаксическую характеристику
	unsigned int GetSyntaxCategory();
	///возвращает синтаксическую окончательную характеристику 
	SS::Core::Features::CSyntaxFeatureAposteriori* GetSyntaxFeatureAposteriori();
	///возвращает синтаксическую  характеристику 
	SS::Core::Features::CSyntaxFeatureApriori* GetSyntaxFeatureApriori();
	///устанавливает что фича вопрос
	void SetIsQuestion();
	///устанавливает что фича не вопрос
	void SetIsNotQuestion();
	///проверяет является ли фича вопросом
	unsigned int IsQuestion();
	///вернет размер, занимаемый итемом в байтах
	unsigned int GetMemorySize();

private:
	///признак вопроса
	bool m_bQuestion;
	///синтаксическая категория
	unsigned int m_uiSyntaxCategory;
	///окончательная синтаксическая информация
	SS::Core::Features::CSyntaxFeatureAposteriori m_SyntaxFeatureAposteriori;
	///синтаксическая  характеристика 
	SS::Core::Features::CSyntaxFeatureApriori m_SyntaxFeatureApriori;
};

}
}
}
