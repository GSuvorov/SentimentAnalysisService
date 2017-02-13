#pragma once

namespace SS
{
namespace Core
{
namespace Types
{
	
	///режим анализа
	enum EAnalyseMods
	{
		///индексация
		eamIndexation = 0,
		///анализ запроса
		eamQuery = 1,
		///поиск по ключевым словам
		eamStrictQuery,
		///поиск по ключевым словам
		eamOnlyKeyWords,
		///анализ запроса c генерацией поисковой строки
		eamQueryAndSearchString,
		/// анализ запроса с ослабленными критериями
		eamRejectUnimportant,
		/// поиск ответа на вопрос
		eamSearch
	};

	enum ERelevanceParams
	{
		erpUndefine,
		erpCalculateIndexesWeight 
	};

	enum ECurrentSystemMode
	{
		ecsmLocal,
		ecsmGlobal
	};

	enum ESyntaxAnalysisMode
	{
		esamFull,
		esamFullWithoutSyntaxTree,
		esamMorphologyAmbiguityResolve
	};
	


}
}
}
