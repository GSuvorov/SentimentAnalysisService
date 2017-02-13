#pragma once

#include "./IBlackBoxItems.h"
#include "./IBlackBoxQuery.h"
#include "./IMainAnalyse.h"
#include "./ICommon.h"

// {457886C5-DAB8-4ca1-A457-6FA06F047D65}
static const GUID CLSID_RevModule = 
{ 0x457886c5, 0xdab8, 0x4ca1, { 0xa4, 0x57, 0x6f, 0xa0, 0x6f, 0x4, 0x7d, 0x65 } };

// {D348104C-9D71-4825-B8D1-92CB45F25EDC}
static const GUID IID_RelevanceEvaluation = 
{ 0xd348104c, 0x9d71, 0x4825, { 0xb8, 0xd1, 0x92, 0xcb, 0x45, 0xf2, 0x5e, 0xdc } };


// {BA68B0FA-8142-4623-ABF9-832FF88794D1}
static const GUID CLSID_LinguisticRelevance = 
{ 0xba68b0fa, 0x8142, 0x4623, { 0xab, 0xf9, 0x83, 0x2f, 0xf8, 0x87, 0x94, 0xd1 } };

// {082AD17C-427E-4763-B1FA-EE5559BC3A5F}
static const GUID IID_LinguisticRelevance = 
{ 0x82ad17c, 0x427e, 0x4763, { 0xb1, 0xfa, 0xee, 0x55, 0x59, 0xbc, 0x3a, 0x5f } };


namespace SS
{
namespace Interface
{
namespace Core
{
namespace Rev
{
	///интерфейс предоставляет функциональность по оценки релевантности запроса предложениям
	class ILinguisticRelevance : 
		public virtual SS::Interface::IBase,
		public virtual SS::Interface::Core::Common::CAnalyseParams
	{
	public:
		virtual ~ILinguisticRelevance(){};
	public:
		///выставляет текущий запрос
		/** 
			\param pQuery контейнер с трансформациями запроса
		*/
		virtual void SetQuery(SS::Interface::Core::BlackBox::Query::IQuery* pQuery) = 0;

		///оценивает входной запрос на наличие слов ответа
		virtual bool IsExistAnswerWord(SS::Interface::Core::BlackBox::IText* pText) = 0;

		///оценивает входной запрос и выполняет обновление релевантности
		/** 
			\return __int32 значение релевантности
		*/
		virtual __int32 UpdateRelevance(
			SS::Interface::Core::BlackBox::IText* pText, 
			SS::Interface::Core::BlackBox::IText* pTextTitle) = 0;

		///подсвечивает слова, имеющие совпадающие морфоиндексы со словами из первой трансформации запроса
		virtual void HighlightKeywords(SS::Interface::Core::BlackBox::IText* pText) = 0;

	};


	///интерфейс предоставляет функциональность по оценки релевантности запроса предложениям
	class IRelevanceEvaluation 
		: public virtual SS::Interface::IBase
		, public virtual SS::Interface::Core::Common::CAnalyseParams
	{
	public:
		virtual ~IRelevanceEvaluation(){};
	public:
		///оценивает входной запрос
		/** 
			\param pText результат анализа запроса
			\param pQuery выходной контейнер с трансформациями запроса
			\param pAnalyseParams текущие параметры системы
		 
		*/
		virtual HRESULT EvaluateQuery(
			SS::Interface::Core::BlackBox::IText* pText, 
			SS::Interface::Core::BlackBox::Query::IQuery* pQuery) = 0;
	};

}
}
}
}
