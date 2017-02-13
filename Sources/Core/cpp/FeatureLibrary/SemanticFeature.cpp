#include "StdAfx.h"
#include ".\semanticfeature.h"

namespace SS
{
namespace Core
{
namespace Features
{
CSemanticFeature::CSemanticFeature(void)
	: m_SemanticType(Values::TSemanticType.Undefined)
	, m_STimeIntervalType(Values::TSTimeIntervalType.Undefined)
	, m_SPlaceType(Values::TSPlaceType.Undefined)
	, m_SMeasureType(Values::TSMeasureType.Undefined)
	, m_SPersonType(Values::TSPersonType.Undefined)
	, m_SOrganizationType(Values::TSOrganizationType.Undefined)
	, m_SVerbType(Values::TSVerbType.Undefined)
	, m_SVBeType(Values::TSVBeType.Undefined)
	, m_SVAcquirePossessType(Values::TSVAcquirePossessType.Undefined)
	, m_SVEndType(Values::TSVEndType.Undefined)
	, m_SVAcquireType(Values::TSVAcquireType.Undefined)
	, m_SServiceTypes(Values::TSServiceTypes.Undefined)
	, m_SPCity(Values::TCityType.Undefined)
	, m_SPCArea(Values::TAreaType.Undefined)
	, m_SPCADistrict(Values::TDistrictType.Undefined)
	, m_SPCADMetro(Values::TMetroType.Undefined)
	, m_SGeneric(Values::TGenericType.Undefined)
	, m_SInfoTypes(Values::TSInfoType.Undefined)
	, m_SMaterial(Values::TSMaterialType.Undefined)
	, m_STransport(Values::TSTransportType.Undefined)
	, m_SBuilding(Values::TSBuildingType.Undefine)
	, m_SMFood(Values::TSMFoodType.Undefined)
	, m_SMForFreeTime(Values::TSMForFreeTimeType.Undefined)
{
	Init();
}

CSemanticFeature::~CSemanticFeature(void)
{
}
////////////////////////////////////////////////////////////////////////
void CSemanticFeature::AssignSemanticFeature( CSemanticFeature const* pValue)
{
	if( !pValue ){
		throw std::invalid_argument( "NULL pointer" );
	}
	unsigned int nCount = min(GetFeatureCount(), pValue->GetFeatureCount());
	for( unsigned int nc = 0; nc < nCount; ++nc ){
		m_ppFeatures[nc]->Assign(pValue->m_ppFeatures[nc]);
	}// for
}
////////////////////////////////////////////////////////////////////////
void CSemanticFeature::Init()
{
	m_FeatureCollectionType = Base::fctSemanticFeature;
	
   ///////////////////////////////////////////////////////////////////////////////
   // ВНИМАНИЕ!!! При изменении семантических характеристик необходимо изменить 
   // реализацию функции GetNextLevelFeatureNumber
   ///////////////////////////////////////////////////////////////////////////////

	// level 1
	AddFeatureToCollection(&m_SemanticType);
	// level 2
	AddFeatureToCollection(&m_STimeIntervalType);
	AddFeatureToCollection(&m_SPlaceType);
	AddFeatureToCollection(&m_SMeasureType);
	AddFeatureToCollection(&m_SPersonType);
	AddFeatureToCollection(&m_SOrganizationType);			
	AddFeatureToCollection(&m_SVerbType);
	AddFeatureToCollection(&m_SServiceTypes);
	AddFeatureToCollection(&m_SInfoTypes);
	AddFeatureToCollection(&m_SMaterial);
	AddFeatureToCollection(&m_STransport);
	AddFeatureToCollection(&m_SBuilding);
	// level 3
	AddFeatureToCollection(&m_SVBeType);
	AddFeatureToCollection(&m_SVAcquirePossessType);
	AddFeatureToCollection(&m_SVEndType);

	AddFeatureToCollection(&m_SMFood);
	AddFeatureToCollection(&m_SMForFreeTime);
	// level 4
	AddFeatureToCollection(&m_SVAcquireType);
	// level 5
	AddFeatureToCollection(&m_SPCity);
	AddFeatureToCollection(&m_SPCArea);
	AddFeatureToCollection(&m_SPCADistrict);
	AddFeatureToCollection(&m_SPCADMetro);
	// level 6
	AddFeatureToCollection(&m_SGeneric);
};


bool CSemanticFeature::IsInclude(const CSemanticFeature& value) const
{
	//TODO: Переписать с использованием GetNextLevelFeatureNumber
   if(! m_SemanticType.Equal(Values::TSemanticType.Undefined)){
		if(! m_SemanticType.IsIntersectsWith(&value.m_SemanticType)) return false;
	}

	if(m_SemanticType.Equal(Values::TSemanticType.smtTimeInterval)){
		if(! m_STimeIntervalType.IsIntersectsWith(&value.m_STimeIntervalType)) return false;						
	}
	else if(m_SemanticType.Equal(Values::TSemanticType.smtPlace))	{							
		if(! m_SPlaceType.IsIntersectsWith(&value.m_SPlaceType)) return false;
	}
	else if(m_SemanticType.Equal(Values::TSemanticType.smtMeasure)){							
		if(! m_SMeasureType.IsIntersectsWith(&value.m_SMeasureType)) return false;						
	}
	else if(m_SemanticType.Equal(Values::TSemanticType.smtPerson)){							
		if(! m_SPersonType.IsIntersectsWith(&value.m_SPersonType)) return false;						
	}
	else if(m_SemanticType.Equal(Values::TSemanticType.smtOrganization)){							
		if(! m_SOrganizationType.IsIntersectsWith(&value.m_SOrganizationType)) return false;						
	}
	else if(m_SemanticType.Equal(Values::TSemanticType.smtVerb)){							
		if(! m_SVerbType.IsIntersectsWith(&value.m_SVerbType)) return false;					


		if(m_SVerbType.Equal(Values::TSVerbType.semvtBe)){
			if(! m_SVBeType.IsIntersectsWith(&value.m_SVBeType)) return false;
		}
		else if(m_SVerbType.Equal(Values::TSVerbType.semvtAcquirePossess)){
			if(! m_SVAcquirePossessType.IsIntersectsWith(&value.m_SVAcquirePossessType)) return false;

			if(m_SVAcquirePossessType.Equal(Values::TSVAcquirePossessType.tsvaptAcquire)){
				if(! m_SVAcquireType.IsIntersectsWith(&value.m_SVAcquireType)) return false;					
			}
		}
		else if(m_SVerbType.Equal(Values::TSVerbType.semvtEnd)){
			if(! m_SVEndType.IsIntersectsWith(&value.m_SVEndType)) return false;								
		}
	}

	return true;
};


bool CSemanticFeature::IsIncludeEx(const CSemanticFeature& value) const
{
   //TODO: Переписать с использованием GetNextLevelFeatureNumber
   if(! m_SemanticType.IsIntersectsWith(&value.m_SemanticType)) return false;						

	if(m_SemanticType.Equal(Values::TSemanticType.smtTimeInterval)){
		if(! m_STimeIntervalType.IsIntersectsWith(&value.m_STimeIntervalType)) return false;						
	}
	else if(m_SemanticType.Equal(Values::TSemanticType.smtPlace)){
		if(! m_SPlaceType.IsIntersectsWith(&value.m_SPlaceType)) return false;						
	}
	else if(m_SemanticType.Equal(Values::TSemanticType.smtMeasure)){
		if(! m_SMeasureType.IsIntersectsWith(&value.m_SMeasureType)) return false;						
	}
	else if(m_SemanticType.Equal(Values::TSemanticType.smtPerson)){
		if(! m_SPersonType.IsIntersectsWith(&value.m_SPersonType)) return false;						
	}
	else if(m_SemanticType.Equal(Values::TSemanticType.smtOrganization)){
		if(! m_SOrganizationType.IsIntersectsWith(&value.m_SOrganizationType)) return false;						
	}
	else if(m_SemanticType.Equal(Values::TSemanticType.smtVerb)){
		if(! m_SVerbType.IsIntersectsWith(&value.m_SVerbType)) return false;					


		if(m_SVerbType.Equal(Values::TSVerbType.semvtBe)){
			if(! m_SVBeType.IsIntersectsWith(&value.m_SVBeType)) return false;
			
		}
		else if(m_SVerbType.Equal(Values::TSVerbType.semvtAcquirePossess)){
			if(! m_SVAcquirePossessType.IsIntersectsWith(&value.m_SVAcquirePossessType)) return false;

			if(m_SVAcquirePossessType.Equal(Values::TSVAcquirePossessType.tsvaptAcquire)){
				if(! m_SVAcquireType.IsIntersectsWith(&value.m_SVAcquireType)) return false;					
			}
		}
		else if(m_SVerbType.Equal(Values::TSVerbType.semvtEnd)){
			if(! m_SVEndType.IsIntersectsWith(&value.m_SVEndType)) return false;								
		}
	}

	return true;
};

///Пересечение множеств (для пересечения непосредственно с CSemanticFeature)
bool CSemanticFeature::IsIntersectsOrEmpty(const CSemanticFeature & oValue) const
{
	using namespace Values;
	if(m_SemanticType.IsUndefined())
	{ return true; }

	bool bRetVal =
		(m_SemanticType.IsIntersectsOrEmpty(&oValue.m_SemanticType)&&
		(
		((m_SemanticType.GetValue() & TSemanticType.smtTimeInterval.GetValue())&& m_STimeIntervalType.IsIntersectsOrEmpty(&oValue.m_STimeIntervalType))||
		((m_SemanticType.GetValue() & TSemanticType.smtPlace.GetValue())&& m_SPlaceType.IsIntersectsOrEmpty(&oValue.m_SPlaceType))||
		((m_SemanticType.GetValue() & TSemanticType.smtMeasure.GetValue())&& m_SMeasureType.IsIntersectsOrEmpty(&oValue.m_SMeasureType))||
		((m_SemanticType.GetValue() & TSemanticType.smtPerson.GetValue())&& m_SPersonType.IsIntersectsOrEmpty(&oValue.m_SPersonType))||
		((m_SemanticType.GetValue() & TSemanticType.smtOrganization.GetValue())&& m_SOrganizationType.IsIntersectsOrEmpty(&oValue.m_SOrganizationType))||
		((m_SemanticType.GetValue() & TSemanticType.smtVerb.GetValue()) && m_SVerbType.IsIntersectsOrEmpty(&oValue.m_SVerbType))||
		((m_SemanticType.GetValue() & TSemanticType.smtService.GetValue()) && m_SServiceTypes.IsIntersectsOrEmpty(&oValue.m_SServiceTypes))
		));
	return bRetVal;
}

///Пересечение множеств (для Natural Query) с учетом уровней TCity TArea TDistrict TMetro
bool CSemanticFeature::IsIntersectsOrEmptyDeep(const CSemanticFeature & oValue) const
{
	using namespace Values;
	if (m_SemanticType.IsUndefined())
	{ return true; }
	
	//Обязательное пересечение
	if ( !( m_SemanticType.IsIntersectsWith(&oValue.m_SemanticType) ) || 
		 !( m_SGeneric.IsIntersectsOrEmpty(&oValue.m_SGeneric) ) ) 
	{ return false; }
	
	//Проверка PlaceType
	bool bValue = false;
	if ( m_SemanticType.GetValue() & TSemanticType.smtPlace.GetValue() )
	{
		bValue = m_SPlaceType.IsIntersectsWith(&oValue.m_SPlaceType);
		if ( bValue && (m_SPlaceType.GetValue() & TSPlaceType.pltCity.GetValue()) )
		{
			bValue = m_SPCity.IsIntersectsWith(&oValue.m_SPCity);
			if ( bValue && (m_SPCity.GetValue() & TCityType.smtcityArea.GetValue()) )
			{
				bValue = m_SPCArea.IsIntersectsWith(&oValue.m_SPCArea);
				if ( bValue && (m_SPCArea.GetValue() & TAreaType.smtareaDistrict.GetValue()) )
				{
					bValue = m_SPCADistrict.IsIntersectsWith(&oValue.m_SPCADistrict);
					if ( bValue && (m_SPCADistrict.GetValue() & TDistrictType.smtDistrMetro.GetValue()) ) 
					{ bValue = m_SPCADMetro.IsIntersectsWith(&oValue.m_SPCADMetro); }
				}
			}
		}
	}

	//Проверка остальных
	bool bResultValue = (
		(  bValue  ) ||
		( (m_SemanticType.GetValue() & TSemanticType.smtTimeInterval.GetValue()) && m_STimeIntervalType.IsIntersectsWith(&oValue.m_STimeIntervalType) ) ||
		( (m_SemanticType.GetValue() & TSemanticType.smtMeasure.GetValue()	   ) && m_SMeasureType.IsIntersectsWith(&oValue.m_SMeasureType)			 ) ||
		( (m_SemanticType.GetValue() & TSemanticType.smtPerson.GetValue()	   ) && m_SPersonType.IsIntersectsWith(&oValue.m_SPersonType)			 ) ||
		( (m_SemanticType.GetValue() & TSemanticType.smtOrganization.GetValue()) && m_SOrganizationType.IsIntersectsWith(&oValue.m_SOrganizationType) ) ||
		( (m_SemanticType.GetValue() & TSemanticType.smtVerb.GetValue()		   ) && m_SVerbType.IsIntersectsWith(&oValue.m_SVerbType)				 ) ||
		( (m_SemanticType.GetValue() & TSemanticType.smtService.GetValue()	   ) && m_SServiceTypes.IsIntersectsWith(&oValue.m_SServiceTypes)		 )
					  );
	
	return bResultValue;
}

unsigned int CSemanticFeature::GetNextLevelFeatureNumber( unsigned int CurFeatureNumber, unsigned int CurFeatureValue )
{
   //Максимальное число которое может вернуть CMapFeature::GetSingleValue. 
   //-1 т.к. мы не используем значения Undefined
   static const size_t MAX_FEATURE_VALUE = sizeof(unsigned int) * CHAR_BIT - 1;

   //Номера характеристик CBaseFeatureCollection::GetFeature
   enum TFeaturesNumber
   {
      //1-й уровень семантической иерархии
      FNSemanticType,

      //2-й уровень семантической иерархии
      FNTimeIntervalType,
      FNPlaceType,
      FNMeasureType,
      FNPersonType,
      FNOrganizationType,		
      FNVerbType,
      FNServiceTypes,

      //3-й уровень семантической иерархии
      // глагольные семантические типы
      FNVBeType,
      FNVAcquirePossessType,
      FNVEndType,

      //4-й уровень семантической иерархии				
      FNVAcquireType,

      FNCOUNT
   };
   
   //Карта в которой перым измерением содержится номер текущей характеристики, вторым измерением - 
   //значение текущей характеристики - 1 (не обрабатываем значения Undefined), и результатом 
   //номер следующей характеристики
   static const unsigned int rgLevelsMap[FNCOUNT][MAX_FEATURE_VALUE] = {
      /* FNSemanticType        */ { FNTimeIntervalType, FNPlaceType, FNMeasureType, FNPersonType, FNOrganizationType, FNVerbType, FNServiceTypes },
      
      /* FNTimeIntervalType    */ { },
      /* FNPlaceType           */ { },
      /* FNMeasureType         */ { },
      /* FNPersonType          */ { },
      /* FNOrganizationType    */ { },	
      /* FNVerbType            */ { FNVBeType, FNVAcquirePossessType, 0, FNVEndType },
      /* FNServiceTypes        */ { },

      /* FNVBeType             */ { },
      /* FNVAcquirePossessType */ { FNVAcquireType },
      /* FNVEndType            */ { },

      /* FNVAcquireType        */ { }
   };

   if( !CurFeatureValue )
      return 0;

   return rgLevelsMap[CurFeatureNumber][CurFeatureValue - 1];
}

///Пересечение базовой части
bool CSemanticFeature::IsIntersectsWith(const CSemanticFeature & oValue) const
{
	unsigned int iCount = min(GetFeatureCount(), oValue.GetFeatureCount());
	for (unsigned int i = 0; i < iCount; ++i)
	{
		if ( !m_ppFeatures[i]->IsIntersectsWith(oValue.m_ppFeatures[i]) )
		{ return false;	}
	}
	return true;
}

}
}
}
