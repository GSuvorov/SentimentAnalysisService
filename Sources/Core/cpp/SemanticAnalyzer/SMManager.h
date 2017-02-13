#pragma once
//#include "..\assinterface\WorkWithManagerConnection.h"
#include "..\ASSInterface\IAMConverter.h"
#include "..\assinterface\iblackbox.h"
//#include "..\ASSInterface\IDateVirtualDictionary.h"
#include "..\ASSInterface\IDictionaryManager.h"
#include ".\smexpertgroup.h"
#include "..\ASCInterface\services.h"

#include "SMPostSyntax.h"
#include "SMQueryG.h"
#include "SMIndaxation.h"
#include "SMQuestion.h"
#include "..\ASCInterface\services.h"

#include ".\OfficialVirtualSemanticDictionary.h"


namespace SS
{
	namespace Semantic
	{
		class CSMExpert;

		class CSMManager
			: virtual public SS::Core::CommonServices::CBaseCoreClass
			, virtual public SS::Interface::Core::Common::CAnalyseParams
		/// управл€ет экспертами - распредел€ет указатели на интерфейсы, группирует экспертов
			//public SS::Core::Classes::CWorkWithManagerConnection
		{
		public:
			CSMManager(void);
			~CSMManager(void);
			
			void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
			/// установить строку
			/** 
				\param str - строка
			*/
			void SetQueryString(wstring & str){ m_sQuery=str; };

			SS::Interface::Core::AMConverter::IAMConverterMorpho * GetAMConverterMorpho(){return m_pIAMCMorpho;};
			SS::Interface::Core::AMConverter::IAMConverterSemantic * GetAMConverterSemantic(){return m_pIAMCSemantic;};
			SS::Semantic::VirtualDictionaries::COfficialVirtualSyntaxDictionary * GetOfficialVirtualSyntax(){ return m_pOfficialVirtualSyntax; };
			SS::Interface::Core::BlackBox::IBlackBoxTextStorage * GetBlackBoxTextStorage(){ return m_pIBlackBoxFactory;};
			SS::Interface::Core::Dictionary::IMorphologyAnalyser * GetDictionaryInterface(){ return m_pDictionaryInterface; };

			/// доразбор вороса после синтаксиса 
			/** 
				\param pIText - разобранный вопрос
			*/
			void AnalizeQuestion(SS::Interface::Core::BlackBox::IText * pIText);
			/// преобразование вопроса в строку запроса
			/** 
				\param pIText - разобранный вопрос
				\return 			wstring - строка коллектора
			*/
			void CollectQuestion(SS::Interface::Core::BlackBox::IText * pIText, std::wstring * pSearchString);
	
			/// проведение анализа типа индексации
			void IndexateText(SS::Interface::Core::BlackBox::IText * pIText);
			/// анализ вопроса
			void QuestionAnalysis(SS::Interface::Core::BlackBox::IText * pIText);

		private:
			///морфологический конвертор 
			SS::Interface::Core::AMConverter::IAMConverterMorpho * m_pIAMCMorpho;
			///семантический конвертор
			SS::Interface::Core::AMConverter::IAMConverterSemantic * m_pIAMCSemantic;
			SS::Interface::IBase * m_pVirtualDictionaries;
			SS::Semantic::VirtualDictionaries::COfficialVirtualSyntaxDictionary * m_pOfficialVirtualSyntax;
			SS::Interface::Core::BlackBox::IBlackBoxTextStorage * m_pIBlackBoxFactory;
			SS::Interface::Core::Dictionary::IMorphologyAnalyser * m_pDictionaryInterface;
			/// строка запроса 
			wstring m_sQuery;
			/// все эксперты в системе
			std::list<CSMExpert*> m_AllExperts;
			/// эксперты дл€ окончательного разбора после синтаксиса
			Groups::CSMPostSyntax m_QuestionParse;
			/// эксперты дл€ строк коллектора
			Groups::CSMQueryG m_QuestionGenerator;

			/// эксперты дл€ индексации
			Groups::CSMIndaxation m_IndaxationGroup;
			/// эксперты дл€ разбора вопроса
			Groups::CSMQuestion m_QuestionGroup;



		};
	}
}