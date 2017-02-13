#pragma once

#include ".\basefeaturecollection.h"
#include "define.h"
namespace SS
{
	namespace Core
	{
		namespace Features
		{
			/// полна€ информаци€ о последовательности слов в конструкции
			class CObjectFeature:
				public SS::Core::Features::Base::CBaseFeatureCollection
			{
			public:
				CObjectFeature(void);
				~CObjectFeature(void);
				CObjectFeature& operator = (const CObjectFeature& value)
				{
					Assign(&value);
					return *this;
				}
				CObjectFeature(const CObjectFeature& value){
					Init();
					*this = value;
				};

 				///вернет строковое им€ характеристики
				const char* GetFeatureName() const {return "ConstructionFeature";};
				///вернет строковое описание характеристики
				const char* GetFeatureDescription() const {return "полна€ информаци€ о последовательности слов в конструкции";};

				/// пор€док объектов конструкции
				Types::ObjectsOrder				m_ObjectsOrder;			// подр€д или нет
				/// положение по отношению к голове
				Types::PositionToDetectedObject	m_PositionToDetectedObject;	// слева или справа
				/// ќб€зательность св€зи
				Types::ObjectObligation			m_ObjectObligation;		// об€зательна€ или необ€зательна€
				/// число объектов одного типа
				Types::SameTypeObjectsNumber	m_SameTypeObjectsNumber;
				/// границы действи€
				Types::ScopeOfTheRule			m_ScopeOfTheRule;		// до конца предложени€ или до конца блока
				/// положение объекта в блоке
				Types::PositionInBlock			m_PositionInBlock;		// положение в блоке
				/// режим согласовани€
				Types::AgreementType			m_AgreementType;		// по роду, числу, падежу или числу, лицу				
				/// глубина поиска зависимых элементов
				Types::ChildrenAnalysisDepth	m_ChildrenAnalysisDepth;
				/// режим наследовани€ синтакисческих характеристик
				Types::SyntaxParamsInheritanceMode m_SyntaxParamsInheritanceMode;
				/// Ќомер предложени€
				Types::SentenceNumber m_SentenceNumber;
				/// ѕор€дковый номер блока в предложении
				Types::BlockPosition m_BlockPosition;
				///  оличество блоков дл€ просмотра от детект-блока
				Types::BlockSearchScope m_BlockSearchScope;
				/// —оздать новый юнит
				Types::CreateUnit m_CreateUnit;
				/// “ип перемещени€ юнита
				Types::MoveType m_MoveType;

			private:
				void Init();
			};
		}
	}
}