// ITextsInfoExtracting.h

#pragma once

#include ".\ICommonContainers.h"
#include ".\ITextFeature.h"
#include <string>

// {10A19D78-8F7E-432f-9FE0-82D55AAC92F7}
static const GUID CLSID_NdxSearchEngine = 
{ 0x10a19d78, 0x8f7e, 0x432f, { 0x9f, 0xe0, 0x82, 0xd5, 0x5a, 0xac, 0x92, 0xf7 } };

// {B36E0120-DCB1-4591-8477-2E8B4CC412E9}
static const GUID IID_TextsInfoExtracting = 
{ 0xb36e0120, 0xdcb1, 0x4591, { 0x84, 0x77, 0x2e, 0x8b, 0x4c, 0xc4, 0x12, 0xe9 } };

namespace SS
{
namespace Interface
{
namespace Core
{
namespace TextsInfo
{

//--------------------------------------------------------------------//

///интерфейс доступа к информации по текстам, работает с текущей активной индексной базой
class ITextsInfoExtracting : public virtual IBase
{
public:
	///заполняет ITextFeature по номеру текста
	virtual void GetTextInfo(unsigned int uiTextIndex, 
		SS::Interface::Core::CommonContainers::ITextFeature* pTextFeature) = 0;

	///извлекает бинарное содержимое текста для данного индексного хранилища по номеру текста
	///и добавляет его в ITextBlock, номер текста должен храниться в текстфиче ITextBlock-а
	virtual void FillTextContent(std::wstring* pwsIndexStorageName, 
			unsigned int uiHeadPosition,
			unsigned int uiTailPosition,
			SS::Interface::Core::CommonContainers::ITextBlock* pTextBlock) = 0;

};

//--------------------------------------------------------------------//

}
}
}
}