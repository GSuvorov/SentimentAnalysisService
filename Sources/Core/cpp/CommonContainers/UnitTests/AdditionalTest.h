//AdditionalTest.h
/*Файл содержит дополнительные глобальные
функции и классы для тестирования модуля*/
#pragma once

//Если тестирование
#ifdef _SS_UNITTESTS

#include "../../ASSInterface/IBlackBoxItems.h"
#include "../../ASSInterface/IDictionaryManager.h"
#include "../../ASSInterface/IBlackBoxQuery.h"
#include "../../ASSInterface/IAMConverter.h"

namespace SS
{
namespace UnitTests
{

	///Пространство имен для BlackBox
	using namespace SS::Interface::Core::BlackBox;
	using namespace SS::Interface::Core::BlackBox::Query;
	using SS::Dictionary::Types::TDictionaryIndex;

	///Проверка IText-а
	class CAdditionalTest
	{
	private:
		///Хранилище юнитов
		std::vector<IUnit*> m_UnitCollection;
		///Тип итератор хранилища
		typedef std::vector<IUnit*>::iterator TIterator;
		///Словарь
		SS::Interface::Core::Dictionary::IDictionary*			m_pDictionary;
		///Морфо конвертер
		SS::Interface::Core::AMConverter::IAMConverterMorpho*	m_pAMConverterMorpho;
		///Текстовое представление юнита
		std::wstring m_wsWord;
		///Анализировать одно слово
		bool m_bOneWord;

	private:
		///ДЛЯ IText
		///Заполнить коллекцию юнитов
		void GetUnitCollection(IText* pText);
		///Убрать лишние юниты, если нужен только один
		void UnitCollectionFilter();
		///Вывод текстового значения юнита
		void ShowInfoFromTextUnit(IUnit* pUnit) const;
		///Вывод информации о IFeatures для каждого IIndex
		void ShowFeaturesInfo(IIndex* pIndex) const;
		///Вывод синтаксического флага юнита
		void ShowSyntaxFlag(IUnit* pUnit) const;
		///Вывод синтаксической категории юнита
		void ShowSyntaxCategory(IUnit* pUnit) const;
		///Вывод данных из морфологических фич юнита
		void ShowMorphoFeature(IIndex* pIndex) const;
		
		///ДЛЯ IQuery
		///Вывод информации из квери-юнита
		void ShowInfoFromQueryUnit(IQueryUnit* pQueryUnit) const;
		
		///ДЛЯ ПОЛУЧЕНИЯ ЛИТЕРАЛЬНОГО ПРЕДСТАВЛЕНИЯ
		///Получение литерального представления из индекса
		std::wstring GetWordFromMorphoIndex(TDictionaryIndex oTDictionaryIndex) const;
		///Получить литеральное представление из юнита
		std::wstring GetWordFromUnit(IUnit* pUnit) const;
		///Получить литеральное представление из квери-юнита
		std::wstring GetWordFromQueryUnit(IQueryUnit* pQueryUnit) const;

		///Вывод индекса
		void ShowIndex(TDictionaryIndex oTDictionaryIndex) const;
	
	public:
		///Конструктор
		CAdditionalTest() : m_pDictionary(NULL), m_pAMConverterMorpho(NULL), m_bOneWord(false){};
		///Выполнить проверку текста
		void AnalyseText(IText* pText);
		///Выполнить проверку запроса
		void AnalyseQuery(IQuery* pQuery);
		///Установить менеджер (инициализировать m_pDictionary)
		void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
		///Установить слово для анализа
		void SetOneWordAnalyse(std::wstring s) { m_wsWord = s; m_bOneWord = true; };
		///Сброс однословного анализа(вернуться к обычному анализу)
		void ResetOneWordAnalyse() { m_bOneWord = false; };
	};

}//UnitTests
}//SS

#endif //_SS_UNITTESTS