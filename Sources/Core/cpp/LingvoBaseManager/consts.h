#pragma once

namespace SS
{

	namespace MANAGER
	{

		namespace Constants
		{

			///// тип базы - поисковая (хранит индексы для поиска)
			//static const wchar_t* c_szSearchBase=L"SearchBase";
			///// тип базы - сервисная (хранит образы документов)
			////static const wchar_t* c_szServiceBase="ServiceBase";
			///// тип базы - поисковая (хранит индексы для поиска)
			//static const wchar_t* c_szNdxBase=L"NdxBase";
			///// тип базы - русская морфологическая
			//static const wchar_t* c_szMorphoRus=L"MorphoRus";
			///// тип базы - английская морфологическая
			//static const wchar_t* c_szMorphoEng=L"MorphoEng";
			///// тип базы - комбинаторная, синтактико-семантическая русская
			//static const wchar_t* c_szCoSeDiRus=L"CoSeDiRus";			
			///// тип базы - комбинаторная, синтактико-семантическая английская
			//static const wchar_t* c_szCoSeDiEng=L"CoSeDiEng";
			///// тип базы -  база синтаксических конструкций английская
			//static const wchar_t* c_szSyntaxConstructionsEng=L"SyntaxConstructionsEnglish";
			///// тип базы - база синтаксических конструкций русская
			//static const wchar_t* c_szSyntaxConstructionsRus=L"SyntaxConstructionsRussian";
			///// тип базы -  база семнтических конструкций английская
			//static const wchar_t* c_szSemanticConstructionsEng=L"SemanticConstructionsEnglish";
			///// тип базы - база семнтических конструкций русская
			//static const wchar_t* c_szSemanticConstructionsRus=L"SemanticConstructionsRussian";
			///// тип базы - для скачанных из интернета файлов
			//static const wchar_t* c_szDownloadInfo=L"DownloadInfo";
			///// тип базы - аббревиатур английская
			//static const wchar_t* c_szAbbreviationEng =L"AbbreviationEng";
			///// тип базы - аббревиатур русская
			//static const wchar_t* c_szAbbreviationRus =L"AbbreviationRus";

			/// группа баз DBMS (объединяет все базы, управляемые через DBMS) 
			static const wchar_t* c_szDBMS =L"DBMS";
			/// группа SQL - баз (объединяет все базы, управляемые через СУБД MSSQL)
			static const wchar_t* c_szSql =L"SQL";
		}
	}
}
