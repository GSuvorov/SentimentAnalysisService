#pragma once
#pragma warning( disable : 4290 )
#include <stdexcept> 
#include ".\basefeaturecollection.h"
#include "define.h"
namespace SS
{
namespace Core
{
namespace Features
{
/// семантические характеристики
class CSemanticFeature :
	public SS::Core::Features::Base::CBaseFeatureCollection
{
public:
	CSemanticFeature(void);
	~CSemanticFeature(void);
	CSemanticFeature& operator = (const CSemanticFeature& value)
	{
		Assign(&value);
		return *this;
	}

	CSemanticFeature(const CSemanticFeature& value){
		Init();
		*this = value;
	};

	///вернет строковое имя характеристики
	const char* GetFeatureName() const {return "SemanticFeature";};
	///вернет строковое описание характеристики
	const char* GetFeatureDescription() const {return "семантические характеристики";};
	//
	void AssignSemanticFeature( CSemanticFeature const* pValue)throw( std::invalid_argument );
//БЛОК ХАРАКТЕРИСТИК

///////////////////////////////////////////////////////////////////////////////
// ВНИМАНИЕ!!! При изменении семантических характеристик необходимо изменить 
// реализацию функции GetNextLevelFeatureNumber
///////////////////////////////////////////////////////////////////////////////

	//1-й уровень семантической иерархии
	Types::TSemanticType			m_SemanticType;

	//2-й уровень семантической иерархии
	Types::TSTimeIntervalType		m_STimeIntervalType;
	Types::TSPlaceType				m_SPlaceType;
	Types::TSMeasureType			m_SMeasureType;
	Types::TSPersonType				m_SPersonType;
	Types::TSOrganizationType		m_SOrganizationType;			
	Types::TSVerbType				m_SVerbType;
	Types::TSServiceTypes			m_SServiceTypes;
	Types::TSInfo					m_SInfoTypes;
	Types::TSMaterial				m_SMaterial;
	Types::TSTransport				m_STransport;
	Types::TSBuilding				m_SBuilding;

	//3-й уровень семантической иерархии
	// глагольные семантические типы
	Types::TSVBeType				m_SVBeType;
	Types::TSVAcquirePossessType	m_SVAcquirePossessType;
	Types::TSVEndType				m_SVEndType;

	Types::TSMFood					m_SMFood;
	Types::TSMForFreeTime			m_SMForFreeTime;
	
	//4-й уровень семантической иерархии				
	Types::TSVAcquireType			m_SVAcquireType;
	
	//5-й уровень семантической иерархии	
	Types::TCity					m_SPCity;
	Types::TArea					m_SPCArea;
	Types::TDistrict				m_SPCADistrict;
	Types::TMetro					m_SPCADMetro;

	//6-й уровень семантической иерархии
	Types::TGeneric					m_SGeneric;

	//ФУНКЦИИ, СПЕЦИФИЧНЫЕ ДЛЯ ДАННОГО КЛАССА
	bool CSemanticFeature::IsInclude(const CSemanticFeature& value) const;
	bool CSemanticFeature::IsIncludeEx(const CSemanticFeature& value) const;
	///Пересечение множеств
	bool IsIntersectsOrEmpty(const CSemanticFeature & pValue) const;
	///Пересечение множеств с учетом уровней TCity TArea TDistrict TMetro
	bool IsIntersectsOrEmptyDeep(const CSemanticFeature & pValue) const;
	///Пересечение базовой части
	bool IsIntersectsWith(const CSemanticFeature & oValue) const;

/// Функция позволяет востановить иерархию уровней семантических характеристик
/// Семантические характеристики разделены на несколько уровней, причём некоторые значения 
/// характеристик одного уровня требуют уточнения по средством характеристик следующего уровня.
/// Функция опираясь на номер текущей характеристики CurFeatureNumber (CBaseFeatureCollection::GetFeature)
/// и на её значение CurFeatureValue (CMapFeature::GetSingleValue) возвращает номер характеристики следующего
/// уровня или 0 если текущий уровень является последним.
static unsigned int GetNextLevelFeatureNumber( unsigned int CurFeatureNumber, unsigned int CurFeatureValue );
	///Базовый метод
	using CBaseFeatureCollection::IsIntersectsOrEmpty;
	using CBaseFeatureCollection::IsIntersectsWith;

private:
	void Init();
};
}
}
}