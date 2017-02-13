#pragma once

#include "../ASSInterface/ISemanticAnalyze.h"
#include "SMManager.h"


namespace SS
{
	namespace Core
	{
		class CSemanticAnalyze:
			public SS::Interface::Core::ISemanticAnalyze
		{
		public:
			CSemanticAnalyze(void);
			~CSemanticAnalyze(void);
			//IBase
			///приведение к интерфейсу с поданным REFIID. 
			/**pBase будет NULL, если данный интерфейс не поддерживается*/
			HRESULT QueryInterface(REFIID pIID, void** pBase); 
			///освобождение
			ULONG Release();

			//void Init(
			//	///параметры анализа
			//	SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams);

			///анализ очередной части текста и занесение результата в pText
			bool AnalyseNext(
				///это куда записывать результат
				SS::Interface::Core::BlackBox::IText* pText);

			/// возвращает строку по обработанному запросу
			/** 
			\param pQuery - контейнер содержащий разобранный заброс (результат в контейнере pQuery)
			\return bool - истина - если не было ошибок

			до вызова этой функции должен быть сделан вызов Init с параметром - анализ вопроса , затем 
			AnalyseNext(...)
			*/
			bool QueryToSearchString(SS::Interface::Core::BlackBox::IText* pQuery,
				std::wstring * pSearchString);

			bool SyntaxQuestionPostProcess(SS::Interface::Core::BlackBox::IText* pQuery);

			void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);

			void SetAnalyseParams(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams);


		private:

			SS::Semantic::CSMManager m_SemanticAnalyzeManager;

			SS::Interface::Core::BlackBox::IText * m_pIText;
			//SS::Interface::Core::MainAnalyse::IAnalyseParams* m_pAnalyseParams;
		};

	}
}

