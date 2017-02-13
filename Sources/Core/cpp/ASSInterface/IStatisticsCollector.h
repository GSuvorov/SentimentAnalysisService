#pragma once

#include ".\TStatisticsCollector.h"
#include ".\IBlackBox.h"
#include ".\ICoSeDi.h"
#include "..\ASCInterface\IInterface.h"
#include "..\ASCInterface\ILinguisticProcessor.h"

// --------------------------------------------------------------------------------
/// Идентификатор класса StatisticsCollector
static const GUID CLSID_IStatistics = { 0x33435295, 0x6ae1, 0x4703, { 0x91, 0xc0, 0x3c, 0xbe, 0xe2, 0x9c, 0x5f, 0x33 } };

static const GUID IID_IStatistics   = { 0x34085ec0, 0xa10a, 0x4dcb, { 0xac, 0xa3, 0xb9, 0x8c, 0xb5, 0x6e, 0xa6, 0xd8 } };
// ------------------------------------------------------------------------------------------------------------------------------


#define POST_nFree0                0x00
#define POST_nVerbs                0x01
#define POST_nNouns                0x02
#define POST_nAdjectives           0x03
#define POST_nNumerals             0x04
#define POST_nNumbers              0x05
#define POST_nAdverbs              0x06
#define POST_nPronouns             0x07
#define POST_nPrepositions         0x08
#define POST_nConjunctions         0x09
#define POST_nParticles            0x0A
#define POST_nInterjections        0x0B
#define POST_nPredicatives         0x0C
#define POST_nSymbols              0x0D
#define POST_nFree1                0x0E
#define POST_nFree2                0x0F
#define POST_nFree3                0x10
#define POST_nVerbsActive          0x11
#define POST_nVerbsPassive         0x12
#define POST_nVerbsTransitive      0x13
#define POST_nVerbsIntransitive    0x14
#define POST_nVerbsPast            0x15
#define POST_nVerbsPresent         0x16
#define POST_nVerbsFuture          0x17
#define POST_nVerbsUndefTime       0x18
#define POST_nVerbsParticiple      0x19
#define POST_nVerbsDeepr           0x1A
#define POST_nVerbsPersonal        0x1B
#define POST_nVerbsInfinitive      0x1C
#define POST_nVerbsImperative      0x1D
#define POST_nVerbsActiveIndef     0x1E
#define POST_nVerbsTransitIndef    0x1F
#define POST_nFree4                0x20
#define POST_nNounsNominal         0x21
#define POST_nNounsProper          0x22
#define POST_nNounsAnimate         0x23
#define POST_nNounsInanimate       0x24
#define POST_nNounsUndefAnimate    0x25


namespace SS
{

namespace Interface
{
namespace Statistics
{
	/// Интерфейс для подсчета статистики по тексту
	class IStatisticsCollector
	{
		public:

			/// выполняет подготовку к обработке нового текста
			virtual void NextText(void) = 0;

			/// выполняет сбор статистических данных со следующего блока текста
			virtual void AnalyseNext(/*in*/SS::Interface::Core::BlackBox::IText * pText) = 0;

			/// возвращает набор статистических параметров
			virtual void GetTextStatistics(/*in out*/SS::Statistics::Types::IStatisticsParameters* pStatisticsParameters) = 0;

			///// возвращает список редко встечающихся слов
			//virtual void GetUnCommonWords(/*out*/SSymbolsSequence * poListOfWordsInfo) = 0;

			/// Возвращает список символов : nSymbol // Число символов ( ! " , . - )
			//virtual void GetSymbols(/*out*/std::list< std::string > * poListOfSymbols) = 0;

			///// Возвращает список символьных биграмм и триграмм
			//virtual void GetBigrammsDuadsAndTrigramms(/*out*/SSymbolsSequence * poListOfPolygrammsInfo) = 0;
	};

	/// Интерфейс для подсчета последовательностей символов в тексте
	class ISymbolsSequenceCollector
	{
		public:

			/// выполняет подготовку к обработке нового текста
			virtual void Init(/*in*/SS::Statistics::Types::TNativeSymbols* pNativeSymbols) = 0;

			/// выполняет сбор статистических данных со следующего блока текста
			virtual void AnalyseNext(/*in*/SS::Interface::Core::ILexemeBoard* pLexemeBoard) = 0;

			/// число встреч символьной последовательности, заданной в Init
			virtual void GetSymbolsStatistics(/*out*/SS::Statistics::Types::TResultType* pResultType) = 0;

	};

	/// Интерфейс для создания частотных словарей слов
	class IWordFrequencyCollector
	{
		public:

			/// Выполняет подготовку к новому сбору статистики
			virtual void Init(void) = 0;

			/// Выполняет сбор данных по частоте встречаемости слов
			virtual void AnalyseNext(IN SS::Interface::Core::BlackBox::IText * pText) = 0;

			/// Устанавливает набор статистических параметров
			virtual void SetWordFrequencyStatistics(IN SS::Statistics::Types::IWordFrequencyParameters * pWordFrequencyParameters) = 0;

			/// Возвращает набор статистических параметров
			virtual void GetWordFrequencyStatistics(OUT SS::Statistics::Types::IWordFrequencyParameters * pWordFrequencyParameters) = 0;

	};

	/// Интерфейс для создания частотных словарей слов
	class ISemanticsCollector
	{
		public:

			/// Выполняет подготовку к новому сбору статистики
			virtual void Init(void) = 0;

			/// Выполняет сбор данных по частоте встречаемости слов
			virtual void AnalyseNext(IN SS::Interface::Core::BlackBox::IText * pText) = 0;

			/// Устанавливает набор статистических параметров
			virtual void SetSemanticsStatistics(IN SS::Statistics::Types::ISemanticsParameters * pSemanticsParameters) = 0;

			/// Возвращает набор статистических параметров
						/**
				\param  IN   pClassesList - список семантических классов
				\param  OUT  pSemanticsParameters - структура, содержащая слова, относящиеся к заданным сем. классам
			*/
			virtual void GetSemanticsStatistics(IN std::list<TYPE_DictIndex> * pClassesList, OUT SS::Statistics::Types::ISemanticsParameters * pSemanticsParameters) = 0;

	};

	/// Интерфейс контент-анализа
	class IContentAnalysis
	{
		public:

			/// Выполняет подготовку к новому сбору статистики
			virtual void Init(void) = 0;

			/// Выполняет сбор данных по частоте встречаемости слов
			virtual void AnalyseNext(IN SS::Interface::Core::BlackBox::IText * pText) = 0;

			/// Коэффициент Юла
			virtual double GetIndexYule(  IN std::list<TYPE_DictIndex> * pPositiveA,         IN std::list<TYPE_DictIndex> * pPositiveB,         UINT iWordDistance = 10) = 0;
			virtual double GetIndexYule(  IN           TYPE_DictIndex    iPositiveSemClassA, IN           TYPE_DictIndex    iPositiveSemClassB, UINT iWordDistance = 10) = 0;

			/// Коэффициент Яниса
			virtual double GetIndexIanis( IN std::list<TYPE_DictIndex> * pPositiveList,      IN std::list<TYPE_DictIndex> * pNegativeList) = 0;
			virtual double GetIndexIanis( IN           TYPE_DictIndex    iPositiveSemClass,  IN           TYPE_DictIndex    iNegativeSemClass) = 0;

			/// Формула корреляции Пирсона
			virtual double GetIndexPirson(IN std::list<TYPE_DictIndex> * pPositiveA,         IN std::list<TYPE_DictIndex> * pPositiveB,         UINT iWordDistance = 10) = 0;
			virtual double GetIndexPirson(IN           TYPE_DictIndex    iPositiveSemClassA, IN           TYPE_DictIndex    iPositiveSemClassB, UINT iWordDistance = 10) = 0;

			///// Индекс рефлексивности
			//virtual double GetIndexReflection(void) = 0;

			/// Получение поддерева сем. класа iSemClass с маркерами
			virtual bool GetClassSubTreeWithMarkers(IN TYPE_DictIndex iSemClass, OUT SS::Statistics::Types::ISemClassSubTreeWithMarkers * pIStruct) = 0;

			/// Получение указателя на восстановленный текст с пометкой маркеров (после работы <GetClassSubTreeWithMarkers()>)
			virtual const std::wstring * GetTextWithMarkers(void) = 0;

	};

	/*! \interface IStatisticsCollector
	*   \brief     Интерфейс сборщика статистики
	*/
	class IStatistics : public SS::Interface::IBase   
	{
		public:

			/// возвращает интерфейс для подсчета статистики по тексту
			virtual IStatisticsCollector * GetStatisticsCollector() = 0;

			/// возвращает интерфейс для подсчета последовательностей символов в тексте
			virtual ISymbolsSequenceCollector * GetSymbolsSequenceCollector() = 0;

			/// возвращает интерфейс для создания частотных словарей слов
			virtual IWordFrequencyCollector * GetWordFrequencyCollector() = 0;

			/// возвращает интерфейс для подсчета слов, относящихся к семантическим классам
			virtual ISemanticsCollector * GetSemanticsCollector() = 0;

			/// возвращает интерфейс контент-анализа
			virtual IContentAnalysis * GetContentAnalysis() = 0;

			///// задание менеджера загрузки 
			//virtual void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)   {
			//	CBaseCoreClass::SetLoadManager(pLoadManager);
			//};

	};
}
}
}
