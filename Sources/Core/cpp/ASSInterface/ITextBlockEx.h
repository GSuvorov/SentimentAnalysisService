//ITextBlockEx.h

#pragma once

#include "..\ASCInterface\ICommonContainers.h"
#include "..\ASCInterface\TCoommonTypes.h"
#include ".\IBlackBoxItems.h"


namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			namespace CommonContainers
			{
		
				using SS::Interface::Core::BlackBox::IText;
				using SS::Core::Types::IndexationSearch::TLinguisticProcessorMode;
				
				///реализует функциональность ITextBlock + его заполнение из IText + формирование IText из заполненного ITextBlock
				class ITextBlockEx : virtual public ITextBlock 
				{
				public:
					///ОБ УДАЛЕНИИ IText в SetIText() и GenerateIText(), ДОЛЖЕН ЗАБОТИТЬСЯ ИХ ВЫЗЫВАЮЩИЙ
					///заполнение ITextBlock из поданного IText
					virtual void SetIText(IText* pIText, bool bIsCopyTextFeature = true) = 0;
					///формирование IText из полного ITextBlock
					virtual IText* GenerateIText() = 0;
					///установление режима лингвистического процессора
					virtual void SetLinguisticProcessorMode(TLinguisticProcessorMode* pLinguisticProcessorMode) = 0;
					/// выполняет обновление релевантности
					virtual void UpdateRelevance() = 0;
				};			
			
			
			}
		}
	}
}