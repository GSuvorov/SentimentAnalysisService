#pragma once

#include ".\basefeaturecollection.h"
#include "define.h"
namespace SS
{
	namespace Core
	{
		namespace Features
		{
			/// полная априорная синтаксическая информация
			class CSyntaxFeatureApriori :
				public SS::Core::Features::Base::CBaseFeatureCollection
			{
			public:
				CSyntaxFeatureApriori(void);
				~CSyntaxFeatureApriori(void);

				CSyntaxFeatureApriori& operator = (const CSyntaxFeatureApriori& value)
				{
					Assign(&value);
					return *this;
				}
				CSyntaxFeatureApriori(const CSyntaxFeatureApriori& value){
					Init();
					*this = value;
				};

 				///вернет строковое имя характеристики
				const char* GetFeatureName() const {return "SyntaxFeatureApriori";};
				///вернет строковое описание характеристики
				const char* GetFeatureDescription() const {return "полная априорная синтаксическая информация";};

				/// тип конструкции глагола
				Types::VerbConstruction			m_VerbConstruction;
				/// тип конструкции существительного
				Types::NounConstruction			m_NounConstruction;
				/// тип конструкции прилагательного
				Types::AdjectiveConstruction	m_AdjectiveConstruction;
				/// тип последнего объекта
				Types::TypeOfEndingObject		m_TypeOfEndingObject;		
				/// тип комбинации
				Types::CombiType				m_CombiType;	
				/// синтаксическая категория
				Types::SyntaxCategories			m_SyntaxCategories;		

				///условное пересечение с другим набором характеристик
				bool IsIntersectsWith(const CSyntaxFeatureApriori& value) const;
			private:
				void Init();
			};
		}
	}
}