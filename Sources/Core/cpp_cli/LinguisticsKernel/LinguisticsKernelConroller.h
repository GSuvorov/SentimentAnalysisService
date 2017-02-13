#pragma once

#include "TreeGenerator.h"
#include ".\treecomparator.h"
#include ".\IndexBaseInit.h"

#include "..\..\cpp\ASSInterface\IDictionaryManager.h"

SET_LOAD_MANAGER_DEFINE;

namespace LinguisticsKernel
{
	using namespace System;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::Data;
	using namespace System::IO;
	using namespace System::Windows::Forms;
	using namespace System::Xml::Linq;


	///данный класс управляет процессами тестового анализа и конвертацией данных для тестера
    public ref class LinguisticsKernelConroller
	{
        SS::Interface::Core::CommonServices::IWorkRegister* m_pIWorkRegister;

		/* интерфейсы лингвистического процессора */
		SS::Interface::Core::ILinguisticProcessor*	m_pLinguisticProcessor;

		SS::Interface::Core::Dictionary::IDictionary* m_pDictionary;

		SS::LinguisticProcessor::CSearch^			 m_pSearch;
		SS::LinguisticProcessor::CIndexation^		 m_pIndexation;
		SS::LinguisticProcessor::CQuery*			 m_pQuery;

		SS::Interface::Core::ILinguisticIndexation*	 m_pLinguisticIndexation;
		SS::Interface::Core::ILinguisticQuery*		 m_pLinguisticQuery;
		SS::Interface::Core::ILinguisticSearch*		 m_pLinguisticSearch;
		SS::Interface::Core::ILinguisticShowResults* m_pLinguisticShowResults;

		/*	интерфейсы конвертеров	*/
		SS::Interface::Core::AMConverter::IAMConverterSyntax* m_pAMConverterSyntax;
		SS::Interface::Core::AMConverter::IAMConverterMorpho* m_pAMConverterMorpho;

		SS::Core::Types::IndexationSearch::TQueryParams* m_pQueryParams;
		///генератор деревьев
		CTreeGenerator^ m_pTreeGenerator;
		///сравниватель деревьев
		CTreeComparator^ m_pTreeComparator;

		bool m_bIsInside;

		CIndexBaseInit* m_pIndexBaseInit;

		System::IO::StreamWriter^ logFile;

		SemanticResolution::CoreferenceResolution^    m_pCoreferenceResolution;
		SemanticResolution::SemanticNetwork^          m_pSemanticNetwork;
		SurfaceSyntaxAnalyzer::SurfaceSyntaxAnalyzer^ m_pSurfaceSyntaxAnalyzer;
		CoreferenceResolving::AbbreviationResolver^   m_pAbbreviationResolver;

		/// Флаг использования словаря геоназваний
		bool _IsUseGeoNamesDict;

	public:		
		enum class ETreeType
		{
			ettText,	
			ettQuery,	
			ettQueryFinal,
			ettTextIndexation
		};
		ref struct SSearchMode
		{
			bool bAnswer;
			bool bSynonyms;
		};

		LinguisticsKernelConroller(void);
		~LinguisticsKernelConroller(void);

		void Close();

		void DomainUnload(Object^ sender, EventArgs^ e);
		///индексирует текст и отрисовывает деревце зависимостей
		void GenerateTreeITextIndexation(String^ sText, TreeView^ pTree, bool bOutside, bool IsGlobal);
		///анализирует запрос и отрисовывает деревце зависимостей
		void GenerateTreeITextQuery(String^ sQuery, TreeView^ pTree, SSearchMode^ pSearchMode, bool IsGlobal);
		///анализирует запрос, формирует на его основе трансофрмации и отрисовывает деревцо трансформаций
		void GenerateTreeIQueryQuery(String^ sQuery, TreeView^ pTree, SSearchMode^ pSeaearchMode, bool IsGlobal);
		///сравнивает результаты разборов двух запросов
		void CompareQueries(String^ sLeftQuery, ETreeType LeftQueryTreeType, SSearchMode^ pLeftSearchMode, String^ sRightQuery, ETreeType RightQueryTreeType, SSearchMode^ pRightSearchMode, TreeView^ pComparationTree, TreeView^ pCoincidenceTree, bool bIsGlobal);
		///осуществляет поиск по запросу в тексте
		void Search(String^ sQuery, String^ sText, String^ sTextTitle, 
			DataSet^% pResultDataSet, ArrayList^% pArrayList, bool bIsGlobalSearch);
		///осуществляет тестирование интерфейса IShowResult
		void MakeShowResult(String^ sText, TreeView^ pTree);
		void SetParams(bool bIsMorpho, bool bIsSemantic, bool bIsSyntax, bool bIsSynonims, bool bIsInside);
		//ИНТЕРФЕЙС. Используется в автомате для определения тональности
		// получение дайджеста в формате XML. На вход - текст (sText), список синонимов в строке через \n (synonims), 
		// кореферентностей (korefs), массив смещений в тексте, которые могут относится к дайжесту (offsets),
		// тип объекта (sObjType)
		String^ GetXmlDigest(String^ sText, String^ synonims,
			String^ korefs, ArrayList^ offsets, String^ sObjType);
		//ИНТЕРФЕЙС. Используется в автомате для определения тональности с учетом тем
		 String^ GetSentXMLDigest(String^ sText, ArrayList^ offsets,
			 ArrayList^ themeIDs, String^ dictName);
		//ИНТЕРФЕЙС. Используется в визуальном приложении АПАОТ для фнкциональности настройки тональности
		// перегружает тональные словари по имени типа набора этих словарей.
		void ReinitToneDicts(String^ typeName);
		//ИНТЕРФЕЙС. Используется в визуальном приложении АПАОТ для фнкциональности настройки тональности
		// добавление нового слова в словарь. Задано имя набора словарей, код словаря (глаголы, дополнения и т.д.) и добавляемое слово
		void AddWordToDict(String^ typeName, int dict_code, String^ word);
 		//ИНТЕРФЕЙС. Используется в визуальном приложении АПАОТ для функциональности настройки тональности
		// генерирует пустой набор словарей
		void GenerateDictLing(String^ typeName);
		
		// Возвращает XML со списком словоформ с характеристиками для заданного слова.
        String^ GetAllWordFormsXML(String^ word);

		// Вовзращает массив словоформ слова
		List<String^>^ GetAllWordForms(String^ word);

		/// Возвращает массив словоформ слова и его части речи
		System::Tuple<List<String^>^,
			List<String^>^>^ GetAllWordFormsWithPartsOfSpeech(String^ word);

        // Возвращает нормальную форму слова
        String^ GetNormalWordForm(String^ word);

		// Возвращает все возможные нормальные формы
		List<String^>^ GetAllNormalWordForm(String^ word);
         
		/// Получение RDF документа
		/// afterSpellChecking - Флаг документа в виде XML после орфокоррекции
		/// baseDate - Базовая дата для разрешения относительных дат
		/// dictGeoNames - Флаг использования справочника геомест
		XElement^ GetRDF_New(String^ text, bool afterSpellChecking, System::DateTime baseDate, int selectEntitiesMode);
		Linguistics::Core::IText^ GetRDF_New(String^ text, System::DateTime baseDate);

		/// Получение семантической сети
		/// rdf - Документ в виде RDF
		/// generateAllSubthemes - Флаг добавления в список подсущностей
		System::Tuple<System::Collections::Generic::IEnumerable<Lingvistics::Types::ThemeItem^>^,
			System::Collections::Generic::IEnumerable<Lingvistics::Types::LinkItem^>^>^
			GetSemanticNetwork(XElement^ rdf, Linguistics::Coreference::ICoreferenceInfo^ coreferenceInfo, bool generateAllSubthemes);

		/// Фильтрация RDF
		void FilterRDF(XElement^% rdf);

		/// Проверка сущности на тему
		bool IsTheme(XElement^ entity);

		System::Tuple<XElement^, XElement^>^ ProcessPTS
			(String^ xml, bool isFullRDF, String^ language);

	protected:
		void Init();
		/// заполняем результаты поиска
		void FillDataSet(SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult, DataSet^% pResultDataSet, ArrayList^% pArrayList);

        /// загрузка служебных словарей
        void LoadDictionaries();

		/// установка значений из конфигурационного файла
		void SetConfigurationParams();

        /// загрузка служебного словаря
        void LoadDictionary(wstring path, vector<vector<wstring>> &dictionaryCollection);
		/// загрузка всех профилей служебного словаря стоп-прилагательных
		void LoadProfileAdjectives(wstring path);

		bool IsExistWord(String^ sText);
		///индексирует текст
		SS::Interface::Core::BlackBox::IText* GenerateITextIndexation(String^ sText, bool bOutside, bool IsGlobal);
		///анализирует запрос
		SS::Interface::Core::BlackBox::IText* GenerateITextQuery(String^ sQuery, SSearchMode^ pSearchMode, bool IsGlobal);
		///анализирует запрос
		SS::Interface::Core::BlackBox::Query::IQuery* GenerateIQueryQuery(String^ sQuery, SSearchMode^ pSearchMode, bool IsGlobal);

    private:
		delegate void EntitiesSelector(Linguistics::Core::Content^ content, String^ baseDate);

		Linguistics::Core::IText^ SelectEntities(String^ sText, String^ baseDate);
		void SelectEntitiesOnContent(Linguistics::Core::Content^ content, String^ baseDate, EntitiesSelector^ selector);
		void AddEntityListToContent(Linguistics::Core::Content^ content, List<SimpleEntities::SimpleEntity^>^ entities);

		System::Tuple<String^, Linguistics::Core::IText^, List<PTS::TextRange^>^>^
			ProcessPTSText(String^ xml, bool isFullRDF, String^ language, PTS::SimpleXmlParser^ parser);
		
		// Используется для получения сущностей по упрощенному алгоритму. На входе текст, на выходе
		// этот XML, содержащий разметку этого текста, где выделены предложения и сущности
		XElement^ GetXmlEntities(String^ sText);

		// Возвращает хэш по тексту
		String^ GetHash(String^ sEntText);

		// Возвращает нормальные формы всех слов предложения
        String^ GetTextAllNormalForms(String^ text);

		String^ TransliteringText(String^ srcText);

		String^ GetMorphoXML(String^ word);

		List<String^>^ GetTokens(String^ srcText);
		String^ GetMorphoInfo(String^ word);
	};
}