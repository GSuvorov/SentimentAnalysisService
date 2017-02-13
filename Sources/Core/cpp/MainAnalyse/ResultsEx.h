#pragma once

//#define _SS_OLD_DICTIONARY

#include "../ASCInterface/ILinguisticProcessor.h"



#ifdef _SS_OLD_DICTIONARY

#include "../ASSInterface/IDictionary.h"

#else

#include "../ASSInterface/IDictionaryManager.h"

#endif


namespace SS
{
namespace LinguisticProcessor
{

class CResultsEx 
	: public SS::Core::CommonServices::CBaseCoreClass
	, public SS::Interface::Core::Common::CAnalyseParams
{
	SS::Interface::Core::Dictionary::IDictionary* m_pDictionary;
public:
	CResultsEx(void);
	~CResultsEx(void);

private:
	void Init();
public:
	/// устанавливает указатель на лоад мэнеджер
	virtual void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
	{
		SS_TRY
		{
			SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);
			Init();
		}
		SS_CATCH(L"")
	};
public:
	void ShowTable(std::list< std::list<SS::Core::Types::IndexationSearch::TNativeTextForIndexation*> > * p_l_l_NativeText);
	void ShowTable(SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult);
	void PrepareForView(
	/// контейнер с результатами поиска
	SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut);

	void PrepareForView(
	/// контейнер с результатами поиска
	SS::Interface::Core::CommonContainers::ITextBlock* pTextBlock);

	/// данная функция используется, если результат поиска содержит индексы слов для наполнения строковым описанием слов
	void PrepareForView(
		/// контейнер с результатами поиска
		SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult
		/// параметры подготовки к выводу (кол-во подготавливаемых результатов и т.п.)
		, SS::Core::Types::IndexationSearch::TPrepareForViewParams* pPrepareForViewParams = NULL);

	void FillIndexationIndexes(SS::Interface::Core::CommonContainers::ITextBlock * pTextBlock);

	unsigned int IsExistWord(const wchar_t* psWord);

};

}
}