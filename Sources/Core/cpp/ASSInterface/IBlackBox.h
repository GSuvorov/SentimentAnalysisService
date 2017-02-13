#pragma once

#include "IBlackBoxItems.h"
#include "IBlackBoxQuery.h"
#include "IObjectDescr.h"

// {86F4B584-8230-437f-A0F8-A1A230A918D6}
static const GUID CLSID_BlackBoxTextStorage = 
{ 0x86f4b584, 0x8230, 0x437f, { 0xa0, 0xf8, 0xa1, 0xa2, 0x30, 0xa9, 0x18, 0xd6 } };
// {06A238CF-0406-45fa-8BC2-370DCF36A0CB}
static const GUID IID_BlackBoxTextStorage = 
{ 0x6a238cf, 0x406, 0x45fa, { 0x8b, 0xc2, 0x37, 0xd, 0xcf, 0x36, 0xa0, 0xcb } };

// {098BB93C-D591-47f7-85AB-26D2F9FE5B28}
static const GUID CLSID_BlackBoxQueryStorage = 
{ 0x98bb93c, 0xd591, 0x47f7, { 0x85, 0xab, 0x26, 0xd2, 0xf9, 0xfe, 0x5b, 0x28 } };
// {BAC81F68-716D-4e0f-9C4C-C491F811A137}
static const GUID IID_BlackBoxQueryStorage = 
{ 0xbac81f68, 0x716d, 0x4e0f, { 0x9c, 0x4c, 0xc4, 0x91, 0xf8, 0x11, 0xa1, 0x37 } };


namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			namespace BlackBox
			{
				
/// управление хранилищем, имеющим структуру текст-абзац-предложение-слово
/** 
	фактически является фабрикой классов, создает по требованию
 	экземпляры различных итемов для BlackBox
*/
				class IBlackBoxTextStorage : public virtual SS::Interface::IBase
				{
				public:
					///создание экземпляра текста
					virtual IText*			CreateText() = 0;
					///создание экземпляра параграфа
					virtual IParagraph*		CreateParagraph() = 0;
					///создание экземпляра предложения
					virtual ISentence*		CreateSentence() = 0;
					///создание экземпляра юнита(квази-слова)
					virtual IUnit*			CreateUnit() = 0;
					///создание экземпляра индекса
					virtual IIndex*			CreateIndex() = 0;
					///создание экземпляра морфологических характеристик для данного индекса
					virtual IFeature*		CreateFeature() = 0;
					///создание экземпляра текстовой фичи
					virtual ITextFeature*	CreateTextFeature() = 0;
					///создание экземпляра описателя объекта для дайдежста
					virtual SS::Interface::Core::BlackBox::IObjectDescr*	CreateObjectDescr() = 0;
				};

/// управление хранилищем, содержащим запрос
/** 
	фактически является фабрикой классов, создает по требованию
 	экземпляры различных итемов для BlackBox запроса
*/
				class IBlackBoxQueryStorage : public virtual SS::Interface::IBase
				{
				public:
					///создание экземпляра запроса
					virtual Query::IQuery*					CreateQuery() = 0;
					///создание экземпляра трансформации запроса
					virtual Query::ITransformation*			CreateTransformation() = 0;
					///создание экземпляра группы трансформации
					virtual Query::IGroup*					CreateGroup() = 0;
					///создание экземпляра юнита группы
					virtual Query::IQueryUnit*				CreateQueryUnit() = 0;
					///создание экземпляра индекса юнита
					virtual Query::IQueryUnitIndex*			CreateQueryUnitIndex() = 0;
					///создание экземпляра коллекции индексов юнита
					virtual Query::IQueryUnitIndexCollection* CreateQueryUnitIndexCollection() = 0;
				};

			}
		}
	}
}
