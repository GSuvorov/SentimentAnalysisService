#pragma once
#include "..\assinterface\iblackbox.h"
#include ".\smexpert.h"

namespace SS
{
	namespace Semantic
	{

using namespace SS::Interface::Core;

		class CSMExpertGroup : virtual public SS::Interface::Core::Common::CAnalyseParams
		{
		public:
			CSMExpertGroup(TCHAR * scDescription);
			virtual ~CSMExpertGroup(void);
			/// анализ текста BlackBox
			/** 
			\param pIText - текст для анализа
			\return bool - истина - если анализ прошёл успешно

			задаёт единый интерфейс к поведению экспертов
			*/
			bool Analize(BlackBox::IText * pIText);

			/// добавление эксперта в группу
			/** 
				\param pExpert - эксперт
			*/
			void AddExpert( CSMExpert * pExpert );
			/// возвращает описание группы
			wstring GetGroupDescription(){ return m_szDescription; };
			/// устанавливает описание группы
			void SetGroupDescription(TCHAR * scDescription){ m_szDescription=scDescription; };
			void SetExpertsSMManager(CSMManager * pManager);

		protected:
			void DeleteExperts();

		private:
			list<CSMExpert*> m_Experts;
			list<CSMExpert*>::iterator m_CurrentExpert;
			
			wstring m_szDescription;
		};
	}
}