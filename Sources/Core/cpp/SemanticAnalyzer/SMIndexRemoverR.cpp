#include "StdAfx.h"
#include ".\smindexremoverr.h"
#include "..\assinterface\TDictionaryNames.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Question
			{
				namespace Russian
				{
					CSMIndexRemoverR::CSMIndexRemoverR(void) : m_uiObject(0), m_uiSubject(0)
					{
						m_bInit = false;
						m_AnalyseLanguage=SS::Core::Types::ealRussian;
						m_sDescription=_T(__FUNCTION__);
					}

					CSMIndexRemoverR::~CSMIndexRemoverR(void)
					{
					}

					bool CSMIndexRemoverR::AnalizeSentence()
					{
						SS_TRY
						{
							IUnit * pUnit;
							SS::Core::Features::CSyntaxFeatureAposteriori * oSyntaxFeatureAposteriori;
							Types::SyntaxCategories oSORemoveCategory=Values::SyntaxCategories.scPredicate;
							oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scAjunct);
							oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scWhoDefinition);
							oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scWhatDefinition);
							oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scAjunctPlaceEnd);
							oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scAjunctPlaceFixedPoint);
							oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scAjunctPlaceSource);
							oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scAjunctTimeDuration);
							oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scAjunctTimeEnd);
							oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scAjunctTimeFixedPoint);
							oSORemoveCategory.AssignOR(&Values::SyntaxCategories.scAjunctTimeSource);

							if (!GetFirstUnit()) return true;;
							do{
								//wstring str = m_pCurrentUnit->GetWord();
								if (m_pCurrentUnit->GetSyntaxFeature()  )
								{
									oSyntaxFeatureAposteriori=m_pCurrentUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori();

									if(oSyntaxFeatureAposteriori->m_SyntaxCategories.IsIntersectsWith(&oSORemoveCategory))
									{
										RemoveSubjectObjectIndices(m_pCurrentUnit);
										if(m_pCurrentUnit->HasChilds() && (pUnit=m_pCurrentUnit->GetLeftChildUnit()))
										{
											do{
												RemoveSubjectObjectIndices(pUnit);										
											}while(pUnit=pUnit->GetRightUnit());
										}
									}
								}
								if (!(m_pCurrentUnit->GetSyntaxFeature() && m_pCurrentUnit->GetSyntaxFeature()->IsQuestion()))
								{
									if(!RemoveSemanticFromUnits(m_pCurrentUnit))
										RemoveSemanticIndices(m_pCurrentUnit);
									if(!RemoveSyntIndexFromUnits(m_pCurrentUnit,Values::SyntaxCategories.scPrepositionObject))
										RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scPrepositionObject,m_pCurrentUnit);
								}
							}while(GetNextUnit());

							return true;
						}
						SS_CATCH(L"")
					}

					bool CSMIndexRemoverR::RemoveSemanticFromUnits(IUnit * pGroup)
					{
						if(!pGroup->HasChilds())
							return false;

						RemoveSemanticIndices(pGroup);

						IUnit * pUnit=pGroup->GetLeftChildUnit();
						do
						{
							//wstring str = pUnit->GetWord();
							if (!RemoveSemanticFromUnits(pUnit))
								RemoveSemanticIndices(pUnit);
							else
								RemoveSemanticIndices(pUnit);
						}
						while ( pUnit=pUnit->GetRightUnit() );

						return true;
					}

					bool CSMIndexRemoverR::RemoveSyntIndexFromUnits(IUnit * pGroup, const Types::SyntaxCategories & oCategory)
					{
						if(!pGroup->HasChilds())
							return false;
						
						DeleteSyntaxCategory(pGroup,oCategory);
						RemoveSyntaxCategoryIndex(oCategory,pGroup);
						RemoveSubjObjFromUnit(pGroup);

						IUnit * pUnit=pGroup->GetLeftChildUnit();
						do
						{
							DeleteSyntaxCategory(pUnit,oCategory);
							if (!RemoveSyntIndexFromUnits(pUnit,oCategory))
							{
								//wstring str = pUnit->GetWord();
								RemoveSyntaxCategoryIndex(oCategory,pUnit);
								RemoveSubjObjFromUnit(pUnit);
							}
							else
							{
								RemoveSyntaxCategoryIndex(oCategory,pUnit);
								RemoveSubjObjFromUnit(pUnit);
							}
						}
						while ( pUnit = pUnit->GetRightUnit() );

						return true;
					}

					bool CSMIndexRemoverR::Init(IText * pIText)
					{
						if (!CSMExpert::Init(pIText)) return false;

						if(!m_bInit)
						{
							m_bInit = true;

							m_uiSubject=Values::SyntaxCategories.scSubjectFin.GetSingleValue() + (SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24);
							m_uiObject=Values::SyntaxCategories.scObject.GetSingleValue() + (SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24);
						}

						return true;
					}

					void CSMIndexRemoverR::RemoveSubjectObjectIndices(IUnit * pUnit)
					{
						IIndex * pIndex;
						UINT uiIndex;
						if (pIndex=pUnit->GetFirstIndex())
						{
							do{
								//uiIndex=pIndex->GetDictionaryIndex()->GetFirst();
								uiIndex=pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndex();
								if (uiIndex==m_uiObject || uiIndex==m_uiSubject )
								{
									pIndex=pIndex->ReleaseIndex();
								}else pIndex=pUnit->GetNextIndex();
							}while(pIndex);
						}
					}

					void CSMIndexRemoverR::RemoveSemanticIndices(IUnit * pUnit)
					{
						IIndex * pIndex;
						if (pIndex=pUnit->GetFirstIndex())
						{
							do{
								//if(pIndex->GetDictionaryIndex()->GetIndexType() == SS::Interface::Core::BlackBox::eitSemanticIndex)
								if(pIndex->GetDictionaryIndex()->GetFirst().GetIndexType() == SS::Interface::Core::BlackBox::eitSemanticIndex)
								{
									pIndex=pIndex->ReleaseIndex();
								}else pIndex=pUnit->GetNextIndex();
							}while(pIndex);
						}
					}

					void CSMIndexRemoverR::RemoveSubjObjFromUnit(IUnit * pUnit)
					{
						if(!pUnit) return;

						if(IsContainSyntaxCategoryIndex(Values::SyntaxCategories.scWhoDefinition,pUnit)||
							IsContainSyntaxCategoryIndex(Values::SyntaxCategories.scWhatDefinition,pUnit))
							RemoveSubjectObjectIndices(pUnit);

						return;
					}

				}

			}
		}
	}
}