//SemanticService.cpp
#include "StdAfx.h"
#include ".\semanticservice.h"

namespace SS
{
namespace Syntax
{
namespace BoardElements
{

	///Пространство фичи
	using namespace SS::Core::Features;
	
	///Конструктор
	CSemanticService::CSemanticService()
		: m_pAMConverterSemantic(NULL),
		  m_pBlackBoxTextStorage(NULL),
		  m_pUnit(NULL),
		  m_IsModified(false)
	{
	}

	///Оператор=
	void CSemanticService::operator=(const CSemanticService & v)
	{
		m_oSemanticContainer = v.m_oSemanticContainer;
		m_oSemanticFeature.AssignSemanticFeature(&v.m_oSemanticFeature);
		m_pUnit = v.m_pUnit;
		m_pAMConverterSemantic = v.m_pAMConverterSemantic;
		m_pBlackBoxTextStorage = v.m_pBlackBoxTextStorage;
		m_IsModified = v.m_IsModified;
	}

	///Конструктор копирования
	CSemanticService::CSemanticService(const CSemanticService & v) : m_oSemanticContainer(v.m_oSemanticContainer)
	{
		m_oSemanticFeature.AssignSemanticFeature(&v.m_oSemanticFeature);
		m_pUnit = v.m_pUnit;
		m_pAMConverterSemantic = v.m_pAMConverterSemantic;
		m_pBlackBoxTextStorage = v.m_pBlackBoxTextStorage;
		m_IsModified = v.m_IsModified;
	}

	///Проверка указателей
	bool CSemanticService::BadPtr()
	{
		if ( (!m_pAMConverterSemantic) || (!m_pBlackBoxTextStorage) || (!m_pUnit) )
		{ return true; }
		return false;
	}
	
	///Удаление значения Undefine из значений семантической фичи
	void CSemanticService::UndefineSemantic()
	{
		//Количество фич
		const unsigned int Count = m_oSemanticFeature.GetFeatureCount();
		//Значение фичи
		unsigned int iValue = 0;
		//Перебор фич
		for (unsigned int i = 0; i < Count; ++i)
		{
			//Получить значение
			iValue = m_oSemanticFeature.GetFeature(i)->GetValue();
			//Если фича вообще не имеет никакого значения
			if (iValue == 0) 
			{ iValue = 1; /*Ставим undefined*/ }
			//Установить значение
			m_oSemanticFeature.GetFeature(i)->SetValue(iValue);	
		}
	}

	///Обнуляем фичу
	void CSemanticService::ZeroSemantic()
	{
		//Количество фич
		const unsigned int Count = m_oSemanticFeature.GetFeatureCount();
		//Значение фичи
		const unsigned int iValue = 0;
		//Перебор фич
		for (unsigned int i = 0; i < Count; ++i)
		{ m_oSemanticFeature.GetFeature(i)->SetValue(iValue); }
	}
	
	///Сгенерировать общую семантику
	void CSemanticService::GenerateSemantic()
	{
		//Обнуляем фичу
		ZeroSemantic();
		//Перебор
		for (SSemanticData* pSemanticData = m_oSemanticContainer.GetFirst(); pSemanticData; pSemanticData = m_oSemanticContainer.GetNext())
		{ 
			//Если помечен на удаление то пропустить
			if (pSemanticData->oMark == mDelete)
			{ continue; }
			m_oSemanticFeature.AssignOR(&pSemanticData->oSemanticFeature); 
		}
		//Проставляем undefine где 0
		UndefineSemantic();
	}
	
	///Применить элемент конструкции к данному юниту
	void CSemanticService::ApplyConstructionObject( SS::SyntaxConstructions::Types::CConstructionObject* pObject,
												    const CSemanticService* pSemanticServiceBase )
	{
		SS_TRY
		{
			//Проверка данных
			if (BadPtr())
			{ SS_THROW(L"Недостаточно данных для совершения операции."); }
			
			//Проверка режима
			if (pObject->m_SemanticFeatureAposteriori.m_OperationMod.Equal(Values::OperationMod.opmRemove))
			{
				Remove(pObject);
			}
			else if (pObject->m_SemanticFeatureAposteriori.m_OperationMod.Equal(Values::OperationMod.opmSelect)) 
			{
				Select(pObject);
			}
			else if (pObject->m_SemanticFeatureAposteriori.m_OperationMod.Equal(Values::OperationMod.opmAdd)) 
			{
				Add(pObject);
			}
			else if (pObject->m_SemanticFeatureAposteriori.m_OperationMod.Equal(Values::OperationMod.opmAppoint)) 
			{
				Appoint(pObject);
			}

			//Если есть откуда, то наследуем семантику
			if (pSemanticServiceBase)
			{
				Inherit(pSemanticServiceBase);
			}
		}
		SS_CATCH(L"")
	}

	///Установить юнит
	void CSemanticService::SetUnit(SS::Interface::Core::BlackBox::IUnit* pUnit)
	{ 
		if (pUnit == NULL) 
		{ SS_THROW(L"Инициализация NULL-ом"); }
		m_pUnit = pUnit; 
	}
	
	///Установить семантический конвертер
	void CSemanticService::SetSemanticConverter(SS::Interface::Core::AMConverter::IAMConverterSemantic* pAMConverterSemantic)
	{ 
		if (pAMConverterSemantic == NULL) 
		{ SS_THROW(L"Инициализация NULL-ом"); }
		m_pAMConverterSemantic = pAMConverterSemantic; 
	}

	///Установить BlackBox фабрику
	void CSemanticService::SetTextStorage(SS::Interface::Core::BlackBox::IBlackBoxTextStorage* pBlackBoxTextStorage)
	{
		if (pBlackBoxTextStorage == NULL) 
		{ SS_THROW(L"Инициализация NULL-ом"); }
		m_pBlackBoxTextStorage = pBlackBoxTextStorage;
	}

	///Добавить семантику
	void CSemanticService::AddSemantic( SS::Interface::Core::BlackBox::IIndex* pIndex,
										SS::Core::Features::CSemanticFeature* pSemanticFeature,
										EMark oMark )
	{
		SSemanticData* pSemanticData = m_oSemanticContainer.Add();
		if (!pSemanticData) 
		{ SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"Превышение предела количества семантических характеристик."); return; }
		pSemanticData->pIndex = pIndex;
		pSemanticData->oSemanticFeature.AssignSemanticFeature(pSemanticFeature);
		pSemanticData->oMark = oMark;
		m_IsModified = true;
	}

	///Очистить список и данные
	void CSemanticService::Clear()
	{
		m_oSemanticContainer.Clear();
		m_oSemanticFeature.Undefine();
		m_pUnit = NULL;
		m_IsModified = false;
	}

	///Получить семантическую фичу
	SS::Core::Features::CSemanticFeature & CSemanticService::GetSemanticFeature()
	{ 
		if (m_IsModified) 
		{
			m_IsModified = false;
			GenerateSemantic();
		}
		return m_oSemanticFeature; 
	}

	///Удалить
	void CSemanticService::Remove(SS::SyntaxConstructions::Types::CConstructionObject* pObject)
	{
		const unsigned int MIN_SIZE = 1;
		for ( SSemanticData* pSemanticData = m_oSemanticContainer.GetFirst(); (pSemanticData && (m_oSemanticContainer.Size() > MIN_SIZE)); 
			  pSemanticData = m_oSemanticContainer.GetNext() )
		{ 
			if ( pSemanticData->oSemanticFeature.IsIntersectsWith(pObject->m_SemanticFeatureAposteriori) )
			{
				//Помечаем на удаление
				pSemanticData->oMark = mDelete;
				m_IsModified = true;
			}
		}
	}

	///Количество семантики без помеченых на удаление
	unsigned int CSemanticService::Size()
	{
		unsigned int uiSize = 0;
		for (SSemanticData* pSemanticData = m_oSemanticContainer.GetFirst(); pSemanticData; pSemanticData = m_oSemanticContainer.GetNext())
		{ 
			if (pSemanticData->oMark == mDelete) continue;
			++uiSize;
		}
		return uiSize;
	}
	
	///Выбрать
	void CSemanticService::Select(SS::SyntaxConstructions::Types::CConstructionObject* pObject)
	{
		unsigned int uiSize = Size();
		std::list<SSemanticData*> oDelDataList;
		//Перебор
		for (SSemanticData* pSemanticData = m_oSemanticContainer.GetFirst(); pSemanticData; pSemanticData = m_oSemanticContainer.GetNext())
		{
			if (pSemanticData->oMark == mDelete) continue;
			if ( !pSemanticData->oSemanticFeature.IsIntersectsWith(pObject->m_SemanticFeatureAposteriori) )
			{ oDelDataList.push_back(pSemanticData); }
		}
		//Проверка
		if (oDelDataList.size() == uiSize)
		{ return; }
		else
		{
			std::list<SSemanticData*>::iterator it(oDelDataList.begin());
			for (it; it != oDelDataList.end(); ++it)
			{
				//Помечаем на удаление
				(*it)->oMark = mDelete;
				m_IsModified = true;
			}
		}
	}
	
	///Назначить
	void CSemanticService::Appoint(SS::SyntaxConstructions::Types::CConstructionObject* pObject)
	{
		//Помечаем на удаление всю семантику
		for (SSemanticData* pSemanticData = m_oSemanticContainer.GetFirst(); pSemanticData; pSemanticData = m_oSemanticContainer.GetNext())
		{ pSemanticData->oMark = mDelete; }
		//Добавляем в контейнер
		AddSemantic(NULL, &pObject->m_SemanticFeatureAposteriori, mAdd);
	}
	
	///Добавить
	void CSemanticService::Add(SS::SyntaxConstructions::Types::CConstructionObject* pObject)
	{
		//Добавляем в контейнер
		AddSemantic(NULL, &pObject->m_SemanticFeatureAposteriori, mAdd);
	}

	///Унаследовать семантику от заданного элемента
	void CSemanticService::Inherit(const CSemanticService* pSemanticServiceBase)
	{
		//Перебор семантики базового юнита
		SSemanticData* pBaseSemanticData = pSemanticServiceBase->m_oSemanticContainer.GetFirst();
		for (pBaseSemanticData; pBaseSemanticData; pBaseSemanticData = pSemanticServiceBase->m_oSemanticContainer.GetNext())
		{ 
			//Добавляем в контейнер
			AddSemantic(NULL, &pBaseSemanticData->oSemanticFeature, mAdd);
		}
	}

	///Применить изменения семантики к BlackBox
	void CSemanticService::UpdateBlackBox()
	{
		//Перебор всей семантики
		for (SSemanticData* pSemanticData = m_oSemanticContainer.GetFirst(); pSemanticData; pSemanticData = m_oSemanticContainer.GetNext())
		{ 
			if (pSemanticData->oMark == mAdd)
			{
				//Создаем новый индекс
				SS::Interface::Core::BlackBox::IIndex* pIndex = m_pBlackBoxTextStorage->CreateIndex();
				m_pUnit->AddIndex(pIndex);
				//Индекс словаря
				SS::Dictionary::Types::TDictionaryIndex oIndexVal;
				m_pAMConverterSemantic->Code(&pSemanticData->oSemanticFeature, &oIndexVal);
				oIndexVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
				pIndex->GetDictionaryIndex()->AppendIndex(oIndexVal);
				//Снять пометку
				pSemanticData->oMark = mUndefined;
				pSemanticData->pIndex = pIndex;
			}
			else if (pSemanticData->oMark == mDelete)
			{
				SS::Interface::Core::BlackBox::IIndex* pIndex = NULL;
				pIndex = pSemanticData->pIndex;
				if (pIndex)
				{ pIndex->ReleaseIndex(); }
				m_oSemanticContainer.DeleteCurrent();
			}
		}
	}

}
}
}