#pragma once

#include "ICommon.h"
#include "./TTextsComparers.h"

// {AF09E308-3873-4368-A7DA-914C4EAF3250}
static const GUID CLSID_ShingleComparer = 
{ 0xaf09e308, 0x3873, 0x4368, { 0xa7, 0xda, 0x91, 0x4c, 0x4e, 0xaf, 0x32, 0x50 } };

// {DAF1429B-C736-44f0-AA1E-AD125C379C4E}
static const GUID IID_ShingleComparer = 
{ 0xdaf1429b, 0xc736, 0x44f0, { 0xaa, 0x1e, 0xad, 0x12, 0x5c, 0x37, 0x9c, 0x4e } };


// {1563C86E-E220-418e-8C81-97DF1B713962}
static const GUID CLSID_FuzzyComparer = 
{ 0x1563c86e, 0xe220, 0x418e, { 0x8c, 0x81, 0x97, 0xdf, 0x1b, 0x71, 0x39, 0x62 } };

// {5E55E786-AA09-41f6-885F-12DAA194213C}
static const GUID IID_FuzzyComparer = 
{ 0x5e55e786, 0xaa09, 0x41f6, { 0x88, 0x5f, 0x12, 0xda, 0xa1, 0x94, 0x21, 0x3c } };



// {4C457939-E254-432a-AD5C-32CAE67B5934}
static const GUID CLSID_StatisticsComparer = 
{ 0x4c457939, 0xe254, 0x432a, { 0xad, 0x5c, 0x32, 0xca, 0xe6, 0x7b, 0x59, 0x34 } };

// {4488ED65-6FA5-444d-A7CA-119D54D663CE}
static const GUID IID_StatisticsComparer = 
{ 0x4488ed65, 0x6fa5, 0x444d, { 0xa7, 0xca, 0x11, 0x9d, 0x54, 0xd6, 0x63, 0xce } };



namespace TextsComaparers
{

class IShingleComparer : 
		virtual public SS::Interface::IBase,
		virtual public SS::Interface::Core::Common::CAnalyseParams
{
public:
	virtual double Compare(LPCWSTR szText1, LPCWSTR szText2, int iShingleSize) = 0;
};

class IFuzzyComparer : 
		virtual public SS::Interface::IBase,
		virtual public SS::Interface::Core::Common::CAnalyseParams
{
public:
	virtual double Compare(LPCWSTR szText1, LPCWSTR szText2, int iSequenceSize, bool bByCount, int iErrCount) = 0;
};

// Класс вызвающий методы рубрикатора
class IStatisticsComparer : virtual public SS::Interface::IBase
{
	public:

		/// Выполняет обучение на входной выборке TXT-файлов по пути <wzPathToFiles>.
		virtual void Learn(/*IN*/ const wchar_t * wzPathToFiles) = 0;

		/// Выполняет обучение или текстам указанным в <pSampleTexts>.
		virtual void Learn(/*in*/TSampleTexts * pSampleTexts) = 0;

		/// Возвращает полные пути к файлам рубрик
		virtual void GetRubricFilePaths(/*out*/ list< wstring > * pFilePathsList) = 0;

		/// Выполняет рубрикацию текста wzAnalyseTexts
		virtual void Classify(/*in*/const wchar_t * wzAnalyseTexts, /*out*/ list< TTextEstimate > * pTextsScore) = 0;

		/// Возвращает значимые комбинации из текущего текста в виде текста в котором комбинации разделены точками и переносами строк.
		virtual void GetTextCombination(/*in*/const wchar_t * wzAnalyseTexts, /*out*/ wstring * pwText) = 0;

		/// Возвращает значимые комбинации из текущего текста в виде списка строк комбинаций, которые заканчиваются точками без переноса строк.
		virtual void GetTextCombination(/*in*/const wchar_t * wzAnalyseTexts, /*out*/ list< wstring > * pwTextList) = 0;
};

}
