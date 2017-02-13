#include "stdafx.h"
#include "MorphoListElement.h"

namespace SS
{
namespace Syntax
{
namespace BoardElements
{
	
	///Инициализация элемента морфоллогии
	void CMorphoListElement::Init( SS::Interface::Core::AMConverter::IAMConverterMorpho* pAMConverterMorpho,
								   SS::Interface::Core::BlackBox::IFeature* pFeature, 
								   SS::Interface::Core::BlackBox::IIndex* pIndex, int unit_num, bool isPerson ) 
	{
		SS_TRY
		{		
			if ( !pAMConverterMorpho ) 
			{
				SS_THROW(L"!pAMConverterMorpho"); 
			}
			
			m_oMorphoFeature.Undefine();
			m_pFeature = pFeature; 
			m_pIndex = pIndex;
			m_pAMConverterMorpho = pAMConverterMorpho;

			if ( m_pIndex->GetDictionaryIndex()->IsComposite() ) 
			{
				m_oMorphoFeature.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;
			}
			//else
			//{
				SS::Dictionary::Types::TDictionaryIndex oIndexVal;
				m_pIndex->GetDictionaryIndex()->GetFirst(&oIndexVal);

				m_pAMConverterMorpho->EnCode( oIndexVal.GetDictionaryNumber(), m_pFeature->GetMorphoInfo(), 
											  m_pFeature->GetPartOfSpeech(), &(m_oMorphoFeature) ); 
				
				if ((isPerson 
					/*&& 
					(oIndexVal.GetDictionaryNumber()==SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus ||
					 oIndexVal.GetDictionaryNumber()==SS::Dictionary::DATA_TYPE::NAMES::ednUnknownEng)*/)
					|| (oIndexVal.GetFirstLetterUpperCase() && unit_num != 0))  
				{
					if (oIndexVal.GetLanguage()==SS::Core::Types::ealEnglish)
					{
					 if (oIndexVal.GetDictionaryNumber()!=SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory)
					   oIndexVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednEProperName);
					}
					else
					{
					 if (oIndexVal.GetDictionaryNumber()!=SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory)
					   oIndexVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednRProperName);
					}
					//pIndex->GetDictionaryIndex()->AppendIndex(oIndexVal);
					m_oMorphoFeature.m_CommonType.Assign(SS::Core::Features::Values::CommonType.cmtProper);
				}
		//	}
		}
		SS_CATCH(L"")
	}

	SS::Core::Features::Types::PartOfSpeechTypes CMorphoListElement::GetPOST() const
	{
		return m_oMorphoFeature.m_PartOfSpeechTypes;
	}

	SS::Core::Features::Types::VerbForm CMorphoListElement::GetVerbForm() const
	{
		return m_oMorphoFeature.m_VerbForm;
	}

	SS::Core::Features::Types::TransitionalType CMorphoListElement::GetTransType() const
	{
		return m_oMorphoFeature.m_TransitionalType;
	}

	///Преобразование CMorphoFeature в IFeature 
	void CMorphoListElement::UpdateIFeature()
	{
		SS_TRY
		{
			if (m_oMorphoFeature.IsUndefined())
			{ return; }
			int iMorphoInfo;
			int iPartOfSpeech;
			
			SS::Dictionary::Types::TDictionaryIndex oIndexVal;
			m_pIndex->GetDictionaryIndex()->GetFirst(&oIndexVal);
			
			m_pAMConverterMorpho->Code(oIndexVal.GetDictionaryNumber(), &(m_oMorphoFeature), &(iMorphoInfo), &(iPartOfSpeech) );
			m_pFeature->SetMorphoInfo(iMorphoInfo);
			m_pFeature->SetPartOfSpeech(iPartOfSpeech);
		}
		SS_CATCH(L"")
	}

	///Получение указателя на IIndex
	SS::Interface::Core::BlackBox::IIndex* CMorphoListElement::GetIndex()
	{
		return m_pIndex;	
	}

	//operator==
	//bool CMorphoListElement::operator==(const CMorphoListElement & rMorpho)
	//{
		/*if ( (GetPOST().GetValue() == SS::Core::Features::Values::PartOfSpeechTypes.postVerb.GetValue()) &&
			 (rMorpho.GetPOST().GetValue() == SS::Core::Features::Values::PartOfSpeechTypes.postVerb.GetValue()) )
		{
			if (GetVerbForm().GetValue() != rMorpho.GetVerbForm().GetValue())
			{ return (GetVerbForm().GetValue() < rMorpho.GetVerbForm().GetValue()); }
			else
			{ return (GetTransType().GetValue() < rMorpho.GetTransType().GetValue()); }
		}
		else
		{ return (GetPOST().GetValue() < rMorpho.GetPOST().GetValue()); }*/
	//}

}
}
}



















