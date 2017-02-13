#pragma once

#include ".\basefeaturecollection.h"
#include "define.h"
namespace SS
{
	namespace Core
	{
		namespace Features
		{
			/// полная апосториорная синтаксическая информация
			class CSyntaxFeatureAposteriori :
				public SS::Core::Features::Base::CBaseFeatureCollection
			{
			public:
				CSyntaxFeatureAposteriori(void);
				~CSyntaxFeatureAposteriori(void);

				CSyntaxFeatureAposteriori& operator = (const CSyntaxFeatureAposteriori& value)
				{
					Assign(&value);
					return *this;
				}
				CSyntaxFeatureAposteriori(const CSyntaxFeatureAposteriori& value){
					Init();
					*this = value;
				};

				///вернет строковое имя характеристики
				const char* GetFeatureName() const {return "SyntaxFeatureAposteriori";};
				///вернет строковое описание характеристики
				const char* GetFeatureDescription() const {return "полная апосториорная синтаксическая информация";};

				/// синтаксические категории
				Types::SyntaxCategories			m_SyntaxCategories;		
				/// точность определения синтаксической категории
				Types::SyntaxCategoryPower		m_SyntaxCategoryPower;
				/// номер родителя
					/*	не заполняется и не анализируется в данной версии синтаксического анализатора	*/
				Types::IdParent					m_IdParent;
				///	сила связи внутри конструкции
				Types::DependencyPower			m_DependencyPower;								
			private:
				void Init();
			};
		}
	}
}