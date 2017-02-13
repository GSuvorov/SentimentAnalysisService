#pragma once



namespace SS
{
	namespace Core
	{
		namespace Features
		{
			/*	кодирование/декодирование синтаксической информации	*/
			class CSyntaxAnalyzer
			{
			public:
				/// конструктор
				CSyntaxAnalyzer(void);
				/// деструктор
				virtual ~CSyntaxAnalyzer(void){};
			public: //from SS::Core::SMISL::IAMConverterSyntax	
			/// Кодирует набор априорных синтаксических харктеристик в число
			/** 
				\param oPartOfSpeechTypes - часть речи слова, для которого необходимо выполнить кодирование
				\param *pSyntaxFeature	- набора априорных синтаксических характеристик
				\param piSyntaxInfo	- число, которое содержит закодированные априорные синтаксические характеристики
				\return void
			*/
				HRESULT Code(
					/*in*/Types::PartOfSpeechTypes oPartOfSpeechTypes,
					/*in*/SS::Core::Features::CSyntaxFeatureApriori *pSyntaxFeature, 
					/*out*/unsigned int* iSyntaxInfo);
			/// Выполняет декодирование числа в набор априорных синтаксических характеристик 
			/** 
				\param oPartOfSpeechTypes - часть речи слова, синтаксические характеристики которого необходимо получить
				\param iSyntaxInfo - закодированная синтаксическая характеристика
				\param *pSyntaxFeature - набор синтаксических категорий 
				\return void
			*/
				HRESULT EnCode(
				/*in*/ Types::PartOfSpeechTypes oPartOfSpeechTypes,
				/*in*/unsigned int iSyntaxInfo,	
				/*out*/SS::Core::Features::CSyntaxFeatureApriori *pSyntaxFeature);
			};
		};
	};	
};