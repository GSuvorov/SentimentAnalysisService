#pragma once

#include "detail.h"
#include "OrderingRussianFeature.h"


namespace SS
{
namespace Dictionary
{
namespace Virtual
{

class COrderingRussian : public Detail::IBasicMorphologyAnalyser 	  
{
public:
	COrderingRussian();
	explicit COrderingRussian(IAMConverterMorpho* pAMConverter);
	virtual ~COrderingRussian();

public:
	/// проверка наличия слова в словаре
	virtual bool IsExistWord(/*in*/const wchar_t* wcWord);

	/// поиск полной морфологической информации по слову
	/// wcWord - слово приведённое к нижнему регистру
	/// wcWordOriginalCase - Слово с оригинальным регистром
	/// IsFirstLetterUpperCase - Находится ли первая буква слова в верхнем регистре
	virtual bool GetWordInfo/*ByWord*/(
		/*in*/const wchar_t* wcWord, 
		/*in*/const wchar_t* wcWordOriginalCase,
		/*in*/bool IsFirstLetterUpperCase,
		/*out*/std::list<SS::Dictionary::Types::TWordInfo>* p_l_WordInfo 
	);

	/// поиск полной морфологической информации по индексу слова
	virtual bool GetWordInfo/*ByWordIndex*/(
		/*in*/SS::Dictionary::Types::SWordIndex* pWordIndex, 
		/*out*/SS::Dictionary::Types::SWordInfo* pWordInfo
	);

	/// поиск слова по его индексу 
	virtual bool GetWord(
		/*in*/SS::Dictionary::Types::SWordIndex* pWordIndex,
		/*out*/wchar_t* wcWord
	);

	/// поиск основы слова слова (части слова без оканчания) по его индексу 
	virtual bool GetWordSource(
		/*in*/const SS::Dictionary::Types::TDictionaryIndex &DictIndex,
		/*out*/wchar_t* wcWordSource
	);

	/// поиск словоформ слова по индексу основы
	virtual bool GetWordFormsByIDSource(
		/*in*/SS::Dictionary::Types::TDictionaryIndex* pDictionaryIndex,
		/*out*/std::list<SS::Dictionary::Types::TWordInfo>* p_l_WordInfo
	);

private:
	bool GetWordByIndex(const TDictionaryIndex &Index, TIDForm idForm, wchar_t* wcWord);
	void FillWordInfo(SWordInfo& oWordInfo, LPCWSTR szWord, UINT uiValue, TIDForm idForm, 
		int iMorphoInfo, int iSpecialMorpho);
	TIDForm GetIDForm(LPCWSTR szWord);
	LPCWSTR GetWordSuffix(LPCWSTR szWord);

	bool GetWordValues(const wchar_t* wcWord, UINT& uiValue, TIDForm& idForm);

private:
	COrderingRussianFeature m_oHelper;
};

}
}
}
