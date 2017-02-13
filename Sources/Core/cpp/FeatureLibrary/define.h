#pragma once

namespace SS{
	namespace Core{
		namespace Features{
			///набор морфологических характеристик
			class CMorphoFeature;
		}
	}
}

#include "BaseType.h"
//#include "MapDefinitionMorpho.h"
//#include "MapDefinitionSemantic.h"
//#include "MapDefinitionSyntax.h"
#include "AutoFeatures.h"

#include "morphofeature.h"
#include "ObjectFeature.h"
//#include "MorphoFeatureApriori.h"
//#include "MorphoFeatureAposteriori.h"
#include "SyntaxFeatureApriori.h"
#include "SyntaxFeatureAposteriori.h"
#include "ConstructionFeature.h"
#include "SemanticFeature.h"

namespace SS{
	namespace Core{
		namespace Features{
			namespace Types{
#pragma warning (disable : 4091)
				///тип значения часть речи
				typedef PartOfSpeechTypes;
				///тип значения части речи
				typedef PartOfSpeechTypes;
				/// тип значения одушевленность
				typedef AnimInAnimType;
				/// тип значения уникальность характеристики
				typedef Uniqueness;
				///	тип значения выполняемая операция
				typedef TDetectWordAnalyzeMode;
				///	тип значения выполняемая операция
				typedef OperationMod;
				///	тип значения режим определения
				typedef ExistenceMod;
				///	тип значения вид глаголов
				typedef AspectType;
				///	тип значения тип наречия
				typedef AdverbType;
				///	тип значения падеж
				typedef CaseType;
				///	тип значения исчисляемость
				typedef CountType;
				///	тип значения нарицательность
				typedef CommonType;
				///	тип значения степень сравнения
				typedef DegreeType;
				///	тип значения род
				typedef GenderType;
				///	тип значения число
				typedef NumberType;
				///	тип значения тип числительного
				typedef NumericType;
				///	тип значения характеристика отрицания
				typedef NegativeType;
				///	тип значения фазисность
				typedef PhaseVerbType;
				///	тип значения лицо
				typedef PersonType;
				///	тип значения группа местоимений
				typedef PronounGroup;
				///	тип значения класс местоимений
				typedef PronounClass;
				///	тип значения краткость
				typedef ShortType;
				///	тип значения класс наречий
				typedef TAdverbClass;
				///	тип значения тип союза
				typedef TConjunctionType;
				///	тип значения тип определителя
				typedef TDeterminerType;
				///	тип значения время
				typedef TimeType;
				///	тип значения переходность
				typedef TransitionalType;
				///	тип значения залог
				typedef VoiceType;
				///	тип значения форма глагола
				typedef VerbForm;
				///	тип значения тип глагола
				typedef VerbType;
				///	тип значения наклонение
				typedef MoodType;

			//ТИПЫ СЕМАНТИКИ
				/// семантические типы
				typedef TSemanticType;
				/// семантический тип времени
				typedef TSTimeIntervalType;
				/// тип места
				typedef TSPlaceType;
				/// тип города
				typedef TCityType;//		TCityType;
				/// тип округа
				typedef TAreaType;//		TAreaType;
				/// тип района
				typedef TDistrictType;//	TDistrictType;
				/// тип метро
				typedef TMetroType;//		TMetroType;


				typedef TSMaterial;
				typedef TSTransport;
				typedef TSBuilding;
				typedef TSMFood;
				typedef TSMForFreeTime;

				/// тип контактной информации
				typedef TSInfo;

				/// типы мер
				typedef TSMeasureType;
				/// семантические типы, применимые к людям
				typedef TSPersonType;
				/// Типы организаций
				typedef TSOrganizationType;
				/// Семантические типы глаголов
				typedef TSVerbType;
				///
				typedef TSVBeType;
				///
				typedef TSVAcquirePossessType;
				///
				typedef TSVAcquireType;
				///
				typedef TSVEndType;
				///
				//typedef TSVMoveType;
				///
				//typedef TSVEmotionType;
				///
				//typedef TSVExperiencedType;
				///
				//typedef TSVSearchType;
				///
				//typedef TSServiceTypes;

			// НАБОРЫ СИНТАКСИСА
				/// глагольные конструкции
				typedef VerbConstruction;
				/// конструкции существительного
				typedef NounConstruction;
				/// конструкции прилагательного
				typedef AdjectiveConstruction;
				/// тип последнего объекта
				typedef TypeOfEndingObject;
				///
				typedef CombiType;
				/// синтаксические категории (тип связи
				typedef SyntaxCategories;
				/// направление зависимости
				typedef HeadDependentType;
				/// тип предложения
				typedef SentenceType;
				/// порядок следования объектов
				typedef ObjectsOrder;
				/// положение по отношению к голове
				typedef PositionToDetectedObject;
				/// Обязательность связи
				typedef ObjectObligation;
				/// тип согласования
				typedef AgreementType;
				/// область действия правила
				typedef ScopeOfTheRule;
				///
				typedef PositionInBlock;

				/// сила связи
				typedef DependencyPower;
				/// точность определения синтаксической категории
				typedef SyntaxCategoryPower;
				/// Номер родителя
				typedef IdParent;
				/// колличество объектов одного типа
				typedef SameTypeObjectsNumber;
				/// глубина анализа детей
				typedef ChildrenAnalysisDepth;       
            /// Номер предложения
            typedef SentenceNumber;
            /// Наследование синтаксических характеристик родительского элемента
				typedef SyntaxParamsInheritanceMode;

#pragma warning (default : 4091)
			}
		}
	}
}

//static 
namespace SS{
	namespace Core{
		namespace Features{
			namespace Values{
				///набор значений части речи
				static const SS::Core::Features::Types::PartOfSpeechTypesType PartOfSpeechTypes;
				// набор значений служебной части речи
				static const SS::Core::Features::Types::OfficialTypeType OfficialType;
				/// набор значений одушевленность
				static const SS::Core::Features::Types::AnimInAnimTypeType AnimInAnimType;
				/// набор значений уникальность характеристики
				static const SS::Core::Features::Types::UniquenessType Uniqueness;
				///	набор значений выполняемая операция
				static const SS::Core::Features::Types::TDetectWordAnalyzeModeType TDetectWordAnalyzeMode;
				///	набор значений выполняемая операция
				static const SS::Core::Features::Types::OperationModType OperationMod;
				///	набор значений режим определения
				static const SS::Core::Features::Types::ExistenceModType ExistenceMod;
				///	набор значений вид глаголов
				static const SS::Core::Features::Types::AspectTypeType AspectType;
				///	набор значений тип наречия
				static const SS::Core::Features::Types::AdverbTypeType AdverbType;
				///	набор значений падеж
				static const SS::Core::Features::Types::CaseTypeType CaseType;
				///	набор значений исчисляемость
				static const SS::Core::Features::Types::CountTypeType CountType;
				///	набор значений нарицательность
				static const SS::Core::Features::Types::CommonTypeType CommonType;
				///	набор значений степень сравнения
				static const SS::Core::Features::Types::DegreeTypeType DegreeType;
				///	набор значений род
				static const SS::Core::Features::Types::GenderTypeType GenderType;
				///	набор значений число
				static const SS::Core::Features::Types::NumberTypeType NumberType;
				///	набор значений тип числительного
				static const SS::Core::Features::Types::NumericTypeType NumericType;
				///	набор значений характеристика отрицания
				static const SS::Core::Features::Types::NegativeTypeType NegativeType;
				///	набор значений фазисность
				static const SS::Core::Features::Types::PhaseVerbTypeType PhaseVerbType;
				///	набор значений лицо
				static const SS::Core::Features::Types::PersonTypeType PersonType;
				///	набор значений группа местоимений
				static const SS::Core::Features::Types::PronounGroupType PronounGroup;
				///	набор значений класс местоимений
				static const SS::Core::Features::Types::PronounClassType PronounClass;
				///	набор значений краткость
				static const SS::Core::Features::Types::ShortTypeType ShortType;
				///	набор значений класс наречий
				static const SS::Core::Features::Types::TAdverbClassType TAdverbClass;
				///	набор значений тип союза
				static const SS::Core::Features::Types::TConjunctionTypeType TConjunctionType;
				///	набор значений тип определителя
				static const SS::Core::Features::Types::TDeterminerTypeType TDeterminerType;
				///	набор значений время
				static const SS::Core::Features::Types::TimeTypeType TimeType;
				///	набор значений переходность
				static const SS::Core::Features::Types::TransitionalTypeType TransitionalType;
				///	набор значений залог
				static const SS::Core::Features::Types::VoiceTypeType VoiceType;
				///	набор значений форма глагола
				static const SS::Core::Features::Types::VerbFormType VerbForm;
				///	набор значений тип глагола
				static const SS::Core::Features::Types::VerbTypeType VerbType;
				///	набор значений наклонение
				static const SS::Core::Features::Types::MoodTypeType MoodType;

			//НАБОРЫ СЕМАНТИКИ
				/// семантические типы
				static const SS::Core::Features::Types::TSemanticTypeType TSemanticType;
				/// семантический тип времени
				static const SS::Core::Features::Types::TSTimeIntervalTypeType TSTimeIntervalType;
				/// тип места
				static const SS::Core::Features::Types::TSPlaceTypeType TSPlaceType;
				/// тип города
				static const SS::Core::Features::Types::TCityType		TCityType;
				/// тип округа
				static const SS::Core::Features::Types::TAreaType		TAreaType;
				/// тип района
				static const SS::Core::Features::Types::TDistrictType	TDistrictType;
				/// тип метро
				static const SS::Core::Features::Types::TMetroType		TMetroType;
				/// тип общее/частное
				static const SS::Core::Features::Types::TGenericType		TGenericType;
				/// тип контактная информация
				static const SS::Core::Features::Types::TSInfoType		TSInfoType;

				static const SS::Core::Features::Types::TSMaterialType		TSMaterialType;
				static const SS::Core::Features::Types::TSTransportType		TSTransportType;
				static const SS::Core::Features::Types::TSBuildingType		TSBuildingType;
				static const SS::Core::Features::Types::TSMFoodType			TSMFoodType;
				static const SS::Core::Features::Types::TSMForFreeTimeType	TSMForFreeTimeType;

				
				/// типы мер
				static const SS::Core::Features::Types::TSMeasureTypeType TSMeasureType;
				/// семантические типы, применимые к людям
				static const SS::Core::Features::Types::TSPersonTypeType TSPersonType;
				/// Типы организаций
				static const SS::Core::Features::Types::TSOrganizationTypeType TSOrganizationType;
				/// Семантические типы глаголов
				static const SS::Core::Features::Types::TSVerbTypeType TSVerbType;
				///
				static const SS::Core::Features::Types::TSVBeTypeType TSVBeType;
				///
				static const SS::Core::Features::Types::TSVAcquirePossessTypeType TSVAcquirePossessType;
				///
				static const SS::Core::Features::Types::TSVAcquireTypeType TSVAcquireType;
				///
				static const SS::Core::Features::Types::TSVEndTypeType TSVEndType;
				///
				static const SS::Core::Features::Types::TSServiceTypesType TSServiceTypes;
			// НАБОРЫ СИНТАКСИСА
				/// глагольные конструкции
				static const SS::Core::Features::Types::VerbConstructionType VerbConstruction;
				/// конструкции существительного
				static const SS::Core::Features::Types::NounConstructionType NounConstruction;
				/// конструкции прилагательного
				static const SS::Core::Features::Types::AdjectiveConstructionType AdjectiveConstruction;
				/// тип последнего объекта
				static const SS::Core::Features::Types::TypeOfEndingObjectType TypeOfEndingObject;
				///
				static const SS::Core::Features::Types::CombiTypeType CombiType;
				/// синтаксические категории (тип связи
				static const SS::Core::Features::Types::SyntaxCategoriesType SyntaxCategories;
				/// направление зависимости
				static const SS::Core::Features::Types::HeadDependentTypeType HeadDependentType;
				/// тип предложения
				static const SS::Core::Features::Types::SentenceTypeType SentenceType;
				/// порядок следования объектов
				static const SS::Core::Features::Types::ObjectsOrderType ObjectsOrder;
				/// положение по отношению к голове
				static const SS::Core::Features::Types::PositionToDetectedObjectType PositionToDetectedObject;
				/// Обязательность связи
				static const SS::Core::Features::Types::ObjectObligationType ObjectObligation;
				/// тип согласования
				static const SS::Core::Features::Types::AgreementTypeType AgreementType;
				/// область действия правила
				static const SS::Core::Features::Types::ScopeOfTheRuleType ScopeOfTheRule;
				/// Позиция в блоке
				static const SS::Core::Features::Types::PositionInBlockType PositionInBlock;
				/// глубина анализа детей
				static const SS::Core::Features::Types::ChildrenAnalysisDepthType ChildrenAnalysisDepth;
				/// Наследование синтаксических характеристик родительского элемента
				static const SS::Core::Features::Types::SyntaxParamsInheritanceModeType SyntaxParamsInheritanceMode;
				/// Создать новый юнит
				static const SS::Core::Features::Types::CreateUnitType CreateUnit;
				/// Тип перемещения юнита
				static const SS::Core::Features::Types::MoveTypeType MoveType;
			}
		}
	}
}