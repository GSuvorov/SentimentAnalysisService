//BaseExpertConstruction.cpp

#include "StdAfx.h"
#include ".\baseexpertconstruction.h"
#include "../ASSInterface/IDictionaryManager.h"
#include "../[libs]/_Paths.h"

namespace SS
{
namespace Syntax
{
namespace ExpertModel
{
	//Установить детект-слова
	void CBaseExpertConstruction::SetDetectWords(SS::Interface::Core::Dictionary::IDictionary* pDictionary)
	{
		SS_TRY
		{
			typedef std::list<SS::Dictionary::Types::TWordInfo>::iterator TWordInfoIt;
			std::list<SS::Dictionary::Types::TWordInfo> lWordInfo;
			std::list<SS::Dictionary::Types::TWordInfo> lWordInfoDetect;

			//Перебор конструкции
			SS::SyntaxConstructions::Types::CConstruction::iterator itConObj(m_oConstruction.begin());
			for (itConObj; itConObj != m_oConstruction.end(); ++itConObj)
			{
				//Детект-слова одного объекта
				std::set<std::wstring> DetectWords;
				//Перебор детект-слов объекта
				for ( SS::SyntaxConstructions::Types::TDetectWordsGroup::iterator itWord = (*itConObj)->GetDetectWords().begin();
					  itWord != (*itConObj)->GetDetectWords().end(); ++itWord )
				{
					if ( (*itConObj)->m_MorphoFeatureApriori.m_DetectWordAnalyzeMode.Equal(Values::TDetectWordAnalyzeMode.mfamAllForms) )
					{
						//Получить список информации о слове
						pDictionary->GetMorphologyAnalyser()->GetWordInfo( itWord->c_str(), &(lWordInfo), 
																		   SS::Dictionary::Types::efzOnlySearch );
						for (TWordInfoIt itWordInfo = lWordInfo.begin(); itWordInfo != lWordInfo.end(); ++itWordInfo)
						{
							lWordInfoDetect.clear();
							pDictionary->GetMorphologyAnalyser()->GetWordFormsByIDSource( &(itWordInfo->front().m_WordIndex.m_DictIndex), 
																						  &(lWordInfoDetect) );
							for (TWordInfoIt itWordInfoDet = lWordInfoDetect.begin(); itWordInfoDet != lWordInfoDetect.end(); ++itWordInfoDet)
							{ DetectWords.insert(itWordInfoDet->front().m_wWord); }
						}
						lWordInfo.clear();
					}
					else
					{ DetectWords.insert(*itWord); }
				}

				//Занесение очередного множества в список
				if ( (*itConObj)->m_MorphoFeatureApriori.m_ExistenceMod.Equal(Values::ExistenceMod.excmExist) &&
					 (*itConObj)->m_ObjectFeature.m_ObjectObligation.Equal(Values::ObjectObligation.ecoObligatory) && 
					 !DetectWords.empty() )
				{ 
					m_DetectWords.push_back(DetectWords); 
				}
			}
		}
		SS_CATCH(L"")
	}

	//Конструктор
	CBaseExpertConstruction::CBaseExpertConstruction( SS::SyntaxConstructions::Types::CConstructionEx & oConstruction,
													  SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager,
													  SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams,
													  int IdGroup, ESyntaxBase eBase )
	: 
	m_IdGroup(IdGroup), m_oConstruction(oConstruction), m_pModifiedConstruction(NULL)
	{
		m_itBeginBoard = m_NullCollection.end();
		m_itFirst = m_NullCollection.end();
		m_itLast = m_NullCollection.end();
		m_itFirstR = m_NullCollection.end();
		m_itLastL = m_NullCollection.end();
		m_itBlockBegin = m_NullCollection.end();
		m_itBlockEnd = m_NullCollection.end();
		try
		{
			//Получить словарь
			SS::Interface::Core::Dictionary::IDictionary*
			pDictionary = (SS::Interface::Core::Dictionary::IDictionary*)
			pLoadManager->GetInterface( DICTIONARIES_DLL_PATH, CLSID_IDictionary, IID_IDictionary);
			//Загрузить словарь
			pDictionary->GetLoader()->Load();
			pDictionary->SetAnalyseParams(pAnalyseParams);
			//Инициализация указателя
			m_pModifiedConstruction = new CModifiedConstruction(m_itBeginBoard, m_IdGroup, eBase);
			//Инициализация конструкции
			m_pModifiedConstruction->Create(m_oConstruction, pDictionary);
			//Установить диапазоны
			SetRange();
			//Установить детект-слова
			SetDetectWords(pDictionary);
		}
		catch (...)
		{
			Delete();
			SS_THROW((wchar_t*)m_oConstruction.GetName().c_str());
		}
	}

	//Деструктор
	CBaseExpertConstruction::~CBaseExpertConstruction()
	{
		Delete();
	}

	//Освободить память
	void CBaseExpertConstruction::Delete()
	{
		if (m_pModifiedConstruction)
		{ 
			delete m_pModifiedConstruction;
		}
	}

	//Установить элементам диапазон
	void CBaseExpertConstruction::SetRange()
	{
		IModifiedConstruction::iterator it(m_pModifiedConstruction->begin());
		for (it; it != m_pModifiedConstruction->end(); ++it)
		{
			//Установить границы блока
			(*it)->InitAllRange(&m_itBlockBegin, &m_itBlockEnd);
			//Установить диапазоны
			if ( (*it)->GetConstructionObject()->m_ObjectFeature.m_PositionToDetectedObject.Equal(Values::PositionToDetectedObject.ptdoRight) ||
				 (*it)->GetConstructionObject()->m_ObjectFeature.m_PositionToDetectedObject.IsUndefined() )
			{
				(*it)->InitRange(&m_itFirstR, &m_itLast);
			}
			else if ( (*it)->GetConstructionObject()->m_ObjectFeature.m_PositionToDetectedObject.Equal(Values::PositionToDetectedObject.ptdoLeft) ) 
			{
				(*it)->InitRange(&m_itFirst, &m_itLastL);
			}
		}
	}

	//Расчитать IdParent с учетом SameObject
	unsigned int CBaseExpertConstruction::GetIdParentReal(unsigned int idParent)
	{
		unsigned int Id = 0;
		CConstruction::iterator it = m_oConstruction.begin();
		for (unsigned int i = 0; i < idParent; ++i, ++it)
		{ 
			Id += (*it)->m_ObjectFeature.m_SameTypeObjectsNumber.GetValue(); 
		}
		return Id;
	}

	///Применить конструкцию
	void CBaseExpertConstruction::ApplyConstruction(unsigned int iCountApplyedExpertAT)
	{
		SS_TRY
		{
			CBoardElement* pElem = NULL;
			CBoardElement* pElemParent = NULL;
			CBoardElement* pElemBaseSemantic = NULL;
			unsigned int IdParentObject, IdParent, IdOwn, IdBaseSemantic;
			IModifiedConstruction::iterator it(m_pModifiedConstruction->begin());
			for (it; it != m_pModifiedConstruction->end(); ++it)
			{
				pElem = (*it)->GetWorkedElement();
				if (!pElem || pElem->IsUnusedForm()) 
				{ 
					continue; 
				}
				IdParentObject = GetIdParentReal( (*it)->GetConstructionObject()->m_SyntaxFeatureAposteriori.m_IdParent.GetValue() );
				IdOwn = pElem->GetPosition();
				//Родительский элемент
				pElemParent = (*(m_pModifiedConstruction->begin() + IdParentObject))->GetWorkedElement();
				if (pElemParent)
				{ 
					IdParent = pElemParent->GetPosition();
				}
				else
				{ 
					IdParent = IdOwn;	
				}
				
				//Вычисление элемента от которого будем наследовать семантику
				IdBaseSemantic = (*it)->GetConstructionObject()->m_SemanticFeatureAposteriori.m_ShiftSemantic.GetValue();
				if (IdBaseSemantic == SS::Core::Features::CSemanticFeatureAposteriori::DEF_SHIFT)
				{ 
					pElemBaseSemantic = NULL; 
				}
				else
				{
					//используем метод GetIdParentReal() для нахождения реального BaseSemantic
					IdBaseSemantic = GetIdParentReal(IdBaseSemantic);
					pElemBaseSemantic = (*(m_pModifiedConstruction->begin() + IdBaseSemantic))->GetWorkedElement();
				}
				//Применение конструкции
				pElem->ApplyConstruction((*it)->GetConstructionObject(), IdParent, IdOwn, m_IdGroup, iCountApplyedExpertAT, pElemBaseSemantic);
			}
		}
		SS_CATCH(L"")
	}

	//Проверка применимости эксперта к предложению
	bool CBaseExpertConstruction::IsApplicable()
	{
		SS_TRY
		{
			//Проверка на соответствие по DetectWords
			std::list< std::set<std::wstring> >::iterator itWordsSet;
			BoardElementsIterator it;
			bool bIsWord = false;

			for (itWordsSet = m_DetectWords.begin(); itWordsSet != m_DetectWords.end(); ++itWordsSet)
			{
				bIsWord = false;
				for (it = m_itBlockBegin; it != m_itBlockEnd; ++it)
				{
					if (itWordsSet->find((*it)->GetWord()) != itWordsSet->end()) 
					{ 
						bIsWord = true; 
						break;
					}
				}
				if (!bIsWord) 
				{ 
					return false; 
				}
			}

			//Проверка на соответствие по MorphoFeature
			for (it = m_itBlockBegin; it != m_itBlockEnd; ++it)
			{
				for (PMORPHO pMorpho = (*it)->GetFirstMorphoDynamic(); pMorpho; pMorpho = (*it)->GetNextMorphoDynamic())
				{
					if ( (*m_oConstruction.begin())->m_MorphoFeatureApriori.IsIntersectsWith(pMorpho->GetMorphoFeature(), (*it)->GetLang()) )
					{ 
						return true; 
					}
				}
			}
		}
		SS_CATCH(L"")
		return false;
	}

	///Возвращает текстовое название эксперта
	std::wstring CBaseExpertConstruction::GetName() const
	{
		return m_oConstruction.GetName();
	}

}
}
}
