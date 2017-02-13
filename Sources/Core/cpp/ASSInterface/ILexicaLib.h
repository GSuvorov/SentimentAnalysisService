#pragma once

#include "..\assinterface\iblackbox.h"
#include "..\ascinterface\ilinguisticprocessor.h"

#include "IMainAnalyse.h"
#include "ICommon.h"


// {747F8F64-781E-474b-BB51-A516B1EEAFFE}
static const GUID CLSID_LexicaPrimary = 
{ 0x747f8f64, 0x781e, 0x474b, { 0xbb, 0x51, 0xa5, 0x16, 0xb1, 0xee, 0xaf, 0xfe } };

// {CAEFF0C4-D83B-4124-9254-8F0F4051EF5D}
static const GUID CLSID_Lexica = 
{ 0xcaeff0c4, 0xd83b, 0x4124, { 0x92, 0x54, 0x8f, 0xf, 0x40, 0x51, 0xef, 0x5d } };

// {FCEC0C30-CA10-4b05-9DB3-6056E769EB8C}
static const GUID IID_LexicaPrimary = 
{ 0xfcec0c30, 0xca10, 0x4b05, { 0x9d, 0xb3, 0x60, 0x56, 0xe7, 0x69, 0xeb, 0x8c } };

// {6DA5178C-46A3-485e-BF38-C5F0FA8E5F71}
static const GUID IID_Lexica = 
{ 0x6da5178c, 0x46a3, 0x485e, { 0xbf, 0x38, 0xc5, 0xf0, 0xfa, 0x8e, 0x5f, 0x71 } };


namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			namespace LexicalAnalyze
			{					
				///интерфейс управления лексическим анализатором
				class ILexica :
					virtual public SS::Interface::Core::ILexicaPrimary,
					virtual public SS::Interface::Core::Common::CAnalyseParams
				{
				public:
					virtual ~ILexica() {}
				public:
					/// анализ текста, содержащегося в подключенном хранилище лексем и запись результата в БлэкБокс
					virtual void AnalyzeDocStructure(SS::Interface::Core::BlackBox::IText* pText) = 0;
					/// анализ текста, содержащегося в подключенном хранилище лексем и запись результата в БлэкБокс
					virtual void AnalyzeNext(SS::Interface::Core::BlackBox::IText* pText, vector<vector<wstring>> *pParagraphs, bool isSimple = false, int mode = 1) = 0;
					
					virtual void AnalyzeNextSentence(SS::Interface::Core::BlackBox::IText* pText, bool isSimple = false, int mode = 1) = 0;
					/// заполняет синонимы
					virtual void FillSynonyms(SS::Interface::Core::BlackBox::IText* pText) = 0;
					/// установка семантического типа для юнита
					virtual void SetSemanticType(SS::Interface::Core::BlackBox::IUnit* pUnit, SS::Core::Features::Types::TSemanticType semanticType) = 0;
					/// Проверка, что юнит имеет заданный семантический тип
					virtual bool IsSemanticType(SS::Interface::Core::BlackBox::IUnit* pUnit, SS::Core::Features::Types::TSemanticType semanticType) = 0;
					///// установка параметров анализа
					//virtual void SetAfterSyntaxAnalyzeParams(SS::Interface::Core::MainAnalyse::IAfterSyntaxAnalyseParams* pAnalyseParams) = 0;
					///// проведение анализа после синтаксического анализа
					//virtual bool AnalyzeAfterSyntax(SS::Interface::Core::BlackBox::IText* pText)=0;
				};
			};
			

//			///Интерфейс модуля лексического анализа
//			class ILexicAnalyze : public virtual SS::Interface::IBase//,
////						public SS::SMInterface::IWorkWithManagerConnections
//			{
//			public:
//			////////////////////////////////////////////////////////////////////
//			/*ФУНКЦИИ ДЛЯ РАБОТЫ В СОСТАВЕ НОВОЙ СИСТЕМЫ*/
//				///инициализация процесса анализа текста
//				virtual void Init(
//					///параметры анализа
//					SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams, 
//					///текст для анализа
//					SS::Core::Types::IndexationSearch::TNativeText* pNativeText) = 0;
//
//				///анализ очередной части текста и занесение результата в pText
//				virtual bool AnalyseNext(
//					///это куда записывать результат
//					SS::Interface::Core::BlackBox::IText* pText) = 0;
//
//				/// возвращает строку по обработанному запросу
//				/** 
//					\param pQuery - контейнер содержащий разобранный заброс (результат в контейнере pQuery)
//					\return bool - истина - если не было ошибок
//				 
//				до вызова этой функции должен быть сделан вызов Init с параметром - анализ вопроса , затем 
//				AnalyseNext(...)
//				*/
//				virtual bool QueryToSearchString(SS::Interface::Core::BlackBox::IText* pQuery,
//					std::string * pSearchString)=0;
//
//				virtual bool SyntaxQuestionPostProcess(SS::Interface::Core::BlackBox::IText* pQuery)=0;
//
//				ILexicAnalyze(void){};
//				virtual ~ILexicAnalyze(){};
//
//			};

			/////данные о позиции слова в тексте
			//struct SWordPosition{
			//	///номер слова
			//	unsigned int nWordNumber;
			//	///начало
			//	unsigned int iBegin;
			//	///конец
			//	unsigned int iEnd;
			//};

			/////список структур SWordPosition
			//typedef list<SWordPosition> TWordPositionList;

			////////////////////////////////////////////////////////////////////////////
			/////Номер слова преобразует в позицию
			//class IWordPosition : public virtual SS::Interface::IBase
			//{
			//public:
			//	//////////////////////////////////////////////////////////////////////////
			//	///инициализация текстом перед получением номера символа
			//	virtual void Init(const char* szbuf) = 0;
			//	///получение номера символа по номеру слова
			//	virtual unsigned int GetPosition(unsigned int nWordNumber) = 0;

			//	///получение в структуру номеров символов начала и конца слова по переданным номерам слов
			//	virtual void GetPositions(wchar_t* pwchText, unsigned int iLength, TWordPositionList* p_list_WordPositions) = 0;

			//	IWordPosition(void){};
			//	virtual ~IWordPosition(){};

			//};

			/////сервисы для работы со словами для модулей интерфейса и плагинов
			//class IWordEncoding : public virtual SS::Interface::IBase
			//{
			//public:

			//	///инициализация текстом
			//	/**при этом текст по переданному указателю должен жить до конца анализа
			//	*/
			//	virtual void Init(const char* buff) = 0;
			//	///получение наличия слова
			//	virtual bool GetWord(char* szWord) = 0;
			//	///поиск вхождений слова
			//	/**
			//	\param pszWord искомое слово
			//	\param EntryPoints список найденных вхождений
			//	*/
			//	virtual void Find ( char *pszWord, std::list<int> *EntryPoints ) = 0;

			//	///поиск вхождений всех слов
			//	/**
			//	\param AllWords слова
			//	\param AllEntryPoints массив списков найденных вхождений
			//	*/
			//	virtual void FindAll( std::vector<string> AllWords, std::vector< std::list<int> > *AllEntryPoints) = 0;

			//	IWordEncoding(void){};
			//	virtual ~IWordEncoding(){};

			//};
		}
	}
}
