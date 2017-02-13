#pragma once

#include ".\Semanticbitfieldsstructures.h"

#include "..\ASSInterface\TCoSeDiDataTypes.h"


//using namespace SS::Core::SMISL;

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			using namespace bitfieldsstructures;
			/// отвечает за кодирование/ декодирование семантических характеристик
			class CSemanticConverter
			{
			public:
				/// конструктор
				CSemanticConverter(void);
				/// деструктор
				~CSemanticConverter(void);

				/// кодирует набор семантических характеристик в число
				/** 
					\param *pSemanticFeature - набор семантических характеристик
					\param piSymanticType - число, которое содержит закодированные семантические характеристики
					\return void
				*/
				bool Code(/*in*/CSemanticFeature *pSemanticFeature, /*out*/SS::Dictionary::Types::TDictionaryIndex* pDictionaryIndex);
								/// декодирует число в набор семантических характеристик
				/** 
					\param iSymanticType - число, которое содержит закодированные семантические характеристики
					\param *pSemanticFeature - набор семантических характеристик
					\return void
				*/
				bool EnCode(/*in*/SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex, /*out*/CSemanticFeature *pSemanticFeature);
			private:
				/// раскодирует семантические характеристики глаголов
				/** 
					\param SS::AMConverter::Semantic::bitfieldsstructures::SbfSemanticTypeoSemanticType -  структура, содержащая набор характеристик для кодирования семантических иерархий
					\param *pSemanticFeature - указатель на структуру, содержащую набор семантических характеристик
				*/
				void EnCodeVerbSemantics(bitfieldsstructures::SbfSemanticType	oSemanticType, /*out*/CSemanticFeature *pSemanticFeature);
				/// -brief- 
				/** 
					\param SS::AMConverter::Semantic::bitfieldsstructures::SbfSemanticType&oSemanticType -  структура, содержащая набор характеристик для кодирования семантических иерархий
					\param *pSemanticFeature - указатель на структуру, содержащую набор семантических характеристик
				*/
				void CodeVerbSemantics(bitfieldsstructures::SbfSemanticType&	oSemanticType, /*out*/CSemanticFeature *pSemanticFeature);

				/// кодирует семантические характеристики Food
				void CodeMaterial(bitfieldsstructures::SbfSemanticType&	oSemanticType, /*out*/CSemanticFeature *pSemanticFeature);
				/// раскодирует семантические характеристики Food
				void EnCodeMaterial(bitfieldsstructures::SbfSemanticType	oSemanticType, /*out*/CSemanticFeature *pSemanticFeature);

			};
		}
	}
}

