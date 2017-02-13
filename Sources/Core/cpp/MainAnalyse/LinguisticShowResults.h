#pragma once

#include "../ASCInterface/ILinguisticProcessor.h"

#include "../ASSInterface/ILexicaLib.h"
#include "./ResultsEx.h"

#include "..\ASSInterface\IDictionaryManager.h"
using namespace SS::Interface::Core::Dictionary;

#include "..\ASSInterface\TDictionaryManager.h"
using namespace SS::Dictionary::Types;

#include "..\ASSInterface\IMainAnalyse.h"
using namespace SS::Interface::Core::MainAnalyse;

#include "..\ASSInterface\TMainAnalyseTypes.h"
using namespace SS::Core::Types;


namespace SS
{
namespace LinguisticProcessor
{
	class CLinguisticShowResults 
		: public SS::Interface::Core::ILinguisticShowResults
		, public SS::Core::CommonServices::CBaseCoreClass
		, public SS::Interface::Core::Common::CAnalyseParams
	{
		SS::Interface::Core::ILexicaPrimary*						m_pLexicaPrimary;
		IMorphologyAnalyser*												m_pMorphoAnalyzer;
		//IAnalyseParams*														m_pAnalyseParams;

		CResultsEx m_ResultsEx;

		SS::Core::Types::IndexationSearch::TLinguisticProcessorMode m_oMode;

	public:
		CLinguisticShowResults(void);
		~CLinguisticShowResults(void);
	public:
		void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
	public:
		SS::Interface::Core::ILexicaPrimary* GetLexicaPrimary();

		/// проверяет наличие слова в словаре
		/** 
			\param psWord - указатель на слово
			\return unsigned int вернет 0, если слова нет в базе и 1 если есть
		*/
		unsigned int  IsExistWord(const wchar_t* psWord);

		/// данная функция используется, если результат поиска содержит индексы слов для наполнения строковым описанием слов
		void PrepareForView(
			/// контейнер с результатами поиска
			SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult
			/// параметры подготовки к выводу (кол-во подготавливаемых результатов и т.п.)
			, SS::Core::Types::IndexationSearch::TPrepareForViewParams* pPrepareForViewParams
			);

		virtual void GetSourceIndices(const wchar_t* wszWord, std::vector<unsigned int>* pvIndices);

	public:
		void SetMode(SS::Core::Types::IndexationSearch::TLinguisticProcessorMode* pMode)
		{
			m_oMode = *pMode;
		}

	private:
		bool InitDictionary();
	};
}
}