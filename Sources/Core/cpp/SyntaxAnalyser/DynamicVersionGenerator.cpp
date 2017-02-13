//DynamicVersionGenerator.cpp

#include "StdAfx.h"
#include ".\dynamicversiongenerator.h"

namespace SS
{
namespace Syntax
{
namespace BoardElements
{
	///Установить элементы
	void CDynamicVersionGenerator::SetBlockRange(BoardElementsIterator itF, BoardElementsIterator itL)
	{ 
		SS_TRY
		{
			m_itF = itF; 
			m_itL = itL;
			isNull = false;
			SNode oNode;
			m_NodeCollection.clear();
			for (BoardElementsIterator it = m_itF; it != m_itL; ++it)
			{ 
				oNode.pElem = (*it);
				m_NodeCollection.push_back(oNode);
			}
		}
		SS_CATCH(L"")
	}

	///Получить первую гипотезу
	bool CDynamicVersionGenerator::GetFirstVersion()
	{
		SS_TRY
		{

			//Проверка на инициализацию
			if ( /*(m_itF == m_NullCollection.end()) ||
				 (m_itL == m_NullCollection.end()) ||*/isNull || !m_pVersion )
			{ SS_THROW(L"Не выполнена инициализация."); }

			//Заполнить гипотезу
			m_pVersion->ClearAll();

			for (BoardElementsIterator it = m_itF; it != m_itL; ++it)
			{ m_pVersion->AddElement(); }

			//Установить самую первую гипотезу
			for (IteratorNC itNode = m_NodeCollection.begin(); itNode != m_NodeCollection.end(); ++itNode)
			{ itNode->pMorpho = itNode->pElem->GetFirstMorphoDynamic();	}

			//Проверить текущую и при необходимости найти подходящую гипотезу
			if (!FindNormalVersion()) 
			{ return false; }

			//Записать гипотезу
			WriteVersion();

		}
		SS_CATCH(L"")	
		return true;
	}

	///Получить следующую гипотезу
	bool CDynamicVersionGenerator::GetNextVersion()
	{
		SS_TRY
		{
			//Сгенерировать очередную гипотезу
			if (!GenerateVersion())
			{ 
				return false;
			}
			//Проверить текущую и при необходимости найти подходящую гипотезу
			if (!FindNormalVersion()) 
			{ 
				return false; 
			}
			//Записать гипотезу
			WriteVersion();
		}
		SS_CATCH(L"")
		return true;
	}
	
	///===============================================Private=================================================///

	///Найти подходящую гипотезу
	bool CDynamicVersionGenerator::FindNormalVersion()
	{
		SS_TRY
		{

			/// защита от дурака
			for (int i = 0; i < 100; i++)
			{
				if(CheckVersion())
				{ return true; }
				if (!GenerateVersion())
				{ return false; }
			}

		}
		SS_CATCH(L"")
		return true;
	}

	///Записать гипотезу
	void CDynamicVersionGenerator::WriteVersion() const
	{
		SS_TRY
		{
			ConstIteratorNC itNode(m_NodeCollection.begin());
			for (PVERELEMENT pVE = m_pVersion->GetFirst(); pVE; pVE = m_pVersion->GetNext())
			{ 
				pVE->Init(itNode->pElem, itNode->pMorpho);
				++itNode;
			}
		}
		SS_CATCH(L"")
	}
	
	///Сгенерировать очередную гипотезу
	bool CDynamicVersionGenerator::GenerateVersion()
	{
		SS_TRY
		{
			bool bChanged = false;
			for (IteratorNC itNode = m_NodeCollection.begin(); itNode != m_NodeCollection.end(); ++itNode)
			{
				itNode->pMorpho = itNode->pElem->GetNextMorphoForAddress(itNode->pMorpho);
				if (itNode->pMorpho)
				{
					bChanged = true;
					break;
				}
				else
				{ 
					itNode->pMorpho = itNode->pElem->GetFirstMorphoDynamic();
				}
			}
			if (bChanged) 
			{ 
				return true; 
			}
		}
		SS_CATCH(L"")
		return false;
	}
	
	///Проверить гипотезу на совпадение по АТ
	bool CDynamicVersionGenerator::CheckVersion() const
	{
		int numOfIter = 0;
		SS_TRY
		{
			//Узлы содержащие АТ
			std::list<SNode> l_NodesAT;
			//Порядковый номер конструкции
			unsigned int iNumber = 0;
			//Отбор элементов по порядку сработавших конструкций
			do
			{
				l_NodesAT.clear();
				//Номер очередной конструкции
				++iNumber;
				//Отбор узлов с АТ первой сработавшей конструкции
				for (ConstIteratorNC itNode = m_NodeCollection.begin(); itNode != m_NodeCollection.end(); ++itNode)
				{ 
					if ( itNode->pElem->IsAT(iNumber) )
					{ 
						l_NodesAT.push_back(*itNode);
					}
				}
				//Проверка присутствия узлов
				if (l_NodesAT.empty())
				{ 
					return true;
				}
						
				//Сам АТ
				SS::Core::Features::Types::AgreementType oAT;
				//Количество значений АТ
				const unsigned int CountValueAT = SS::Core::Features::Values::AgreementType.GetValueCount();
				//Выборка узлов по конкретному АТ
				std::list<SNode> l_NodesOneAT;
				//Итератор списков
				std::list<SNode>::iterator itN;
				std::list<SNode>::iterator itNBegin;
				//Перебор по АТ
				for (unsigned int iAT = 0; iAT < CountValueAT; ++iAT)
				{
					//Очистка контейнера
					l_NodesOneAT.clear();
					//Установить АТ
					oAT.SetSingleValue(iAT);
					if (oAT.IsUndefined()) { continue; }
					//Заполнение конкретной АТ коллекции
					for (itN = l_NodesAT.begin(); itN != l_NodesAT.end(); ++itN)
					{
						if ( itN->pElem->GetAT(iNumber).IsIntersectsWith(&oAT) )
						{ 
							l_NodesOneAT.push_back(*itN);
						}
					}
					//Проверка
					itNBegin = l_NodesOneAT.begin();
					if ( l_NodesOneAT.size()>0)
					{
						itN = itNBegin; ++itN;
						//Проверить на совпадение по выбранному АТ
						for (itN; itN != l_NodesOneAT.end(); ++itN)
						{
							if (itN->pMorpho == NULL || itNBegin->pMorpho == NULL) 
							{
								return false;
							}
							if (!IsIntersectByAT(itNBegin->pMorpho->GetMorphoFeature(), itN->pMorpho->GetMorphoFeature(), oAT) )
							{ 
								return false; 
							}
						}	
					}
				}
				numOfIter++;
				if (numOfIter > 100000) 
				{
					return false;
				}
			} 
			while (!l_NodesAT.empty());
		}
		SS_CATCH(L"")
		return true;
	}

	///Проверка пересечения по AgreementType
	bool CDynamicVersionGenerator::IsIntersectByAT( SS::Core::Features::CMorphoFeature & oMorphoOne,
													SS::Core::Features::CMorphoFeature & oMorphoTwo,
													SS::Core::Features::Types::AgreementType & oAt ) const
	{
		SS_TRY
		{
			if ( oAt.IsIntersectsWith(&SS::Core::Features::Values::AgreementType.atC) )
			{ 
				return oMorphoOne.m_CaseType.IsIntersectsWith(&(oMorphoTwo.m_CaseType));
			}
			else if ( oAt.IsIntersectsWith(&SS::Core::Features::Values::AgreementType.atGN) )
			{ 
				return ( oMorphoOne.m_GenderType.IsIntersectsWith(&(oMorphoTwo.m_GenderType)) &&
					     oMorphoOne.m_NumberType.IsIntersectsWith(&(oMorphoTwo.m_NumberType)) );
			}
			else if ( oAt.IsIntersectsWith(&SS::Core::Features::Values::AgreementType.atGNC) )
			{ 
				return ( oMorphoOne.m_GenderType.IsIntersectsWith(&(oMorphoTwo.m_GenderType)) &&
						 oMorphoOne.m_NumberType.IsIntersectsWith(&(oMorphoTwo.m_NumberType)) &&
						 oMorphoOne.m_CaseType.IsIntersectsWith(&(oMorphoTwo.m_CaseType)) );
			}
			else if ( oAt.IsIntersectsWith(&SS::Core::Features::Values::AgreementType.atGPN) )
			{ 
				return ( oMorphoOne.m_GenderType.IsIntersectsWith(&(oMorphoTwo.m_GenderType)) &&
						 oMorphoOne.m_NumberType.IsIntersectsWith(&(oMorphoTwo.m_NumberType)) &&
						 oMorphoOne.m_PersonType.IsIntersectsWith(&(oMorphoTwo.m_PersonType)) );
			}
			else if ( oAt.IsIntersectsWith(&SS::Core::Features::Values::AgreementType.atNP) )
			{ 
				return ( oMorphoOne.m_NumberType.IsIntersectsWith(&(oMorphoTwo.m_NumberType)) &&
						 oMorphoOne.m_PersonType.IsIntersectsWith(&(oMorphoTwo.m_PersonType)) );
			}
			else if ( oAt.IsIntersectsWith(&SS::Core::Features::Values::AgreementType.atN) )
			{ 
				return oMorphoOne.m_NumberType.IsIntersectsWith(&(oMorphoTwo.m_NumberType));
			}
			else
			{ SS_THROW(L"Неподдерживаемый AgreementType."); }

		}
		SS_CATCH(L"")
		return false;
	}

}
}
}



