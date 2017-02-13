#pragma once

#include <list>
using namespace std;
#include "IBlackBoxItems.h"
#include "../ASSInterface/IDictionaryManager.h"
#include "../ASSInterface/IAMConverter.h"

typedef list<SS::Interface::Core::BlackBox::IUnit*> TUnitList;

namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			namespace BlackBox
			{
				//class CWordInfo
				//{
				//	public:
				//	list<wstring> m_listForms; // все словоформы слова	
				//};

				//class CTermInfo
				//{
				//public:
				//	list<CWordInfo*> m_listWords;
				//};

				//class CObjectDescr
				//{	
				//public:
				//	list<wstring> m_listSynonims;
				//	list<wstring> m_listKorefs;

				//	list<CTermInfo*> m_listSynonimTerms;
				//	list<CTermInfo*> m_listKorefTerms;		
				//};

				class IObjectDescr
				{
					public:
					///добавление юнита в конец
					virtual void AddSynonim(wstring synonim)=0;
					virtual void AddSynonim(wstring synonim, int weight)=0;
					virtual void AddKorefs(wstring synonim)=0;		
					virtual void SetType(wchar_t* type_name)=0;
					virtual wchar_t* GetType() = 0;
					virtual void FindObjectInSentence(ISentence* sent, TUnitList& findedUnits, bool isOfficial, IUnit* begunit, bool isKoref = false)=0;
					virtual void FindObjectInUnits(TUnitList& units, TUnitList& findedUnits, bool isOfficial, IUnit* begunit, bool isKoref = false)=0;
					//virtual void FindObjectInSentenceByKoref(ISentence* sent, TUnitList& findedUnits, bool isOfficial, IUnit* begunit)=0;
					virtual void MorphoTranslate(SS::Interface::Core::Dictionary::IMorphologyAnalyser* pMA,
						SS::Interface::Core::AMConverter::IAMConverterMorpho* m_pAMC, bool isQuote)=0;
				};
			}
		}
	}
}