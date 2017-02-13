#pragma once

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			class CSyntaxAposterioriAnalyzer
			{
			public:
				/// конструктор
				CSyntaxAposterioriAnalyzer(void);
				/// деструктор
				~CSyntaxAposterioriAnalyzer(void);
			public:
				/// кодирует набор апосториорных синтаксических харктеристик в число
				/** 
					\param *pSyntaxFeatureAposteriori - набор апосториорных синтаксических характеристик
					\param piSyntaxInfo - число, которое содержит закодированные апосториорные синтаксические характеристики
				*/
				void Code(/*in*/SS::Core::Features::CSyntaxFeatureAposteriori *pSyntaxFeatureAposteriori, 
					/*out*/unsigned int* piSyntaxInfo) ;
				/// Выполняет декодирование числа в набор априорных синтаксических характеристик 
				/** 
					\param iSyntaxInfo - число, которое содержит закодированные апосториорные синтаксические характеристики
					\param *pSyntaxFeatureAposteriori - набор апосториорных синтаксических характеристик
				*/
				void EnCode(
					/*in*/unsigned int iSyntaxInfo,	
					/*out*/SS::Core::Features::CSyntaxFeatureAposteriori *pSyntaxFeatureAposteriori);
			};
		};
	};
};