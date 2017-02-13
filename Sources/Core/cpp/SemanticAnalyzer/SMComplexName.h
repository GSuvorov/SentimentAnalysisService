#pragma once
#include "./smsentenceexpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{


			class CSMComplexName :
				public CSMSentenceExpert
			{
			public:
				CSMComplexName(void);
				~CSMComplexName(void);
				bool AnalizeSentence();
				//bool Init(IText * pIText);
			private:
				//struct 
				/// описание семантики слова
				struct SSmanticDescription{
					/// семантика слова
					SS::Core::Features::CSemanticFeature _oSemanticFeature;
					/// слово - есть имя собственное
					bool _bProper;
					SSmanticDescription(){  _bProper=false; };
				};
			//typedefs
				/// список описаний слов
				typedef list<CSMComplexName::SSmanticDescription> TLSmanticDescription;
			//attributes
				/// семантика - цитаты
				long m_lSemanticCitation;
				///
				SS::Core::Features::CMorphoFeature m_MorphoFeature;

			//methods
				/// пороверяет на допустимость текущее слово
				bool IsValidWord();
				/// выделяет составное имя собственное
				/** 
					\param oDesc - описание группы слов
					\param l_units - соответствующие группе юниты
				*/
				void DetectProper(CSMComplexName::TLSmanticDescription& oDesc, TBoxUnits& l_units);
				/// заполняет семантическую информацию для первоначально выделенной группы слов
				/** 
					\param l_units - группа слов
					\param oDesc - список семанттического описания к заполнению
				*/
				void ProcessUnits(TBoxUnits & l_units, CSMComplexName::TLSmanticDescription& oDesc);
				/// интерпретация собранной инфомации для группы слов и преобразование её в семантические индексы
				/** 
					\param oSemanticFeature - семантика группы
					\param oPersonType - семантика группы относящаяся к типу Person
					\param uiNonProperCount - число допустимых в группе слов не имён собственных
					\param l_SemGeo - семантика относящаяся к месту
					\param l_Sem - вся другая семантика
					\param bPerson - истина - если в результате оказалось что группа - имя 
					\return bool - истина - если всё что то есть в результате
				*/
				bool ConvertFeatureToType(SS::Core::Features::CSemanticFeature & oSemanticFeature,SS::Core::Features::Types::TSPersonType&  oPersonType,  UINT uiNonProperCount , list<long> & l_SemGeo, list<long> & l_Sem, bool & bPerson);
				/// сливает семантику группы 
				/** 
					\param oDesc - группа слов
					\param tsemFeature - итоговая семантика по And
					\param oPersonType - итоговая семантика по Or 
					\return UINT - кол-во слов без семантики в группе
				*/
				UINT MergeSemantic(CSMComplexName::TLSmanticDescription& oDesc,SS::Core::Features::CSemanticFeature & tsemFeature, SS::Core::Features::Types::TSPersonType & oPersonType);
				/// проверка группы на корректность
				/** 
					\param l_units - группа слов
					\param l_SemGeo  - семантика относящаяся к мемту
					\param l_Sem - остальная семантика
					\return bool - юнит объединяющий группу
				 
				-detailed description- 
				*/
				IUnit * CheckSemantic(TBoxUnits & l_units , list<long> & l_SemGeo, list<long> & l_Sem );

			};

		}
	}
}