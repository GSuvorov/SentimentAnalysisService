//UnitSearchPolicys.h
#pragma once
#include ".\BoardElement.h"

namespace SS
{
namespace Syntax
{
namespace Policys
{
	using namespace SS::Syntax::BoardElements;
	///Базовый класс для стратегий вправо и влево
	class CBaseSide
	{
	protected:
		///Первый элемент диапазона
		BoardElementsIterator* m_pFirstIt;
		///Элемент за последним
		BoardElementsIterator* m_pLastIt;
		///Текущий элемент
		mutable BoardElementsIterator m_itCurrent;
		///Коллекция для инициализации итераторов
		mutable BoardCollection m_NullCollection;

		mutable bool isNull;

	protected:
		///Конструктор
		CBaseSide() : m_pFirstIt(NULL), m_pLastIt(NULL)
		{
			//m_itCurrent = m_NullCollection.end();
			isNull = true;
		}

	public:
		///Инициализация диапазона
		void Init(BoardElementsIterator* pFirstIt, BoardElementsIterator* pLastIt)
		{
			m_pFirstIt = pFirstIt;
			m_pLastIt = pLastIt;
			//isNull = true;
		}
	};
	
	//*************************************СТРАТЕГИИ ПЕРЕБОРА ДИАПАЗОНА****************************************//
	
	///Класс-стратегия обхода диапазона слева направо
	class CRightPolicy : public CBaseSide
	{
	public:
		///Возвращает первый элемент диапазона
		CBoardElement* First() const
		{			
			CBoardElement* pReturn = NULL;
			if ( (*m_pFirstIt) == (*m_pLastIt) )
			{ return NULL; }
			try
			{
				m_itCurrent = (*m_pFirstIt);
				pReturn = (*m_itCurrent);
				isNull = false;
			}
			catch (...)
			{
				SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"Не выполнена инициализация диапазона.");
				return NULL;
			}			
			return pReturn;
		}
		///Возвращает следующий элемент диапазона
		CBoardElement* Next() const
		{
			if (/*m_itCurrent == m_NullCollection.end()*/isNull) 
			{
				SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"Не вызван first()."); 
				return NULL;
			}
			if (++m_itCurrent == (*m_pLastIt))
			{ 
				//m_itCurrent = m_NullCollection.end();	
				isNull=true;
				return NULL;
			}
			return (*m_itCurrent);
		}
	};

	///Класс-стратегия обхода диапазона справа налево
	class CLeftPolicy : public CBaseSide
	{
	public:
		///Возвращает первый элемент диапазона
		CBoardElement* First() const
		{
			CBoardElement* pReturn = NULL;
			try
			{
				m_itCurrent = (*m_pLastIt);
				pReturn = (*m_itCurrent);
				isNull = false;
			}
			catch (...)
			{
				SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"Не выполнена инициализация диапазона."); 
				return NULL;
			}
			if ( (*m_pFirstIt) == (*m_pLastIt) )
			{ return NULL; }
			--m_itCurrent;
			pReturn = (*m_itCurrent);
			return pReturn;
		}
		///Возвращает следующий элемент диапазона
		CBoardElement* Next() const
		{
			if (isNull) 
			{
				SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"Не вызван first()."); 
				return NULL;
			}	
			if (m_itCurrent == (*m_pFirstIt))
			{
				//m_itCurrent = m_NullCollection.end();	
				isNull = true;
				return NULL;
			}
			--m_itCurrent;
			return (*m_itCurrent);
		}
	};

	//*************************************СТРАТЕГИИ ПРЕРЫВАНИЯ ПОИСКА*****************************************//
	
	///Класс-стратегия если нужно прервать поиск элемента
	class CBreakPolicy
	{
	private:
		///Запятая
		std::wstring m_sComma;

	public:
		///Конструктор
		CBreakPolicy() : m_sComma(L","){};
		///Проверка на прерывание
		bool Break(CBoardElement* pElem) const
		{
			if ( pElem->GetWord() == m_sComma )
			{ return true; }
			return false;
		}
	};

	///Класс-стратегия если не нужно прерывать поиск элемента
	class CNotBreakPolicy
	{
	public:
		///Проверка на прерывание
		bool Break(CBoardElement* pElem) const
		{
			return false;
		}
	};

	//*************************************СТРАТЕГИИ ПРОВЕРКИ ЭЛЕМЕНТОВ*******************************************//
	
	///Класс-стратегия на пропуск только головных элементов
	class COnlyHeadPolicy
	{
	public:
		///Для стратегии Child (пустой метод)
		void SetIdParent(unsigned int IdParent){};
		///Проверить элемент
		bool Check(CBoardElement* pElem) const
		{
			if ( pElem->IsHead() ) 
			{ return true; }
			return false;
		}
	};

	///Класс-стратегия на пропуск любых элементов
	class CFullPolicy
	{
	public:
		///Для стратегии Child (пустой метод)
		void SetIdParent(unsigned int IdParent){};
		///Проверить элемент
		bool Check(CBoardElement* pElem) const
		{
			return true;
		}
	};

	///Класс-стратегия на пропуск головных или опущеных текущей группой
	class CHeadAndCurrentGroupChildrenPolicy
	{
	private:
		///Идентификатор группы
		unsigned int m_IdGroup;

	public:
		///Конструктор
		CHeadAndCurrentGroupChildrenPolicy(unsigned int IdGroup) : m_IdGroup(IdGroup){};
		///Для стратегии Child (пустой метод)
		void SetIdParent(unsigned int IdParent){};
		///Проверить элемент
		bool Check(CBoardElement* pElem) const
		{
			if ( (pElem->IsHead() || pElem->GetGroup() == m_IdGroup) && (!pElem->IsUnusedForm()) )
			{ return true; }
			return false;
		}
	};

	///Класс-стратегия на пропуск child-элементов последнего сработавшего
	class CChildPolicy
	{
	private:
		///Индекс родителя
		int m_IdParent;
		///Первый элемент предложения
		BoardElementsIterator & m_itBeginBoard;		
	
	public:
		///Конструктор
		CChildPolicy(BoardElementsIterator & itBeginBoard) : m_itBeginBoard(itBeginBoard), m_IdParent(-1){};
		///Расчитать IdParent предыдущего сработавшего
		void SetIdParent(unsigned int IdParent)
		{
			m_IdParent = IdParent;
		}
		///Проверить элемент
		bool Check(CBoardElement* pElem) const
		{
			SS_TRY
			{
				if (pElem->IsHead() || m_IdParent < 0)
				{ return false; }

				int iMaxTreeDepth = 10, iTreeDepth = 0;
				
				for(BoardElementsIterator itParent = m_itBeginBoard + pElem->GetParentIndex(); ((*itParent) != pElem); 
					itParent = m_itBeginBoard + pElem->GetParentIndex())
				{
					if ( pElem->GetParentIndex() == m_IdParent )
					{ return true; }
					pElem = (*itParent);
					
					if(iMaxTreeDepth < ++iTreeDepth)
					{ return false;	}					
				}
			}
			SS_CATCH(L"")
			return false;
			
		}
	};

	//*************************************СТРАТЕГИИ Cont/Discont*******************************************//

	///Класс-стратегия для проверки только первого элемента диапазона
	class CContinuousPolicy
	{
	public:
		///Проверить элемент
		static bool Continue(CBoardElement* pElem)
		{
			return false;
		}
	};
	
	///Класс-стратегия для проверки всех элементов диапазона
	class CDiscontinuousPolicy
	{
	public:
		///Проверить элемент
		static bool Continue(CBoardElement* pElem)
		{
			return true;
		}
	};

	///Класс-стратегия для проверки только первого элемента диапазона, не считая наречия
	class CContinuousNotNeverAdverbPolicy
	{
	public:
		///Проверить элемент
		static bool Continue(CBoardElement* pElem)
		{
			SS_TRY
			{
				for (PMORPHO pMorpho = pElem->GetFirstMorpho(); pMorpho; pMorpho = pElem->GetNextMorpho())
				{
					if ( pMorpho->GetMorphoFeature().m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postAdverb) )
					{ return true; }
				}
			}
			SS_CATCH(L"")
			return false;
		}
	};

	
	//*************************************СТРАТЕГИИ ДЛЯ КЛАССА CFeatureController*******************************************//

	///Класс-стратегия для значения mfamAllForms
	class CAllFormsPolicy
	{
	private:
		///Индексы для AllForms
		std::set<unsigned int> m_DictionaryIndexes;
		///Список необходимых детект-слов
		std::set<std::wstring> m_DetectWords;

	public:
		///Инициализировать данные
		void InitData( SS::SyntaxConstructions::Types::CConstructionObject* pConObj,
					   SS::Interface::Core::Dictionary::IDictionary* pDictionary )
		{
			SS_TRY
			{
				typedef std::list<SS::Dictionary::Types::TWordInfo>::iterator TWordInfoIt;
				std::list<SS::Dictionary::Types::TWordInfo> lWordInfo;
				std::list<SS::Dictionary::Types::TWordInfo> lWordInfoDetect;

				//Перебор детект-слов объекта
				for ( SS::SyntaxConstructions::Types::TDetectWordsGroup::iterator itWord = pConObj->GetDetectWords().begin();
					  itWord != pConObj->GetDetectWords().end(); ++itWord )
				{
					pDictionary->GetMorphologyAnalyser()->GetWordInfo( itWord->c_str(), &(lWordInfo),
																	   SS::Dictionary::Types::efzOnlySearch );
					for (TWordInfoIt itWordInfo = lWordInfo.begin(); itWordInfo != lWordInfo.end(); ++itWordInfo)
					{
						lWordInfoDetect.clear();
						pDictionary->GetMorphologyAnalyser()->GetWordFormsByIDSource( &(itWordInfo->front().m_WordIndex.m_DictIndex), 
																					  &(lWordInfoDetect) );
						for (TWordInfoIt itWordInfoDet = lWordInfoDetect.begin(); itWordInfoDet != lWordInfoDetect.end(); ++itWordInfoDet)
						{ m_DetectWords.insert(itWordInfoDet->front().m_wWord); }

						//Заполнение индексами
						m_DictionaryIndexes.insert(itWordInfo->front().m_WordIndex.m_DictIndex.GetDictionaryIndexWithoutFlags());
					}
					lWordInfo.clear();
				}
			}
			SS_CATCH(L"")
		}

		///Проверка детект-слова
		bool IsEqualWord(CBoardElement* pElem)
		{
			for (PMORPHO pMorpho = pElem->GetFirstMorpho(); pMorpho; pMorpho = pElem->GetNextMorpho())
			{
				if ( m_DictionaryIndexes.find(pMorpho->GetIDSource()) != m_DictionaryIndexes.end() )
				{ return true; }
			}
			return false;
		}
	};

	///Класс-стратегия для значения mfamLetForm
	class CLetFormPolicy
	{
	private:
		///Список необходимых детект-слов
		std::set<std::wstring> m_DetectWords;

	public:
		///Инициализировать данные
		void InitData( SS::SyntaxConstructions::Types::CConstructionObject* pConObj,
					   SS::Interface::Core::Dictionary::IDictionary* pDictionary )
		{
			SS_TRY
			{
				for ( SS::SyntaxConstructions::Types::TDetectWordsGroup::iterator itWord = pConObj->GetDetectWords().begin();
					  itWord != pConObj->GetDetectWords().end(); ++itWord )
				{
					m_DetectWords.insert(*itWord);
				}
			}
			SS_CATCH(L"")
		}
		
		///Проверка детект-слова
		bool IsEqualWord(CBoardElement* pElem)
		{
			if ( m_DetectWords.find(pElem->GetWord()) != m_DetectWords.end() ) 
			{ return true;	}
			return false;
		}
	};

	///Класс-стратегия для значения mfamUndefined
	class CUndefinedFormPolicy
	{
	public:
		///Инициализировать данные
		void InitData( SS::SyntaxConstructions::Types::CConstructionObject* pConObj,
					   SS::Interface::Core::Dictionary::IDictionary* pDictionary )
		{
		//Метод ничего не делает
		}

		///Проверка детект-слова
		bool IsEqualWord(CBoardElement* pElem)
		{
			return true;
		}

	};

	//***********************************position************************************
		
	///Класс-стратегия для значения pibBegin
	class CBeginPositionPolicy
	{
	public:
		///Проверка по позиции элемента в блоке
		static bool IsAppropriatePosition(CBoardElement* pElem, BoardElementsIterator itF, BoardElementsIterator itL)
		{
			SS_TRY
			{
				BoardElementsIterator it = itF;
				for (it; (*it) != pElem; ++it)
				{
					if ( (*it)->IsHead() && (!(*it)->BeatsBlocks()) )
					{ return false; }
				}
			}
			SS_CATCH(L"")
			return true;
		}
	};

	///Класс-стратегия для значения pibEnd
	class CEndPositionPolicy
	{
	public:
		///Проверка по позиции элемента в блоке
		static bool IsAppropriatePosition(CBoardElement* pElem, BoardElementsIterator itF, BoardElementsIterator itL)
		{
			SS_TRY
			{
				BoardElementsIterator it = itL;
				--it;
				for (it; (*it) != pElem; --it)
				{
					if ( (*it)->IsHead() )
					{ return false; }
				}
			}
			SS_CATCH(L"")
			return true;
		}
	};

	///Класс-стратегия для значения pibMiddle и правого диапазона
	class CMiddlePositionPolicy
	{
	public:
		///Проверка по позиции элемента в блоке
		static bool IsAppropriatePosition(CBoardElement* pElem, BoardElementsIterator itF, BoardElementsIterator itL)
		{
			return ( !CBeginPositionPolicy::IsAppropriatePosition(pElem, itF, itL) &&
					 !CEndPositionPolicy::IsAppropriatePosition(pElem, itF, itL)		);
		}
	};

	///Класс-стратегия для значения pibUndefined
	class CUndefinedPositionPolicy
	{
	public:
		///Проверка по позиции элемента в блоке
		static bool IsAppropriatePosition(CBoardElement* pElem, BoardElementsIterator itF, BoardElementsIterator itL)
		{
			return true;
		}
	};

	//*************************************СТРАТЕГИИ ДЛЯ AgreementType*******************************************//

	///Интерфейс для классов стратегий АТ
	class IBaseAT
	{
	public:
		///Проверка применимости
		virtual bool IsIntersect( SS::Core::Features::CMorphoFeature & oMorphoOne,
								  SS::Core::Features::CMorphoFeature & oMorphoTwo ) = 0;
	};
	
	///Класс-стратегия для значения atC
	class CPolicyC : public IBaseAT
	{
	public:
		///Проверка применимости
		bool IsIntersect( SS::Core::Features::CMorphoFeature & oMorphoOne,
						  SS::Core::Features::CMorphoFeature & oMorphoTwo )
		{
			return ( oMorphoOne.m_CaseType.IsIntersectsWith(&(oMorphoTwo.m_CaseType)) );
		}
	};

	///Класс-стратегия для значения atGN
	class CPolicyGN : public IBaseAT
	{
	public:
		///Проверка применимости
		bool IsIntersect( SS::Core::Features::CMorphoFeature & oMorphoOne,
						  SS::Core::Features::CMorphoFeature & oMorphoTwo )
		{
			return ( oMorphoOne.m_GenderType.IsIntersectsWith(&(oMorphoTwo.m_GenderType)) &&
					 oMorphoOne.m_NumberType.IsIntersectsWith(&(oMorphoTwo.m_NumberType)) );
		}
	};

	///Класс-стратегия для значения atGNC
	class CPolicyGNC : public IBaseAT
	{
	public:
		///Проверка применимости
		bool IsIntersect( SS::Core::Features::CMorphoFeature & oMorphoOne,
						  SS::Core::Features::CMorphoFeature & oMorphoTwo )
		{
			return ( oMorphoOne.m_GenderType.IsIntersectsWith(&(oMorphoTwo.m_GenderType)) &&
					 oMorphoOne.m_NumberType.IsIntersectsWith(&(oMorphoTwo.m_NumberType)) &&
					 oMorphoOne.m_CaseType.IsIntersectsWith(&(oMorphoTwo.m_CaseType)) );
		}
	};

	///Класс-стратегия для значения atGPN
	class CPolicyGPN : public IBaseAT
	{
	public:
		///Проверка применимости
		bool IsIntersect( SS::Core::Features::CMorphoFeature & oMorphoOne,
						  SS::Core::Features::CMorphoFeature & oMorphoTwo )
		{
			return ( oMorphoOne.m_GenderType.IsIntersectsWith(&(oMorphoTwo.m_GenderType)) &&
					 oMorphoOne.m_NumberType.IsIntersectsWith(&(oMorphoTwo.m_NumberType)) &&
					 oMorphoOne.m_PersonType.IsIntersectsWith(&(oMorphoTwo.m_PersonType)) );
		}
	};

	///Класс-стратегия для значения atNP
	class CPolicyNP : public IBaseAT
	{
	public:
		///Проверка применимости
		 bool IsIntersect( SS::Core::Features::CMorphoFeature & oMorphoOne,
						   SS::Core::Features::CMorphoFeature & oMorphoTwo )
		{
			return ( oMorphoOne.m_NumberType.IsIntersectsWith(&(oMorphoTwo.m_NumberType)) &&
					 oMorphoOne.m_PersonType.IsIntersectsWith(&(oMorphoTwo.m_PersonType)) );
		}
	};

	///Класс-стратегия для значения atN
	class CPolicyN : public IBaseAT
	{
	public:
		///Проверка применимости
		bool IsIntersect( SS::Core::Features::CMorphoFeature & oMorphoOne,
					 	  SS::Core::Features::CMorphoFeature & oMorphoTwo )
		{
			return oMorphoOne.m_NumberType.IsIntersectsWith(&(oMorphoTwo.m_NumberType));
		}
	};

}
}
}