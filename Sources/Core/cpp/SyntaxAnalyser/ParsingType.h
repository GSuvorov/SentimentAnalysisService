#pragma once

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{
			///Тип разбора
			enum ParsingType
			{
				///Неопределенный
				ptUndefined = -1,
				///Разрешение омонимии (английский)
				ptMorphoResolveEnglish, 
				///Разбор запроса (английский)
				ptQueryAnalyzeEnglish,
				///Полный разбор (английский)
				ptFullParsingEnglish,
				///Разрешение омонимии (русский)
				ptMorphoResolveRussian, 
				///Разбор запроса (русский)
				ptQueryAnalyzeRussian,
				///Полный разбор (русский)
				ptFullParsingRussian
			};
		}
	}
}