#include "StdAfx.h"
#include ".\syntaxfeatureapriori.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			CSyntaxFeatureApriori::CSyntaxFeatureApriori(void)
				: m_VerbConstruction(Values::VerbConstruction.vcOnlyVerb)
				, m_NounConstruction(Values::NounConstruction.ncOnlyNoun)
				, m_AdjectiveConstruction(Values::AdjectiveConstruction.acOnlyAdjective)
				, m_TypeOfEndingObject(Values::TypeOfEndingObject.Undefined)
				, m_CombiType(Values::CombiType.Undefined)
				, m_SyntaxCategories(Values::SyntaxCategories.Undefined)
			{
				Init();
			}

			CSyntaxFeatureApriori::~CSyntaxFeatureApriori(void)
			{
			}

			void CSyntaxFeatureApriori::Init()
			{
				m_FeatureCollectionType = Base::fctSyntaxFeatureApriori;

				AddFeatureToCollection(&m_VerbConstruction);
				AddFeatureToCollection(&m_NounConstruction);
				AddFeatureToCollection(&m_AdjectiveConstruction);
				AddFeatureToCollection(&m_TypeOfEndingObject);
				AddFeatureToCollection(&m_CombiType);
				AddFeatureToCollection(&m_SyntaxCategories);
			};

			///условное пересечение с другим набором характеристик
			bool CSyntaxFeatureApriori::IsIntersectsWith(const CSyntaxFeatureApriori& value) const
			{
				if(m_FeatureCollectionType != value.m_FeatureCollectionType){
					char bff1[20];
					char bff2[20];
					SS_THROW(L"несовпадение типов характеристик при операции, текущий тип = " AND ltoa(m_FeatureCollectionType, bff1, 10) AND "; пришедший тип = " AND ltoa(value.m_FeatureCollectionType, bff2, 10));
				}

				unsigned int i = 0;
				while(i < m_FeatureCount){
					if(m_ppFeatures[i]->GetValue() != 0x00000001){
						if(m_ppFeatures[i]->IsIntersectsWith(value.m_ppFeatures[i]) == false) return false;
					}

					i++;
				}

				return true;
			};

		}
	}
}