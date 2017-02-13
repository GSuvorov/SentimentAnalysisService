#pragma once

#include "../ASSInterface/IBlackBoxFeature.h"
#include "../ASSInterface/IBlackBox.h"
#include "../ASSInterface/IAMConverter.h"
#include "../ASSInterface/TDictionaryManager.h"

namespace SS
{
	namespace Syntax
	{
		namespace BoardElements
		{
			///Элемент морфологического списка элемента доски
			class CMorphoListElement
			{
			private:
				///Морфологическая фича
				SS::Core::Features::CMorphoFeature m_oMorphoFeature;
				///Фича из BlackBox
				SS::Interface::Core::BlackBox::IFeature* m_pFeature;
				///Индекс из BlackBox
				SS::Interface::Core::BlackBox::IIndex* m_pIndex;
				///Морфо Конвертер
				SS::Interface::Core::AMConverter::IAMConverterMorpho* m_pAMConverterMorpho;
			
			private:
				///Преобразование CMorphoFeature в IFeature 
				void UpdateIFeature();
			
			public:
				///Конструктор
				CMorphoListElement() : m_pFeature(NULL), m_pIndex(NULL), m_pAMConverterMorpho(NULL) {};
				///Деструктор
				~CMorphoListElement(){};

				///Инициализация элемента морфоллогии
				void Init( SS::Interface::Core::AMConverter::IAMConverterMorpho* converter,
						   SS::Interface::Core::BlackBox::IFeature* feature, 
						   SS::Interface::Core::BlackBox::IIndex* index, 
						   int unit_num = 0, bool isPerson = false);

				///Возвращает MorphoFeature
				SS::Core::Features::CMorphoFeature & GetMorphoFeature()
				{
					return m_oMorphoFeature;
				}
				///Возвращает часть речи
				SS::Core::Features::Types::PartOfSpeechTypes GetPOST() const;
				///Возвращает форму глагола
				SS::Core::Features::Types::VerbForm GetVerbForm() const;
				///Возвращает переходность глагола
				SS::Core::Features::Types::TransitionalType GetTransType() const;
				///Устанавливает MorphoFeature
				void SetMorphoFeature(SS::Core::Features::CMorphoFeature* feature)
				{
					m_oMorphoFeature.AssignMorphoFeature(feature);
					UpdateIFeature();
				}
				///Добавить MorphoFeature
				void AddMorphoFeature(SS::Core::Features::CMorphoFeature* pMorphoFeature)
				{
					m_oMorphoFeature.AssignValidMorphoFeature(pMorphoFeature);
					UpdateIFeature();
				}
				///Возвращает индекс основы
				unsigned int GetIDSource() const 
				{
					SS::Dictionary::Types::TDictionaryIndex oIndexVal;
					m_pIndex->GetDictionaryIndex()->GetFirst(&oIndexVal);
					
					return oIndexVal.GetDictionaryIndexWithoutFlags();
				}
				///Удаление фичи
				void ReleaseFeature()
				{
					m_pFeature->ReleaseFeature();
				}
				///Получение указателя на IIndex
				SS::Interface::Core::BlackBox::IIndex* GetIndex();
				///operator==
				//bool operator==(const CMorphoListElement & rMorphoElement);
			};
		}
	}
}





