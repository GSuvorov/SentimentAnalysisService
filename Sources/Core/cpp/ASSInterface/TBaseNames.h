#pragma once

namespace SS
{
	namespace MANAGER
	{
		namespace Constants
		{

			/// тип базы - поисковая (хранит индексы для поиска)
			static const wchar_t* c_szSearchBase=L"SearchBase";
			/// тип базы - сервисная (хранит образы документов)
			//static const wchar_t* c_szServiceBase="ServiceBase";
			/// тип базы - поисковая (хранит индексы для поиска)
			static const wchar_t* c_szNdxBase=L"NdxBase";
			/// тип базы - русская морфологическая
			static const wchar_t* c_szMorphoRus=L"MorphoRussian";
			/// тип базы - английская морфологическая
			static const wchar_t* c_szMorphoEng=L"MorphoEnglish";
			/// тип базы - комбинаторная, синтактико-семантическая русская
			static const wchar_t* c_szCoSeDiRus=L"CoSeDiRussian";			
			/// тип базы - комбинаторная, синтактико-семантическая английская
			static const wchar_t* c_szCoSeDiEng=L"CoSeDiEnglish";
			/// тип базы -  база синтаксических конструкций английская
			static const wchar_t* c_szSyntaxConstructionsEng=L"SyntaxConstructionsEnglish";
			/// тип базы - база синтаксических конструкций русская
			static const wchar_t* c_szSyntaxConstructionsRus=L"SyntaxConstructionsRussian";
			/// тип базы - база синтаксических конструкций русская для Natural Query
			static const wchar_t* c_szSyntaxConstructionsRus_NQ = L"SyntaxConstructionsRussian_NaturalQuery";
			/// тип базы -  база семнтических конструкций английская
			static const wchar_t* c_szSemanticConstructionsEng=L"SemanticConstructionsEnglish";
			/// тип базы - база семнтических конструкций русская
			static const wchar_t* c_szSemanticConstructionsRus=L"SemanticConstructionsRussian";
			/// тип базы - для скачанных из интернета файлов
			static const wchar_t* c_szDownloadInfo=L"DownloadInfo";
			/// тип базы - аббревиатур английская
			static const wchar_t* c_szAbbreviationEng =L"AbbreviationEnglish";
			/// тип базы - аббревиатур русская
			static const wchar_t* c_szAbbreviationRus =L"AbbreviationRussian";
		}
	}
}