//FeatureController.h
#pragma once
#include "../ASSInterface/Construction.h"
#include "../ASSInterface/IDictionaryManager.h"
#include "./BoardElement.h"

namespace SS
{
namespace Syntax
{
namespace ModifiedConstruction
{
	using namespace SS::SyntaxConstructions::Types;
	using namespace SS::Syntax::BoardElements;
		
	///Интерфейсный класс, базовый для CFeatureController
	class IFeatureController
	{
	public:
		///Проверка соответствия характеристик элемента доски априорным характеристикам элемента конструкции
		virtual bool IsAppropriate(CBoardElement* pElem) = 0;
		///Вернуть объект конструкции
		virtual CConstructionObject* GetConstructionObject() = 0;
		///Установить диапазон для проверки position
		virtual void InitRange(BoardElementsIterator* pFirstIt, BoardElementsIterator* pLastIt) = 0;
	};
	
	///Класс для проверки применимости Объекта конструкции к Элементу доски
	template<typename TForm, typename TPosition>
	class CFeatureController : public IFeatureController
	{
	private:
		///Объект Form
		TForm m_oForm;
		///Объект конструкции соответствующий данному контроллеру
		CConstructionObject* m_pConObj;
		///Первый элемент диапазона поиска
		BoardElementsIterator* m_pFirstIt;
		///ЗаПоследним элемент диапазона поиска
		BoardElementsIterator* m_pLastIt;
		///База конструкций
		ExpertModel::ESyntaxBase m_eBase;

	private:
		///Проверка морфологических характеристик
		bool IsAppropriateMorpho(CBoardElement* pElem);
		///Проверка синтаксических характеристик
		bool IsAppropriateSyntax(CBoardElement* pElem);
		///Проверка семантических характеристик
		bool IsAppropriateSemantic(CBoardElement* pElem);

	public:
		///Конструктор
		CFeatureController( SS::SyntaxConstructions::Types::CConstructionObject* pConObj,
							SS::Interface::Core::Dictionary::IDictionary* pDictionary,
							ExpertModel::ESyntaxBase eBase);
		///Деструктор
		~CFeatureController(){};
		///Проверка соответствия характеристик элемента доски априорным характеристикам элемента конструкции
		bool IsAppropriate(CBoardElement* pElem);
		///Вернуть объект конструкции
		CConstructionObject* GetConstructionObject() { return m_pConObj; };
		///Установить диапазон для проверки position
		void InitRange(BoardElementsIterator* pFirstIt, BoardElementsIterator* pLastIt) { m_pFirstIt = pFirstIt; m_pLastIt = pLastIt; };
	};

	///Конструктор
	template<typename TForm, typename TPosition>
	CFeatureController<TForm, TPosition>::CFeatureController( SS::SyntaxConstructions::Types::CConstructionObject* pConObj,
															  SS::Interface::Core::Dictionary::IDictionary* pDictionary,
															  ExpertModel::ESyntaxBase eBase )
	:
	m_pConObj(pConObj),
	m_pFirstIt(NULL),
	m_pLastIt(NULL),
	m_eBase(eBase)
	{
		SS_TRY
		{
			m_oForm.InitData(pConObj, pDictionary);
		}
		SS_CATCH(L"")
	}

	///Проверка соответствия характеристик элемента доски априорным характеристикам элемента конструкции
	template<typename TForm, typename TPosition>
	bool CFeatureController<TForm, TPosition>::IsAppropriate(CBoardElement* pElem)
	{
		if ( !TPosition::IsAppropriatePosition(pElem, *m_pFirstIt, *m_pLastIt) )
		{ return false; }

		return ( IsAppropriateMorpho(pElem) &&
				 IsAppropriateSyntax(pElem) &&
				 IsAppropriateSemantic(pElem) );
	}

	///Проверка морфологических характеристик
	template<typename TForm, typename TPosition>
	bool CFeatureController<TForm, TPosition>::IsAppropriateMorpho(CBoardElement* pElem)
	{
		SS_TRY
		{
			if ( !m_oForm.IsEqualWord(pElem) )
			{ return false; }

			for (PMORPHO pMorpho = pElem->GetFirstMorpho(); pMorpho; pMorpho = pElem->GetNextMorpho())
			{
				if ( m_pConObj->m_MorphoFeatureApriori.IsIntersectsWith(pMorpho->GetMorphoFeature(), pElem->GetLang()) )
				{ return true; }
			}
		}
		SS_CATCH(L"")
		return false;
	}

	///Проверка синтаксических характеристик
	template<typename TForm, typename TPosition>
	bool CFeatureController<TForm, TPosition>::IsAppropriateSyntax(CBoardElement* pElem)
	{
		return m_pConObj->m_SyntaxFeatureApriori.IsIntersectsOrEmpty(&pElem->GetSyntaxFeatureApriori());
	}

	///Проверка семантических характеристик
	template<typename TForm, typename TPosition>
	bool CFeatureController<TForm, TPosition>::IsAppropriateSemantic(CBoardElement* pElem)
	{
		if (m_eBase == ExpertModel::sbRussianNQ)
		{ return m_pConObj->m_SemanticFeatureApriori.IsIntersectsOrEmptyDeep(pElem->GetSemanticFeature()); }
		return m_pConObj->m_SemanticFeatureApriori.IsIntersectsOrEmpty(pElem->GetSemanticFeature());
	}

}
}
}