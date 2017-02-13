//AdditionalTest.cpp

#include "StdAfx.h"
#include ".\additionaltest.h"

//Если тестирование
#ifdef _SS_UNITTESTS

namespace SS
{
namespace UnitTests
{

	///Установить менеджер (инициализировать m_pDictionary)
	void CAdditionalTest::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
	{
		//Получить словарь
		m_pDictionary = (SS::Interface::Core::Dictionary::IDictionary*)
		pLoadManager->GetInterface(L"./core/linguistics/Dictionaries.dll", CLSID_IDictionary, IID_IDictionary);
		if (!m_pDictionary)
		{ SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"Не удается получить IDictionary*"); }
		//Получить морфоконвертер
		m_pAMConverterMorpho = (SS::Interface::Core::AMConverter::IAMConverterMorpho*)
		pLoadManager->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);
		if (!m_pAMConverterMorpho)
		{ SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"Не удается получить IAMConverterMorpho*"); }
	}

	///Убрать лишние юниты, если нужен только один
	void CAdditionalTest::UnitCollectionFilter()
	{
		if (!m_bOneWord) 
		{ return; }
			
		//Текущее слово
		std::wstring wsWord;

		TIterator it(m_UnitCollection.begin());
		while (it != m_UnitCollection.end())
		{ 
			wsWord = GetWordFromUnit(*it);     
			//Удаляем лишнее
			if (wsWord != m_wsWord)
			{ it = m_UnitCollection.erase(it); continue; }
			++it;
		}
		if (m_UnitCollection.empty())
		{ SAVE_LOG(L"ЮНИТА НЕТ, ЛИБО НЕТ ЛИТЕРАЛЬНОГО ПРЕДСТАВЛЕНИЯ.");	exit(0); }
	}

	///Заполнить коллекцию юнитов
	void CAdditionalTest::GetUnitCollection(IText* pText)
	{
		//Очистка контейнера юнитов
		m_UnitCollection.clear();

		IParagraph* pPar = NULL;
		ISentence* pSent = NULL;
		IUnit* pCurrUnit = NULL;
		IUnit* pRightUnit = NULL;
		IUnit* pChildUnit = NULL;

		pPar = pText->GetFirstParagraph();
		if (!pPar)
		{ SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"IText не содержит ни одного IParagraph."); return; }
		pSent = pPar->GetFirstSentence();
		if (!pSent)
		{ SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"IParagraph не содержит ни одного ISentence."); return; }

		//Перебор Unit-ов
		pCurrUnit = pSent->GetFirstUnit();
		while (pCurrUnit)
		{
			pChildUnit = pCurrUnit->GetLeftChildUnit();
			if (pChildUnit)
			{
				pCurrUnit = pChildUnit;
				continue;
			}
			//ОЧЕРЕДНОЙ ЮНИТ
			m_UnitCollection.push_back(pCurrUnit);

			pRightUnit = pCurrUnit->GetRightUnit();
			while (pRightUnit == NULL)
			{
				pCurrUnit = pCurrUnit->GetParentUnit();
				if (pCurrUnit == NULL)
				{ break; }
				//ОЧЕРЕДНОЙ ЮНИТ
				m_UnitCollection.push_back(pCurrUnit);

				pRightUnit = pCurrUnit->GetRightUnit();
			}
			pCurrUnit = pRightUnit;
		}
		//Перебор Unit-ов(конец)
		if (m_UnitCollection.empty()) 
		{ SAVE_LOG(L"ЮНИТОВ В ТЕКСТЕ НЕТ."); exit(0); }
	}

	///Выполнить проверку текста
	void CAdditionalTest::AnalyseText(IText* pText)
	{
		//Вынуть все юниты
		GetUnitCollection(pText);
		//Отфильтровать
		UnitCollectionFilter();
		//Перебрать юниты
		SAVE_LOG(L"Sentence::::::::::");
		TIterator itUnit(m_UnitCollection.begin());
		for (itUnit; itUnit != m_UnitCollection.end(); ++itUnit)
		{ ShowInfoFromTextUnit(*itUnit); }
	}

	///Вывод текстового значения unit-а
	void CAdditionalTest::ShowInfoFromTextUnit(IUnit* pUnit) const
	{
		//Вывод слова
		std::wstring wsWord = GetWordFromUnit(pUnit); SAVE_LOG((wchar_t*)wsWord.c_str());
		//Вывод синтаксического флага
		//ShowSyntaxFlag(pUnit);
		//Вывод синтаксической категории
		//ShowSyntaxCategory(pUnit);
		
		IIndex*				pIndex = NULL;
		IDictionaryIndex*	pDictionaryIndex = NULL;
		TDictionaryIndex	oTDictionaryIndex;

		//Перебор индексов
		for (pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
		{
			//Получение IDictionaryIndex
			pDictionaryIndex = pIndex->GetDictionaryIndex();
			if (!pDictionaryIndex) continue;
			//Получение TDictionaryIndex
			if ( !(pDictionaryIndex->GetFirst(&oTDictionaryIndex)) ) continue;
				
			//Какой не нужен индекс
			if (oTDictionaryIndex.GetIndexType() != eitMorphoIndex) continue;
			
			//Вывод индекса
			//ShowIndex(oTDictionaryIndex);
			//Вывод данных из морфологических фич юнита
			ShowMorphoFeature(pIndex);
			//Вывод информации о Features
			//ShowFeaturesInfo(pIndex);
		}
	}

	///Вывод информации о IFeatures для каждого IIndex
	void CAdditionalTest::ShowFeaturesInfo(IIndex* pIndex) const
	{
		SAVE_LOG(L"Очередной IIndex: ");
		
		unsigned int iMorphoIndex = 0; 
		unsigned int iMorphoInfo = 0;
		unsigned int iPartOfSpeech = 0;

		wchar_t BuffMorphoIndex[30];
		wchar_t BuffMorphoInfo[30];
		wchar_t BuffPartOfSpeech[30];
		
		for (IFeature* pFeature = pIndex->GetFirstFeature(); pFeature; pFeature = pIndex->GetNextFeature())
		{
			iMorphoIndex = pFeature->GetMorphoIndex();
			_itow(iMorphoIndex, BuffMorphoIndex, 10);
			iMorphoInfo = pFeature->GetMorphoInfo();
			_itow(iMorphoInfo, BuffMorphoInfo, 10);
			iPartOfSpeech = pFeature->GetPartOfSpeech();
			_itow(iPartOfSpeech, BuffPartOfSpeech, 10);
			//Вывод в лог
			SAVE_LOG(L"IFeature: " AND BuffMorphoIndex AND L"  " AND BuffMorphoInfo AND L"  " AND BuffPartOfSpeech);
		}
	}

	///Вывод синтаксического флага
	void CAdditionalTest::ShowSyntaxFlag(IUnit* pUnit) const
	{
		wchar_t Buff[30];
		_itow((int)pUnit->GetSyntaxFlag(), Buff, 10);
		SAVE_LOG(L"Синтаксический флаг: " AND Buff);
	}
	
	///Вывод синтаксической категории юнита
	void CAdditionalTest::ShowSyntaxCategory(IUnit* pUnit) const
	{
		std::string sCategories = pUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.ToString();
		wchar_t Buff[30];
		MultiByteToWideChar(0, 0, sCategories.c_str(), 30, Buff, 30);
		SAVE_LOG(L"синтаксическая категория: " AND Buff);
	}

	///Вывод данных из морфологических фич юнита
	void CAdditionalTest::ShowMorphoFeature(IIndex* pIndex) const
	{
		if (!m_pAMConverterMorpho)
		{ SS_THROW(L"Установите_LoadManager."); }
		//Морфологическая фича
		SS::Core::Features::CMorphoFeature oMorphoFeature;
		TDictionaryIndex oTDictionaryIndex;
		try
		{
			pIndex->GetDictionaryIndex()->GetFirst(&oTDictionaryIndex);
		}
		catch (...) 
		{
			SAVE_LOG(L"Не могу получить TDictionaryIndex.");
			throw;
		}
		
		//Перебор ай-фич
		for (IFeature* pFeature = pIndex->GetFirstFeature(); pFeature; pFeature = pIndex->GetNextFeature())
		{
			//Получение морфо-фичи
			m_pAMConverterMorpho->EnCode( oTDictionaryIndex.GetDictionaryNumber(), pFeature->GetMorphoInfo(),
										  pFeature->GetPartOfSpeech(), &(oMorphoFeature) );
			//Получить часть речи
			//std::string sPartOfSpeach = oMorphoFeature.m_PartOfSpeechTypes.ToString();
			//wchar_t Buff[30];
			//MultiByteToWideChar(0, 0, sPartOfSpeach.c_str(), 30, Buff, 30);
			//SAVE_LOG(L"часть речи: " AND Buff);

			std::string sXML; 
			oMorphoFeature.GetXMLValue(sXML);
			SAVE_LOG(L"XML: " AND (wchar_t*)sXML.c_str());
		}
	}

	/*************************МЕТОДЫ ДЛЯ IQuery*************************/

	///Выполнить проверку запроса
	void CAdditionalTest::AnalyseQuery(IQuery* pQuery)
	{
		ITransformation*			pTransformation = NULL;
		IGroup*						pGroup = NULL;
		IQueryUnit*					pQueryUnit = NULL;
	
		//Перебор запроса
		for (pTransformation = pQuery->GetFirstTransformation(); pTransformation; pTransformation = pTransformation->GetNextTransformation())
		{
			SAVE_LOG(L"Transformation::::::::::");
			for (pGroup = pTransformation->GetFirstGroup(); pGroup; pGroup = pGroup->GetNextGroup())
			{
				for (pQueryUnit = pGroup->GetFirstQueryUnit(); pQueryUnit; pQueryUnit = pQueryUnit->GetNextQueryUnit())
				{ ShowInfoFromQueryUnit(pQueryUnit); }
			}
		}
	}

	///Вывод текстового значения квери-юнита
	void CAdditionalTest::ShowInfoFromQueryUnit(IQueryUnit* pQueryUnit) const
	{
		//Вывод слова
		std::wstring wsWord = GetWordFromQueryUnit(pQueryUnit); SAVE_LOG((wchar_t*)wsWord.c_str());
				
		IQueryUnitIndexCollection*	pCollection = NULL;
		IQueryUnitIndex*			pIndex = NULL;
		
		//Перебор индексов
		for (pCollection = pQueryUnit->GetFirstQueryUnitIndexCollection(); pCollection; pCollection = pCollection->GetNextQueryUnitIndexCollection())
		{
			//Какая не нужна коллекция
			//if (pCollection->GetIndexCollectionType() != eitMorphoIndex) continue;
			//Вывод индексов
			for (pIndex = pCollection->GetFirstQueryUnitIndex(); pIndex; pIndex = pIndex->GetNextQueryUnitIndex())
			{ ShowIndex(pIndex->GetIndex()); }
		}
	}

	/************************МЕТОДЫ ДЛЯ ПОЛУЧЕНИЯ ЛИТЕРАЛЬНОГО ПРЕДСТАВЛНИЯ*****************************/

	///Получение литерального представления из морфо-индекса
	std::wstring CAdditionalTest::GetWordFromMorphoIndex(TDictionaryIndex oTDictionaryIndex) const
	{
		wchar_t Word[50];
		SS::Dictionary::Types::SWordIndex oWordIndex;
		oWordIndex.m_DictIndex.SetDictionaryIndex(oTDictionaryIndex.GetDictionaryIndex());
		SS::Interface::Core::Dictionary::IMorphologyAnalyser* pMorphologyAnalyser = NULL;
		if (!m_pDictionary)
		{ SS_THROW(L"Установите_LoadManager."); }
		pMorphologyAnalyser = m_pDictionary->GetMorphologyAnalyser();
		pMorphologyAnalyser->GetWord(&oWordIndex, Word);
		return Word;
	}
	
	///Получить литеральное представление из юнита
	std::wstring CAdditionalTest::GetWordFromUnit(IUnit* pUnit) const
	{
		IIndex*				pIndex = NULL;
		IDictionaryIndex*	pDictionaryIndex = NULL;
		TDictionaryIndex	oTDictionaryIndex;
		std::wstring		wsWord;

		wsWord = pUnit->GetWord();
		if (!wsWord.empty()) return wsWord;

		//Если нет готового литерального предсталения для юнита
		for (pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
		{
			//Получение IDictionaryIndex
			pDictionaryIndex = pIndex->GetDictionaryIndex();
			if (!pDictionaryIndex) continue;
			//Получение TDictionaryIndex
			if ( !(pDictionaryIndex->GetFirst(&oTDictionaryIndex)) ) continue;
			//Если индекс морфологический, получить литеральное представление
			if (oTDictionaryIndex.GetIndexType() == eitMorphoIndex)
			{ 
				wsWord = GetWordFromMorphoIndex(oTDictionaryIndex);
				if (!wsWord.empty()) return wsWord;
			}
		}
		return L"Нет_литерального_представления";
	}

	///Получить литеральное представление из квери-юнита
	std::wstring CAdditionalTest::GetWordFromQueryUnit(IQueryUnit* pQueryUnit) const
	{
		IQueryUnitIndexCollection*	pCollection = NULL;
		IQueryUnitIndex*			pIndex = NULL;
		std::wstring				wsWord;

		wsWord = pQueryUnit->GetWord();
		if (!wsWord.empty()) return wsWord;

		//Если нет готового литерального предсталения для квери-юнита
		for (pCollection = pQueryUnit->GetFirstQueryUnitIndexCollection(); pCollection; pCollection = pCollection->GetNextQueryUnitIndexCollection())
		{
			//Должна быть коллекция морфоиндексов
			if (pCollection->GetIndexCollectionType() == eitMorphoIndex)
			{ 
				//Морфоиндексы
				for (pIndex = pCollection->GetFirstQueryUnitIndex(); pIndex; pIndex = pIndex->GetNextQueryUnitIndex())
				{
					wsWord = GetWordFromMorphoIndex(pIndex->GetIndex());
					if (!wsWord.empty()) return wsWord;
				}
			}
		}
		return L"Нет_литерального_представления";
	}

	/***************************ОБЩИЕ МЕТОДЫ**********************/

	///Вывод индекса
	void CAdditionalTest::ShowIndex(TDictionaryIndex oTDictionaryIndex) const
	{
		//Имя индекса
		std::wstring wsName;
				
		if (oTDictionaryIndex.GetIndexType() == eitMorphoIndex)
		{
			wsName = L"морфологический индекс: ";
		}
		else if (oTDictionaryIndex.GetIndexType() == eitSemanticIndex) 
		{
			wsName = L"семантический индекс: ";
		}
		else if (oTDictionaryIndex.GetIndexType() == eitSyntaxIndex)
		{
			wsName = L"синтаксический индекс: ";
		}
		else if (oTDictionaryIndex.GetIndexType() == eitSynonymIndex)
		{
			wsName = L"синонимический индекс: ";
		}
		else if (oTDictionaryIndex.GetIndexType() == eitCombinatoryIndex)
		{
			wsName = L"комбинаторный индекс: ";
		}
		else
		{
			wsName = L"неопределенный индекс: ";
		}

		wchar_t Buff[30];
		_itow(oTDictionaryIndex.GetDictionaryIndexWithoutFlags(), Buff, 10);
		SAVE_LOG((wchar_t*)wsName.c_str() AND Buff);
	}

}
}

#endif //_SS_UNITTESTS