#pragma once

///перечисление, определяющее доступные модули системы	(динамически линкуемые библиотеки)
typedef enum TCoreLibrary{
	///словарь
	dllDictionary,
	///получение синонимов
	dllSynonims,
	///лексический анализатор
	dllLexicaLib,
	///индексация и поиск
	dllNdx,
	///конвертер характеристик
	dllAMConverter,
	///ТКС
	dllCoSeDi,
	///синтаксический анализатор
	dllSyntaxConstructions,
	///синтаксический анализатор
	dllSyntaxAnalyzer,
	///хранилище информации
	dllBlackBox,
	///главный модуль
	dllAISBroker,
	///менеджер анализа
	dllMainAnalyse,
	///модуль оценки релевантности
	dllRev,
	///фабрика характеристик
	dllFeatureFactory,
	///модуль оценки релевантности
	dllTransformGenerator,
	///модуль хранилищ данных индексной базы
	dllNdxDataStorage,
	///модуль управления базами
	dllBasesManager,
	///модуль управления загрузкой модулей
	dllLoadManager,
	///модуль общих сервисов
	dllCommonServices,
	///модуль хранилищ данных индексной базе Ndx
	dllNdxBaseManager,
	///модуль индексации и поиска в индексной базе Ndx
	dllNdxProcessor,
	///модуль общих контейнеров 
	dllCommonContainers,
	///модуль общих неуправляемых сервисов
	dllUMCommonServices,
	///модуль управления лингвистическими базами
	dllLingvoBaseManager,
	///новый модуль лексического анализа
	dllLexicalAnalyzer,
	///модуль семантического анализа
	dllSemanticAnalyzer,
	///модуль оценки лингвистической релевантности
	dllLinguisticRev,
	//тестовый модуль №1
	dllTest1,
	//тестовый модуль №2
	dllTest2,
	//сервер настроек
	dllGSServer,
	//модуль преобразования дат
	dllDataBaseTransformer,
	// модуль статистического анализа
	dllStatisticsCollector,
	// модуль статистического анализа текста
	dllTextStatisticsProcessor,
	// модуль серверного лога	
	dllServerLog,
	// модуль общих сервисных функций
	dllCommonService,
	// модуль контейнеров индексации-поиска
	dllISContainer,
	// модуль планировщика запросов
	dllNdxQuery,
	///база данных
	dllDBMS,
	/// html parser
	dllHtmlParser,
	///набор сессий счетчиков
	dllQPCounter,
	///модуль управления глобальными плагинами
	dllGSPluginManager
};
