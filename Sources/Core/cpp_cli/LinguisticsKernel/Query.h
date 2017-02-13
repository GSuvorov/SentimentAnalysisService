#pragma once


#include "..\..\cpp\ASCInterface/ILinguisticProcessor.h"


#include "..\..\cpp\ASSInterface/ILinguisticProcessorEx.h"

#include "..\..\cpp\ASSInterface/ILexicaLib.h"
#include "..\..\cpp\ASSInterface/ISyntaxAnalyzer.h"
#include "..\..\cpp\ASSInterface/ISemanticAnalyze.h"
#include "..\..\cpp\ASSInterface/IRev.h"
#include "..\..\cpp\MainAnalyse\LinguisticQuery.h"

namespace SS
{
namespace LinguisticProcessor
{

class CQuery : public SS::LinguisticProcessor::CLinguisticQuery
{	
public:
	CQuery(void);
	~CQuery(void);
public:
	/// данная функция используется при поиске для получения запроса к индексному процессору по разобранному запросу 
	/**
	используется для подачи запроса к индексным процессорам, принимающим на вход стандартный контейнер запроса SS
	*/
	SS::Interface::Core::BlackBox::IText* GenerateQueryIText(
		/// параметры поиска
		SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams
		/// параметры анализа запроса
		, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
		);
public:
	/// данная функция используется при поиске для получения запроса к индексному процессору по разобранному запросу 
	/**
	используется для подачи запроса к индексным процессорам, принимающим на вход стандартный контейнер запроса SS
	*/
	SS::Interface::Core::BlackBox::Query::IQuery* GenerateQueryIQuery(
		/// параметры поиска
		SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams
		/// параметры анализа запроса
		, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
		);
};
}
}