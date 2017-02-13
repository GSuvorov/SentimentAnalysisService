#pragma once

#include "..\assinterface\iblackbox.h"

#include "IMainAnalyse.h"
#include "ICommon.h"


// {C4683DA1-D383-440a-ACC2-8FCEDA5A52A5}
static const GUID CLSID_SemanticAnalyzer = 
{ 0xc4683da1, 0xd383, 0x440a, { 0xac, 0xc2, 0x8f, 0xce, 0xda, 0x5a, 0x52, 0xa5 } };

// {4245B9A9-BBE1-4db2-A54B-113ACC7AAA24}
static const GUID IID_SemanticAnalyzer = 
{ 0x4245b9a9, 0xbbe1, 0x4db2, { 0xa5, 0x4b, 0x11, 0x3a, 0xcc, 0x7a, 0xaa, 0x24 } };


namespace SS
{
	namespace Interface
	{
		namespace Core
		{

			class ISemanticAnalyze : 
				virtual public SS::Interface::IBase,
				virtual public SS::Interface::Core::Common::CAnalyseParams
			{
			public:
				virtual ~ISemanticAnalyze(void) {} ;
				ISemanticAnalyze(void) {} ;

				//virtual void Init(
				//	///параметры анализа
				//	SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams) = 0;

				///анализ очередной части текста и занесение результата в pText
				virtual bool AnalyseNext(
					///это куда записывать результат
					SS::Interface::Core::BlackBox::IText* pText) = 0;

				/// возвращает строку по обработанному запросу
				/** 
				\param pQuery - контейнер содержащий разобранный заброс (результат в контейнере pQuery)
				\return bool - истина - если не было ошибок

				до вызова этой функции должен быть сделан вызов Init с параметром - анализ вопроса , затем 
				AnalyseNext(...)
				*/
				virtual bool QueryToSearchString(SS::Interface::Core::BlackBox::IText* pQuery,
					std::wstring * pSearchString)=0;

				virtual bool SyntaxQuestionPostProcess(SS::Interface::Core::BlackBox::IText* pQuery)=0;

			};

		}
	}
}


