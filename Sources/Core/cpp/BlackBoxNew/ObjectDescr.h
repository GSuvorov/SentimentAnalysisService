#pragma once

#include "../ASSInterface/IDictionaryManager.h"
#include "../ASSInterface/IObjectDescr.h"
#include "../ASSInterface/IAmconverter.h"
using namespace SS::Interface::Core::BlackBox;
using namespace SS::Interface::Core::Dictionary;
using namespace SS::Interface::Core::AMConverter;

#include <map>

typedef std::list<SS::Dictionary::Types::TWordInfo> TWIList;

namespace SS
{
namespace Core
{
namespace BlackBox
{
	class CWordInfo
	{
		public:
			CWordInfo();			
			map<wstring, int> m_listForms; // все словоформы слова	
			list<SS::Dictionary::Types::TDictionaryIndex> m_dictIndexes;
			bool isWordInForms1(wstring word);

			bool isWordInForms(IUnit *unit);
			wstring word;
						
	};

	class CTermInfo
	{
	public:
		CTermInfo();
		//void AddWord(CWordInfo
	public:
		list<CWordInfo*> m_listWords;
		int weight;
	};

	class CObjectDescr : public IObjectDescr
	{	
	private:
		list<wstring> m_listSynonims;
		list<int> m_listWeights;
		list<wstring> m_listKorefs;

		list<CTermInfo*> m_listSynonimTerms;
		list<CTermInfo*> m_listKorefTerms;

		IMorphologyAnalyser* m_pMorphoAnalyzer;
		IAMConverterMorpho* m_pAMConverter;

		void AddChildUnits(IUnit* unit, list<IUnit*>& findedUnits);
		bool isOfficial(IUnit* unit);

		wstring typeName;
		static int findedNumber;

	public:
		CObjectDescr();
		~CObjectDescr();
		virtual void AddSynonim(wstring synonim);
		virtual void AddSynonim(wstring synonim, int weight);
		virtual void AddKorefs(wstring synonim);		
		virtual void SetType(wchar_t* type_name);
		virtual wchar_t* GetType();
		virtual void FindObjectInSentence(ISentence* sent, TUnitList& findedUnits, bool isOfficial, IUnit* begunit,  bool isKoref = false);
		virtual void MorphoTranslate(IMorphologyAnalyser* pMA, IAMConverterMorpho* m_pAMC, bool isQuote);
		void MorphoTranslate1(IMorphologyAnalyser* pMA, IAMConverterMorpho* m_pAMC, bool isQuote);
		void FindObjectInUnits(TUnitList& sentenceUnits, TUnitList& findedUnits, bool bOfficial, IUnit* begunit, bool isKoref = false);


	//private:
		
		
	};

}
}
}