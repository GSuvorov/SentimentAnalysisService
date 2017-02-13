#pragma once

namespace SS
{
	namespace Semantic
	{


	enum CommonFeature
	{
		efcNon							=0,
		efcRepresentBecome				=0x01,//1 содержит семантику represent or become
		efcSemanticVerb					=0x02,//3 глагольная семантика
		efcSemanticCommon				=0x04,//4 не глагольная (общая) семантика
		efcSemanticName					=0x08,//5 семантика имени
		efcSemanticMeasure				=0x10,//7 семантика меры
		efcNoun							=0x20,//2 содержит существительное
		efcAdverbAdjective				=0x40,//8 содержит наречие и/или прилагательное
		efcVerb							=0x80,//10 содержит глагол
		efcPretext						=0x0100,//6 предлог
		efcVerbWithPrep					=0x0200, //11 проверенная комбинация глагола с мредлогом
		efcDetectedNoun					=0x0400,//9 определено - что это слово существительное
		efcSemanticCitation				=0x0800,// семантика цитаты
	};
///признаки
	enum ERusFeature
	{
		efRNon							=0,
		efRCaseNominative				=0x01,//1 содержит именительный падеж
		efRCaseInstrumental				=0x02,//11 творительный падеж
		efRCaseAccusative				=0x04,//12 винительный падеж
		efRCaseGenetive					=0x08,//14 родительный падеж
		efRCaseLocative					=0x0100, //25 предложный
		efRCaseDative					=0x0200,  //26 дательный падеж
		efRCaseAll						=0x0400, //20 слово содержит все падежи, те не изменяемая форма (для русского)
		efRAnimate						=0x0800,//18 содержит одушевлённость
		efRPassive						=0x1000,//19 содержит пассив (личная форма в страдательном залоге или краткое страдательное причастие)
		efRVerbDeepr					=0x2000,  //28 деепричастие
	};
	enum EEngFeature
	{
		efENon							=0,
		efEDetPronounAdjective			=0x01,//3 определитель или местоимение pronounAdjective
		efEClearProper					=0x02,//4 все индексы из словаря имён собственных
		efENotOnlyNoun					=0x04,//6 содержит не только существительное
		efECaseGenetive					=0x08,//14 родительный падеж
		efEDNounPrep					=0x10, //21 существительное (англ)
		efEDNounNominative				=0x20, //22 существительное без предлога (англ)
		efEDNounPrepOF					=0x40, //23 существительное с предлогом of (англ)
		efEVerbParticiplePresent		=0x80,  //27 причастие настоящего времени
		efEVerbInfinitive				=0x1000  //29 деепричастие
	};

	///расширенный тип слова(юнита)
	enum TSpecialUnitType{
		ustUndefined,
		ustEndSentence,	//слово - конец предложения
		ustInitials,	//инициалы
		ustFamily,		//фамилия
		ustMeasureUnit, //единица измерения
		ustShortering,  //сокращение типа т.п.
		ustPunkt,		//пункт в начале строки
		ustWordCombination,		//комбинация типа 0 
		ustWordCombinationLex,		//комбинация типа New lexical unit 
		ustURL,			//url	
		ustDate,			//дата 
		ustTime,			//время
		ustAbbreviation,//аббривиатура
		ustAcronim,		//акроним
		ustNumeral,		//числительные
		ustComplexProperName,	//составное имя собственное
		ustAux,
		ustCitation,				//цитата
		ustPlaceSpecification,	//уточнения места типа - город,страна
		ustProperGroup,			//группа слов с большой буквы не объединённая ранее до самого конца разбора
		ustNewWordContainer		//юнит содержит слова для вставки
	};
	///// синтаксический тип 
	//enum TSyntaxUnitType{
	//	usntUndefined,
	//	usntQuestionGroup,	/// вопросительная группа
	//	usntEntity1,		/// первая сущность
	//	usntEntity2,		/// вторая сущность
	//	usntVerb,			/// глагол
	//	usntPrepositionGroup,	/// 
	//	usntMeasure,		/// обстоятельство меры
	//	usntComplexWord,		
	//	usntDateSyntax,			/// обстоятельство времени
	//	usntTimeSyntax,			///время  с предлогом
	//	usntComplexProperNameSyntax,	///составное имя собственное с предлогом
	//	usntSubjectComplement,			/// дефиниции
	//	usntObjectSubject				/// субъект/объект 
	//};


	}
}