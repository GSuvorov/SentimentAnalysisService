//Iterators.h
#pragma once
#include ".\BoardElement.h"
#include "FeatureController.h"
#include "UnitSearchPolicys.h"

namespace SS
{
namespace Syntax
{
namespace ModifiedConstruction
{
	using namespace SS::Syntax::BoardElements;
	using namespace SS::Syntax::Policys;
	
	///Шаблонный класс-итератор для поиска очередного элемента
	template<typename TRange, typename TBreak, typename TCheck>
	class CObjectIterator
	{
	private:
		///Объект диапазона
		TRange oRange;
		///Объект остановки
		TBreak oBreak;
		///Объект проверки
		TCheck oCheck;

	protected:
		///Конструктор по умолчанию
		CObjectIterator(){};
		///Конструктор для HeadAndCurrentGroupChildren
		CObjectIterator(unsigned int IdGroup) : oCheck(IdGroup){};
		///Конструктор для Child
		CObjectIterator(BoardElementsIterator & itBeginBoard) : oCheck(itBeginBoard){};
	
	protected:
		///Установить IdParent
		void SetIdParent(unsigned int IdParent)
		{
			oCheck.SetIdParent(IdParent);
		}
		///Инициализация диапазона
		void InitRange(BoardElementsIterator* pFirstIt, BoardElementsIterator* pLastIt)
		{
			oRange.Init(pFirstIt, pLastIt);
		}
		///Первый элемент
		CBoardElement* First() const
		{
			for (CBoardElement* pElem = oRange.First(); pElem; pElem = oRange.Next())
			{
				if (oBreak.Break(pElem))
				{ return NULL; }
				if (oCheck.Check(pElem))
				{ return pElem; }
			}
			return NULL;
		}
		///Следующий элемент
		CBoardElement* Next() const
		{
			CBoardElement* pElem;
			while (pElem = oRange.Next())
			{
				if (oBreak.Break(pElem))
				{ return NULL; }
				if (oCheck.Check(pElem))
				{ return pElem; }
			}
			return NULL;
		}
	};
		
	///Интерфейс для для "объектного анализатора"
	class IObjectAnalyser
	{
	public:
		///Деструктор
		virtual ~IObjectAnalyser(){};
		///Запустить анализ элемента
		virtual bool Run() = 0;
		///Установить анализатор применимости
		virtual void SetFeatureController(IFeatureController* pFeatureController) = 0;
		///Получить сработавший элемент
		virtual CBoardElement* GetWorkedElement() = 0;
		///Вернуть объект конструкции
		virtual CConstructionObject* GetConstructionObject() = 0;
		///Инициализация диапазона
		virtual void InitRange(BoardElementsIterator* pFirstIt, BoardElementsIterator* pLastIt) = 0;
		///Инициализация всего диапазона
		virtual void InitAllRange(BoardElementsIterator* pFirstIt, BoardElementsIterator* pLastIt) = 0;
		///Установить IdParent
		virtual void SetIdParent(unsigned int IdParent) = 0;
		///Добавить объект АТ
		virtual void AddATObject(IBaseAT* pAT, IObjectAnalyser* pObject) = 0;
	};
	
	///Шаблонный класс принятия решений для одного объекта конструкции
	template<typename TRange, typename TBreak, typename TCheck, typename TContinuance, bool Existence, bool Obligation>
	class CObjectAnalyser : public IObjectAnalyser, private CObjectIterator<TRange, TBreak, TCheck>
	{
	private:
		///Структура содержащая АТ и обьект анализа с которым сравнивать
		struct SATObject
		{
			///Стратегия расчета АТ
			IBaseAT* pBaseAT;
			///Объект анализа
			IObjectAnalyser* pObjectAnalyser;
			///Конструктор
			SATObject() : pObjectAnalyser(NULL), pBaseAT(NULL){};
		};

	private:
		///Матрица принятия решений
		bool m_AnswerMatrix[2][2][2];
		///Анализатор применимости по Морфологии Семантике и Синтаксису
		IFeatureController* m_pFeatureController;
		///Применимость по Морфологии Семантике и Синтаксису
		bool m_Appropriate;
		///Сработавший элемент
		CBoardElement* m_pBoardElement;
		///Коллекция обьектов анализа с которыми проверять по АТ
		std::vector<SATObject> m_vATObjectCollection;

	private:
		///Заполнение матрицы
		void FillData()
		{
			m_AnswerMatrix[0][0][0] = true;  m_AnswerMatrix[0][0][1] = true; m_AnswerMatrix[0][1][0] = true;
			m_AnswerMatrix[0][1][1] = false; m_AnswerMatrix[1][0][0] = true; m_AnswerMatrix[1][0][1] = false;
			m_AnswerMatrix[1][1][0] = true;  m_AnswerMatrix[1][1][1] = true;
			m_Appropriate = false;
			m_pFeatureController = NULL;
			m_pBoardElement = NULL;
		}
		///Применение по АТ
		bool IsAppropriateAT(CBoardElement* pElem)
		{
			//Если проверять не нужно
			if (m_vATObjectCollection.empty()) 
			{ return true; }
			//Элемент из списка
			CBoardElement* pElemBegin = NULL;

			pElem->GetMorphoContainerAT().clear();
			bool IsIntersect = false;
			for (std::vector<SATObject>::iterator it = m_vATObjectCollection.begin(); it != m_vATObjectCollection.end(); ++it)
			{
				//Если сработавший объект не содержит юнита
				pElemBegin = it->pObjectAnalyser->GetWorkedElement();
				if (!pElemBegin)
				{ continue; }
				
				//Если сравнения еще не было, сравниваем по всему списку морфологии первого элемента
				if (pElemBegin->GetMorphoContainerAT().empty())
				{
					for ( PMORPHO pMorphoElemBegin = pElemBegin->GetFirstMorpho(); pMorphoElemBegin; 
						  pMorphoElemBegin = pElemBegin->GetNextMorpho() )
					{
						for ( PMORPHO pMorphoElem = pElem->GetFirstMorpho(); pMorphoElem;
							  pMorphoElem = pElem->GetNextMorpho() )
						{
							if ( it->pBaseAT->IsIntersect(pMorphoElemBegin->GetMorphoFeature(), pMorphoElem->GetMorphoFeature()) )
							{
								pElemBegin->AddMorphoAT(pMorphoElemBegin);
								pElem->AddMorphoAT(pMorphoElem);
								IsIntersect = true;
							}
						}	
					}	
				}
				else
				{
					std::list<PMORPHO>::iterator itMorphoElemBegin;
					for ( itMorphoElemBegin = pElemBegin->GetMorphoContainerAT().begin(); 
						  itMorphoElemBegin != pElemBegin->GetMorphoContainerAT().end(); 
						  ++itMorphoElemBegin )
					{
						for ( PMORPHO pMorphoElem = pElem->GetFirstMorpho(); pMorphoElem;
							  pMorphoElem = pElem->GetNextMorpho() )
						{
							if ( it->pBaseAT->IsIntersect((*itMorphoElemBegin)->GetMorphoFeature(), pMorphoElem->GetMorphoFeature()) )
							{
								pElem->AddMorphoAT(pMorphoElem);
								IsIntersect = true;
							}
						}	
					}
				}
				//Если не совпали по АТ
				if (!IsIntersect)
				{ return false; }
			}
			return true;
		}

	public:
		///Конструктор по умолчанию
		CObjectAnalyser() { FillData(); };
		///Конструктор для HeadAndCurrentGroupChildren
		CObjectAnalyser(unsigned int IdGroup) : CObjectIterator<TRange, TBreak, TCheck>(IdGroup) { FillData(); };
		///Конструктор для Child
		CObjectAnalyser(BoardElementsIterator & itBeginBoard) : CObjectIterator<TRange, TBreak, TCheck>(itBeginBoard) { FillData(); };
	
	public:		
		///Инициализация диапазона
		void InitRange(BoardElementsIterator* pFirstIt, BoardElementsIterator* pLastIt)
		{
			CObjectIterator<TRange, TBreak, TCheck>::InitRange(pFirstIt, pLastIt);
		}
		///Инициализация всего диапазона (всего блока)
		void InitAllRange(BoardElementsIterator* pFirstIt, BoardElementsIterator* pLastIt)
		{
			m_pFeatureController->InitRange(pFirstIt, pLastIt);
		}
		///Запустить анализ
		bool Run()
		{
			SS_TRY
			{
				m_Appropriate = false;
				//Поиск подходящего элемента
				CBoardElement* pElem;
				for (pElem = First(); pElem; pElem = Next())
				{
					m_Appropriate = (m_pFeatureController->IsAppropriate(pElem) && IsAppropriateAT(pElem));
					if (m_Appropriate)
					{ break; }
					if (!TContinuance::Continue(pElem))
					{ break; }
				}
				//Если элемент сработал
				if (m_AnswerMatrix[m_Appropriate][Existence][Obligation])
				{ 
					if (m_Appropriate && Existence)
					{ m_pBoardElement = pElem; }
					else
					{ m_pBoardElement = NULL; }
					return true; 
				}
				//Если не сработал
				m_pBoardElement = NULL;
			}
			SS_CATCH(L"")
			return false;
		}
		///Установить анализатор применимости
		void SetFeatureController(IFeatureController* pFeatureController)
		{
			m_pFeatureController = pFeatureController;
		}
		///Получить сработавший элемент
		CBoardElement* GetWorkedElement()
		{
			return m_pBoardElement;
		}
		///Вернуть объект конструкции
		CConstructionObject* GetConstructionObject()
		{ 
			return  m_pFeatureController->GetConstructionObject();
		}
		///Установить IdParent
		void SetIdParent(unsigned int IdParent)
		{
			CObjectIterator<TRange, TBreak, TCheck>::SetIdParent(IdParent);
		}
		///Добавить объект АТ
		void AddATObject(IBaseAT* pAT, IObjectAnalyser* pObject)
		{
			SATObject oATObject;
			oATObject.pBaseAT = pAT;
			oATObject.pObjectAnalyser = pObject;
			m_vATObjectCollection.push_back(oATObject);
		}
	};	

}
}
}