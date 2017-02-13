#pragma once

#include "BoardCommonTypes.h"
#include "MorphoListElement.h"
#include "BlockVersion.h"
#include "MorphoFormsFilter.h"
#include "../ASSInterface/Construction.h"
#include "../ASSInterface/IAMConverter.h"
#include "../ASSInterface/IBlackBoxUnit.h"
#include "../ASSInterface/TMainAnalyseTypes.h"
#include <list>
#include "Container.h"

namespace SS
{
	namespace Syntax
	{
		namespace BoardElements
		{
			///Элемент доски. Служит для хранения информации об обрабатываемом слове
			class CBoardElement
			{
				///Структура для хранения АТ и его номера
				struct SLayer
				{
					///Порядковый номер применившейся конструкции с АТ
					unsigned int iNumber;
					///Тип согласования
					SS::Core::Features::Types::AgreementType oAgreementType;
					///Конструктор
					SLayer() : iNumber(0) { oAgreementType.Undefine(); };
				};
			
			private:
				///Список морфоформ, которые может принять элемент
				CContainer<MORPHO, SIZEMORPHO> m_oMorphoContainer;
				///Список адресов морфофич, которые сработали по AgrimentType
				std::list<PMORPHO> m_oMorphoContainerAT;
				///Флаг, устанавливаемый, если элемент входит в конструкцию, которая разбила предложение на блоки.
				bool m_beatsBlocks;
				///Флаг, указывающий, стоит ли элемент в дереве на верхнем уровне.
				bool m_isHead, m_isHeadDynamic;				
				///Флаг, устанавливаемый, если надо использовать элемент гипотезы вместо элемента доски
				bool m_useVersion;
				///Для создания дополнительных IFeature (для русского языка)
				SS::Interface::Core::BlackBox::IBlackBoxTextStorage *m_pBlackBoxTextStorage;
				///Морфологический конвертор
				SS::Interface::Core::AMConverter::IAMConverterMorpho *m_pAMConverterMorpho;
				///Семантический конвертор
				SS::Interface::Core::AMConverter::IAMConverterSemantic *m_pAMConverterSemantic; 
				///Апостериорная синтаксическая информация.
				SS::Core::Features::CSyntaxFeatureAposteriori m_syntaxFeatureAposteriori;
				///Априорная синтаксическая информация.
				SS::Core::Features::CSyntaxFeatureApriori m_syntaxFeatureApriori;
				///Сервис для обработки семантики
				CSemanticService m_oSemanticService;
				///Юнит, которому соответствует элемент доски
				SS::Interface::Core::BlackBox::IUnit *m_pUnit;
				///Слово
				std::wstring m_word;
				///Идентификатор группы экспертов, эксперт которой последним менял синтаксическую информацию элемента.
				int m_iIdGroup, m_iIdGroupDynamic;
				///Указывает на элемент гипотезы, соответствующий данному элементу
				PVERELEMENT m_pVersionElement;
				///Язык предложения
				SS::Core::Types::EAnalyseLanguage m_eLang;
				///Позиция по умолчанию
				unsigned int m_uiDefPosition;
				///Работа с морфологическими характеристиками
				CMorphoFormsFilter m_oMorphoFormsFilter;
				///Хранилище слоев примененных конструкций с АТ
				std::vector<SLayer> m_vATLayers;
                /// Флаг, указывающий на завершение обработки элемента доски
                bool m_IsDone;
				
			private:
				///Устанавливает m_word, одновременно преобразуя его в нижний регистр
				void SetWord(const std::wstring &wsWord);
				///Очистка контейнера морфологии и других данных
				void ClearData();
				///Размножение морфологии (для русского языка)
				void DivisionMorpho();
				///Удалить морфологию не сработавшую по АТ
				void RemoveMorphoButAT();
			
			public:
				///Однородные с данным члены предложения ( которым нвдо ставить те же синтакические характеристики )
				std::list<BoardElements::BoardElementsIterator> m_childUniformMembers;
			
			public:
				///Конструктор
				CBoardElement(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
				//Деструктор
				~CBoardElement();
				//Метод для инициализации класса очередным юнитом
				bool Init( SS::Interface::Core::BlackBox::IUnit* unit,
						   SS::Core::Types::EAnalyseLanguage eLang,
						   unsigned int uiPosition );
				///Возвращает элемент гипотезы данного элемента (доб Павленко)
				const PVERELEMENT GetVerElem() const { return m_pVersionElement; };
				///Возвращает язык элемента (доб Павленко)
				SS::Core::Types::EAnalyseLanguage GetLang() const;
				///Возвращает указатель на первый элемент контейнера морфологии
				CMorphoListElement* GetFirstMorpho()
				{
					if (!m_useVersion)
					{ return m_oMorphoContainer.GetFirst();	}
					else
					{ return m_pVersionElement->m_pMorpho; }
				}
				///Возвращает указатель на следующий элемент в контейнере морфологии
				CMorphoListElement* GetNextMorpho()
				{
					if (!m_useVersion)
					{ return m_oMorphoContainer.GetNext(); }
					else
					{ return NULL; }
				}
				///Возвращает указатель на первый элемент контейнера морфологии(для динамического создания гипотез)
				PMORPHO GetFirstMorphoDynamic();
				///Возвращает указатель на следующий элемент в контейнере морфологии(для динамического создания гипотез)
				PMORPHO GetNextMorphoDynamic();
				///Возвращает указатель на следующий после переданого элемент в контейнере морфологии
				PMORPHO GetNextMorphoForAddress(PMORPHO pMorpho);
				///Возвращает слово, соответствующее данному элементу.
				std::wstring & GetWord() { return m_word; };
				///Возвращает индекс родительского элемента
				int GetParentIndex() const
				{
					if (m_useVersion)
					{ return m_pVersionElement->m_syntaxFeatureAposteriori.m_IdParent.GetValue(); }
					else
					{ return m_syntaxFeatureAposteriori.m_IdParent.GetValue(); }
				}
				///Устанавливает элементу доски апостериорные характеристики объекта конструкции
				void ApplyConstruction( SS::SyntaxConstructions::Types::CConstructionObject* pObject, 
									    int parentIndex, int ownIndex, int iIdGroup, unsigned int iCountApplyedExpertAT, 
										CBoardElement* pElemBaseSemantic );
				///Проставляет юниту синтаксическую категорию
				void SetFeatureToUnit();
				///Возвращает флаг, показывающий, является ли данный элемент границей блока
				bool BeatsBlocks() const { return m_beatsBlocks; };
				///Ставит флаг, показывающий, является ли данный элемент границей блока
				void SetBeatsBlocks() {	m_beatsBlocks = true; };
				///Возвращает апостериорную синтаксическую информацию элемента
				SS::Core::Features::CSyntaxFeatureAposteriori & GetSyntaxFeature()
				{
					if (m_useVersion)
					{ return m_pVersionElement->m_syntaxFeatureAposteriori;	}
					else
					{ return m_syntaxFeatureAposteriori; }
				}
				SS::Core::Features::CSyntaxFeatureAposteriori & GetSyntaxFeature1()
				{
					//if (m_useVersion)
					//{ return m_pVersionElement->m_syntaxFeatureAposteriori;	}
					//else
					{ return m_syntaxFeatureAposteriori; }
				}
				///Возвращает апостериорную синтаксическую информацию элемента (для динамического создания гипотез)
				SS::Core::Features::CSyntaxFeatureAposteriori & GetSyntaxFeatureDynamic() { return m_syntaxFeatureAposteriori; }
				///Возвращает флаг, показывающий, является ли элемент головным
				bool IsHead() const	{ return m_isHead; };
				///Установка элемента головным
				void SetHead(bool bHead) { m_isHead = bHead; };
                ///Возвращает флаг, указывающий на завершение обработки элемента доски
                bool IsDone() { return m_IsDone; }
                /// Установка завершения обработки элемента доски
                void SetDone(bool bDone) { m_IsDone = bDone; }
				///Возвращает флаг, показывающий, является ли элемент головным ( используется при динамическом создании гипотез )
				bool IsHeadDynamic() const { return m_isHeadDynamic; };
				///Возвращает название группы, установившей элементу синтаксические характеристики
				int GetGroup() { return m_iIdGroup;	};
				///Возвращает название группы, установившей элементу синтаксические характеристики ( используется при динамическом создании гипотез )
				int GetGroupDynamic() { return m_iIdGroupDynamic; };
				///Устанавливает текущую версию
				void SetVersion(PVERELEMENT pVersion)
				{
					m_pVersionElement = pVersion;
					m_iIdGroup = pVersion->m_iIdGroup;
					m_isHead = pVersion->m_isHead;
					m_useVersion = true;
				}
				///Указывает, произошло ли разбиение на гипотезы или нет
				bool UseVersions() const { return m_useVersion;	};
				///Указывает, является ли текущая версия неиспользуемои морфоформой
				bool IsUnusedForm() const
				{ return ( m_useVersion && (m_pVersionElement->m_morphoFeatureApriori.m_PartOfSpeechTypes.IsUndefined()) );	};
				///Возвращает априорную синтаксическую информацию
				SS::Core::Features::CSyntaxFeatureApriori & GetSyntaxFeatureApriori()
				{
					if (m_useVersion)
					{ return m_pVersionElement->m_syntaxFeatureApriori;	}
					else
					{ return m_syntaxFeatureApriori; }
				}
				///Возвращает априорную синтаксическую информацию (для динамического создания гипотез)
				SS::Core::Features::CSyntaxFeatureApriori & GetSyntaxFeatureAprioriDynamic()	{ return m_syntaxFeatureApriori; };
				///Возвращает семантический тип
				SS::Core::Features::Types::TSemanticType & GetSemanticType() { return GetSemanticFeature().m_SemanticType; };
				///Возвращает семантические характеристики
				SS::Core::Features::CSemanticFeature & GetSemanticFeature();
				///Возвращает указатель на юнит, соответствующий элементу доски
				SS::Interface::Core::BlackBox::IUnit* GetUnit() const { return m_pUnit; };
				///Сброс всех значений элемента
				void Reset();
				///Удаляет всю морфологию кроме той которая в гипотезе (используется только после разбора)
				void RemoveUselessMorpho();
				///Получить собственную позицию
				unsigned int GetPosition() const { return m_uiDefPosition; };
				///Добавление морфо-фичи совпадающей по AgrimentType-у
				void AddMorphoAT(CMorphoListElement* pMorpho);
				///Получить контейнер морфологии сработавшей по АТ
				std::list<PMORPHO> & GetMorphoContainerAT() { return m_oMorphoContainerAT; };
				///Есть ли АТ по данному номеру
				bool IsAT(unsigned int iNumber) const;
				///Получить АТ по номеру сработавшей конструкции
				SS::Core::Features::Types::AgreementType GetAT(unsigned int iNumber) const;
				///Установит семантическую фичу в IText
				void RemoveUselessSemantic();
				///Получить семантический сервис
				const CSemanticService & GetSemanticService(){ return m_oSemanticService; };

				bool IsSemantic(SS::Interface::Core::BlackBox::IUnit* pUnit, SS::Core::Features::Types::TSemanticType semType);

				bool IsPersonApostrof(SS::Interface::Core::BlackBox::IUnit* pUnit);				
				
			};

		}
	}
}
