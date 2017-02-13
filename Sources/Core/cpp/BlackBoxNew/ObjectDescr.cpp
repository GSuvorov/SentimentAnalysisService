#include "StdAfx.h"
#include "ObjectDescr.h"

#include <locale>
#include <fstream>
//#include <string>

using namespace SS::Interface::Core::Dictionary;
using namespace SS::Interface::Core::MainAnalyse;




namespace SS
{
namespace Core
{
namespace BlackBox
{
	int CObjectDescr::findedNumber=0;
	
	typedef std::list<CWordInfo*> TListWordInfo;
	typedef std::list<CTermInfo*> TListTermInfo;

	CObjectDescr::CObjectDescr()
	{		
	}

	CObjectDescr::~CObjectDescr()
	{
		for(list<CTermInfo*>::iterator it = m_listSynonimTerms.begin(); it!=m_listSynonimTerms.end(); it++)
		{
			CTermInfo* ti = *it;
			delete ti;
		}
	}

	void CObjectDescr::MorphoTranslate(IMorphologyAnalyser* pMA, IAMConverterMorpho* m_pAMC, bool isQuote)
	{
		string old_loc = setlocale(LC_CTYPE,"Russian_Russia.1251");
		std::list<SS::Dictionary::Types::TWordInfo> WIList;
		std::list<SS::Dictionary::Types::TWordInfo> lWordInfoDetect;
		m_pMorphoAnalyzer = pMA;
		m_pAMConverter = m_pAMC;
		std::list<int>::iterator itWeigth = this->m_listWeights.begin();
		for ( std::list<wstring>::iterator it = this->m_listSynonims.begin(); it != this->m_listSynonims.end(); ++it, ++itWeigth )
		{		
			wstring synonim = *it;
			wstring word;
			word.clear();
			WIList.clear();
			CTermInfo* termInfo = new CTermInfo();
			size_t synonim_len = synonim.size();
			for(unsigned int i = 0; i < synonim_len; ++i)
			{
				if (synonim[i]==' ' || synonim[i]=='\t'|| i==synonim_len-1)
				{					
					if (i==synonim_len-1 && synonim[i]!=' ' && synonim[i]!='\t')
						if ((synonim[i]!='"' && synonim[i]!='«' &&
					      synonim[i]!='»' &&  synonim[i]!='\'') || isQuote)
						    word+=synonim[i];
					if (word.size()==0) continue;					
					// получили слово. теперь надо получить все его морфоформы
					m_pMorphoAnalyzer->GetWordInfo(word.c_str(), &WIList);					
					CWordInfo *wi = new CWordInfo();
					for(TWIList::iterator it1 = WIList.begin(); it1 != WIList.end(); ++it1)
					{
						SS::Dictionary::Types::SWordInfo* pwi = &(it1->front());
						if (pwi->m_WordIndex.m_DictIndex.GetDictionaryNumber()!=14)
						 wi->m_dictIndexes.push_back(pwi->m_WordIndex.m_DictIndex);	
						else{
							m_pMorphoAnalyzer->GetWordFormsByIDSource(&(pwi->m_WordIndex.m_DictIndex),
							&lWordInfoDetect);
							for(TWIList::iterator it2 = lWordInfoDetect.begin(); it2 != lWordInfoDetect.end(); ++it2)
							{							
								wstring str = it2->front().m_wWord;
								wstring strLo = _wcslwr((wchar_t*)str.c_str());	
								if (wi->m_listForms.find(strLo)==wi->m_listForms.end()) // еще нет
								wi->m_listForms.insert(map<wstring, int>::value_type(strLo, 0));
							}			
						}
					}
					termInfo->m_listWords.push_back(wi);
					wi->word = word;
					word.clear();

				}else if ((synonim[i]!='"' && synonim[i]!='«' &&
					      synonim[i]!='»' &&  synonim[i]!='\'') || isQuote)
							word+=synonim[i];
			}
			termInfo->weight = *itWeigth;
			m_listSynonimTerms.push_back(termInfo);
			
		}

		for ( std::list<wstring>::iterator it = this->m_listKorefs.begin(); it != this->m_listKorefs.end(); ++it, ++itWeigth )
		{		
			wstring synonim = *it;
			wstring word;
			word.clear();
			WIList.clear();
			CTermInfo* termInfo = new CTermInfo();
			size_t synonim_len = synonim.size();
			for(unsigned int i=0;i<synonim_len;i++)
			{
				if (synonim[i] == ' ' || synonim[i] == '\t'|| i == synonim_len - 1)
				{					
					if (i == synonim_len - 1 && synonim[i] != ' ' && synonim[i] != '\t')
						if ((synonim[i] != '"' && synonim[i] != '«' &&
					         synonim[i] != '»' &&  synonim[i] != '\'') || isQuote)
						    word += synonim[i];
					if (word.size() == 0) continue;
					// получили слово. теперь надо получить все его морфоформы
					m_pMorphoAnalyzer->GetWordInfo(word.c_str(), &WIList);					
					CWordInfo *wi = new CWordInfo();
					for(TWIList::iterator it1 = WIList.begin(); it1 != WIList.end(); ++it1)
					{
						SS::Dictionary::Types::SWordInfo* pwi = &(it1->front());
						if (pwi->m_WordIndex.m_DictIndex.GetDictionaryNumber()!=14)
						 wi->m_dictIndexes.push_back(pwi->m_WordIndex.m_DictIndex);	
						else{
							m_pMorphoAnalyzer->GetWordFormsByIDSource(&(pwi->m_WordIndex.m_DictIndex),
							&lWordInfoDetect);
							for(TWIList::iterator it2 = lWordInfoDetect.begin(); it2 != lWordInfoDetect.end(); ++it2)
							{							
								wstring str = it2->front().m_wWord;
								wstring strLo = _wcslwr((wchar_t*)str.c_str());	
								if (wi->m_listForms.find(strLo)==wi->m_listForms.end()) // еще нет
								wi->m_listForms.insert(map<wstring, int>::value_type(strLo, 0));
							}			
						}
					}
					termInfo->m_listWords.push_back(wi);
					wi->word = word;
					word.clear();

				}else if ((synonim[i]!='"' && synonim[i]!='«' &&
					      synonim[i]!='»' &&  synonim[i]!='\'') || isQuote)
							word+=synonim[i];
			}
			termInfo->weight = *itWeigth;
			this->m_listKorefTerms.push_back(termInfo);
			m_listSynonimTerms.push_back(termInfo);
			
		}

		setlocale(LC_CTYPE,old_loc.c_str()); 
	}

	void CObjectDescr::MorphoTranslate1(IMorphologyAnalyser* pMA, IAMConverterMorpho* m_pAMC, bool isQuote)
	{
		string old_loc = setlocale(LC_CTYPE,"Russian_Russia.1251");
		std::list<SS::Dictionary::Types::TWordInfo> WIList;
		std::list<SS::Dictionary::Types::TWordInfo> lWordInfoDetect;
		m_pMorphoAnalyzer = pMA;
		m_pAMConverter = m_pAMC;
		std::list<int>::iterator itWeigth = this->m_listWeights.begin();
		for(std::list<wstring>::iterator it = this->m_listSynonims.begin(); it != this->m_listSynonims.end(); ++it, ++itWeigth)
		{		
			wstring synonim = *it;
			wstring word;
			word.clear();
			WIList.clear();
			CTermInfo* termInfo = new CTermInfo();
			size_t synonim_len = synonim.size();
			for(unsigned int i=0;i<synonim_len;i++)
			{
				if (synonim[i]==' ' || i==synonim_len-1)
				{					
					if (i==synonim_len-1)
						if ((synonim[i]!='"' && synonim[i]!='«' &&
					      synonim[i]!='»' &&  synonim[i]!='\'') || isQuote)
						    word+=synonim[i];
					if (word.size()==0) continue;
					// получили слово. теперь надо получить все его морфоформы
					m_pMorphoAnalyzer->GetWordInfo(word.c_str(), &WIList);					
					CWordInfo *wi = new CWordInfo();
					for(TWIList::iterator it1 = WIList.begin(); it1 != WIList.end(); ++it1)
					{
						SS::Dictionary::Types::SWordInfo* pwi = &(it1->front());
						//pwi->m_WordIndex
						//wchar_t wzWord[100];
						//pwi->
						m_pMorphoAnalyzer->GetWordFormsByIDSource(&(pwi->m_WordIndex.m_DictIndex),
							&lWordInfoDetect);
						for(TWIList::iterator it2 = lWordInfoDetect.begin(); it2 != lWordInfoDetect.end(); ++it2)
						{							
							wstring str = it2->front().m_wWord;
							wstring strLo = _wcslwr((wchar_t*)str.c_str());	
							if (wi->m_listForms.find(strLo)==wi->m_listForms.end()) // еще нет
								wi->m_listForms.insert(map<wstring, int>::value_type(strLo, 0));
						}						
					}
					termInfo->m_listWords.push_back(wi);
					word.clear();

				}else if ((synonim[i]!='"' && synonim[i]!='«' &&
					      synonim[i]!='»' &&  synonim[i]!='\'') || isQuote)
							word+=synonim[i];
			}
			termInfo->weight = *itWeigth;
			m_listSynonimTerms.push_back(termInfo);
			
		}

		setlocale(LC_CTYPE,old_loc.c_str()); 
	}

	void CObjectDescr::AddSynonim(wstring synonim)
	{			
		m_listSynonims.push_back(synonim);
		m_listWeights.push_back(0);		
	}

	void CObjectDescr::AddSynonim(wstring synonim, int weight)
	{		
		m_listSynonims.push_back(synonim);
		m_listWeights.push_back(weight);		
	}

	void CObjectDescr::AddKorefs(wstring synonim)
	{
		m_listSynonims.push_back(synonim);	
		m_listWeights.push_back(0);
	}
	
	bool CObjectDescr::isOfficial(IUnit* pUnit)
	{
		SS::Interface::Core::BlackBox::IIndex* pIndex;
		SS::Interface::Core::BlackBox::IDictionaryIndex* pDictionaryIndex;

		for(pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
		{
			pDictionaryIndex = pIndex->GetDictionaryIndex();

			if(pDictionaryIndex)
			{
				SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex = pDictionaryIndex->GetFirst();
				for(IFeature * pFeature = pIndex->GetFirstFeature();pFeature; pFeature = pIndex->GetNextFeature())
				{
					SS::Core::Features::CMorphoFeature oMorphoFeature;
					m_pAMConverter->EnCode(oDictionaryIndex.GetDictionaryNumber(), 
						pFeature->GetMorphoInfo(),
						pFeature->GetPartOfSpeech(), &oMorphoFeature);					
					if (oMorphoFeature.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postOfficial) &&
						oMorphoFeature.m_OfficialType.Equal(SS::Core::Features::Values::OfficialType.otSymbol))
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	/*void CObjectDescr::GetAllIndexes(IUnit *pUnit, list<SS::Dictionary::Types::TDictionaryIndex>& dictIndexes)
	{
		
	}*/
	void CObjectDescr::FindObjectInUnits(TUnitList& sentenceUnits, TUnitList& findedUnits, bool bOfficial, IUnit* begunit, bool isKorefs)
	{
		//list<IUnit*> sentenceUnits;
		
		list<IUnit*> localFindedUnits;	
		//sent->GetLinearUnits(sentenceUnits);
		localFindedUnits.clear();	

		list<IUnit*>::iterator itSentStart = sentenceUnits.begin();
		if (begunit)
		{
            while(*itSentStart != begunit)
            {
                ++itSentStart;
                if (itSentStart == sentenceUnits.end())
                    break;
            }
		}

		int count = 0;
		//int num_of_finded = 0;
		list<CTermInfo*>* p_listTerms;
		if (!isKorefs)
            p_listTerms = &m_listSynonimTerms;
        else
            p_listTerms = &m_listKorefTerms;

		for(TListTermInfo::iterator it = (*p_listTerms).begin(); it != (*p_listTerms).end(); ++it)
		{
			count++;
			CTermInfo* ti = *it;		
			list<IUnit*>::iterator itSent = itSentStart;
			localFindedUnits.clear();						
			while(itSent != sentenceUnits.end())
			{
				IUnit* unit = *itSent;
				wstring str_word = unit->GetWord();
				SS::Dictionary::Types::TDictionaryIndex dictIndex;
				int len = str_word.size();
				int offs = unit->GetFirstCharPosition();
				//if (len==0 || offs<0) 
				if (len==0 || offs<0 || (isOfficial(unit) && !bOfficial))
                // знаки препинания и кавычки выкидываем 
                {
                    ++itSent;
                    continue;
                }
							
				unit->GetFirstIndex()->GetDictionaryIndex()->GetFirst(&dictIndex);
                bool isRel = true;
				if (ti->m_listWords.size() == 0)
                    isRel = false;
				for(TListWordInfo::iterator itWord = (ti->m_listWords).begin();
                    itWord != (ti->m_listWords).end();
                    ++itWord)
				{				
					CWordInfo* wi = *itWord;
				// получили слово. Теперь сравним его последовательно со ловами в предложении												
					if (wi->isWordInForms(unit))
					{
						localFindedUnits.push_back(unit);						
						++itSent;
						// пропускаем знаки препинания
						while(itSent != sentenceUnits.end())
						{
							if (!isOfficial(*itSent) || bOfficial)
                                break;
							++itSent;
						}						
						if (itSent != sentenceUnits.end())
						{
							unit = *itSent;
							str_word = unit->GetWord();
							len = str_word.size();
							offs = unit->GetFirstCharPosition();
                            if ((len > 0) && (offs >= 0))
                                unit->GetFirstIndex()->GetDictionaryIndex()->GetFirst(&dictIndex);
						}
                        else
                            str_word.clear();
					}
					else
                    {
                        isRel = false;
                        break;
                    }
				}	
				
				if (isRel)
				{
					// нашли синоним
					for(list<IUnit*>::iterator itUnit1 = localFindedUnits.begin(); itUnit1 != localFindedUnits.end(); ++itUnit1)			
					{
						IUnit* unit1 = *itUnit1;
						wstring str = unit1->GetWord();
						if (unit1->GetToneTermCount()<localFindedUnits.size() ||
							(unit1->GetTone()==0 && unit1->GetInversion()==1))
						{
							findedUnits.push_back(unit1);
							unit1->SetTone(ti->weight);
							unit1->SetTermNumber(findedNumber);
							unit1->SetToneTermCount(localFindedUnits.size());
							unit1->SetUnitToneDict(this->GetType());
						}
					}	
					findedNumber++;
				}
				else if(itSent != sentenceUnits.end())
                    ++itSent;
				localFindedUnits.clear();
			}	
		}		
	}

	// begunit - начиная с него ищем
	void CObjectDescr::FindObjectInSentence(ISentence* sent, TUnitList& findedUnits, bool bOfficial, IUnit* begunit, bool isKorefs )
	{		
		list<IUnit*> sentenceUnits;
		sent->GetLinearUnits(sentenceUnits);
		FindObjectInUnits(sentenceUnits, findedUnits, bOfficial, begunit, isKorefs);
	}



	void CObjectDescr::AddChildUnits(IUnit* unit, list<IUnit*>& findedUnits)
	{
		IUnit* pChildUnit = unit->GetLeftChildUnit();
		int nChildUnit = 0;
		while(pChildUnit)
		{			
			AddChildUnits(pChildUnit, findedUnits);
			findedUnits.push_back(pChildUnit);
			wstring str = pChildUnit->GetWord();
			pChildUnit = pChildUnit->GetRightUnit();
		}
	//	findedUnits.push_back(unit);
		wstring str = unit->GetWord();
	}

	void CObjectDescr::SetType(wchar_t* type_name)
	{
		typeName = type_name;
	}

	wchar_t* CObjectDescr::GetType()
	{
		return (wchar_t*)typeName.c_str();
	}


	CWordInfo::CWordInfo()
	{
	}

	CTermInfo::CTermInfo()
	{
		weight = 0;
	}

	bool CWordInfo::isWordInForms(IUnit *pUnit)
	{			
		wstring word1 = pUnit->GetWord();
		if (word1.size()==2 && word1.c_str()[1]=='.')
            return false;
		for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
		{			
			SS::Dictionary::Types::TDictionaryIndex dictIndex;
			pIndex->GetDictionaryIndex()->GetFirst(&dictIndex);
			if (m_dictIndexes.size()==0)
			{
				if (m_listForms.find(word1)!=m_listForms.end())
					return true;
				else
					return false;					
			}
			else
            {
			    for(list<SS::Dictionary::Types::TDictionaryIndex>::iterator it = m_dictIndexes.begin();
                    it != m_dictIndexes.end();
                    ++it)
			    {			
				    SS::Dictionary::Types::TDictionaryIndex dictIndex_cur = *it;
				    int ind  = dictIndex_cur.GetId();
				    if (ind == dictIndex.GetId() &&
					    dictIndex_cur.GetDictionaryNumber() == dictIndex.GetDictionaryNumber())
				    {
					    return true;
				    }
			    }
            }
		}	
		return false;
	}

	bool CWordInfo::isWordInForms1(wstring word)
	{
		bool res = false;
		wstring lowerWord = _wcslwr((wchar_t*)word.c_str());
		if (m_listForms.find(lowerWord)!=m_listForms.end()) return true;
		return false;
	}


}
}
}
