// IQPCounter.h
#pragma once

#include "..\ASCInterface\IInterface.h"
//#include "..\ASCInterface\ILinguisticProcessor.h"
//#include "..\ASCInterface\TCoommonTypes.h"
#include <list>

// {0C6B8616-1AE0-4e73-A7E5-1FFD1675C537}
static const GUID CLSID_ICounterSessions = 
{ 0xc6b8616, 0x1ae0, 0x4e73, { 0xa7, 0xe5, 0x1f, 0xfd, 0x16, 0x75, 0xc5, 0x37 } };

// {912506EE-F73C-494f-A91D-CBBDFB39E5D1}
static const GUID IID_ICounterSessions = 
{ 0x912506ee, 0xf73c, 0x494f, { 0xa9, 0x1d, 0xcb, 0xbd, 0xfb, 0x39, 0xe5, 0xd1 } };

namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			namespace CommonServices
			{
				class ICounterSession;

				class ICounterSessions : public virtual SS::Interface::IBase
				{
				public:
					///вернет сессию с заданным именем. Если не существует, создаст и вернет.
					virtual ICounterSession* GetSession(wchar_t* wszName) = 0;
					///Перестартует все сессии (то есть дает команду всем сессиям сбросить статистику на диск и начать вести ее заново).
					virtual void DropAll() = 0;
					///Перестартует все сессии (то есть дает команду всем сессиям сбросить статистику на диск).
					virtual void DropStatistics() = 0;
					/// Вернет список с указателями на все созданные на данный момент сессии.
					virtual std::list<ICounterSession*>* GetSessionList() = 0;
				};

				///Сессия счетчиков.
				class ICounterSession // : public virtual SS::Interface::IBase
				{
				public:
					/// Начало сбора статистики по сессии.
					virtual void StartSession() = 0;
					/// Окончание сбора статистики по сессии.
					virtual void StopSession() = 0;
					///Запустить счетчик.
					virtual void StartCounter(wchar_t* wszName) = 0;
					///Остановить счетсик.
					virtual void StopCounter(wchar_t* wszName) = 0;
					///Начало сбора статистики по части.
					virtual void StartPart() = 0;
					///Окончание сбора статистики по части.
					virtual void StopPart() = 0;
					///Начало сбора статистики по части. Передается дополнительная строка информации, которая будет записана в строке, отражающей статистику по данному вызову части.
					virtual void StartPart(wchar_t* wszInfo) = 0;
					///Передается дополнительная строка информации, которая будет записана в строке, отражающей статистику по данному вызову части (заменяет собой строку, переданную в StartPart, если была).
					virtual void StopPart(wchar_t* wszInfo) = 0;
					///Начало сбора статистики по части. Передается дополнительная строка информации, которая будет записана в строке, отражающей статистику по данному вызову части. Дополнительно передается размер в байтах обрабатываемой информации.
					virtual void StartPart(wchar_t* wszInfo, unsigned int uiValue) = 0;
					///Передается дополнительная строка информации, которая будет записана в строке, отражающей статистику по данному вызову части (заменяет собой строку, переданную в StartPart, если была). Дополнительно передается размер в байтах обрабатываемой информации (это значение заменяет собой значение, переданное в StartPart, если было).
					virtual void StopPart(wchar_t* wszInfo, unsigned int uiValue) = 0;
					///Сброс статистики на диск и ее обнуление, рестарт сессии
					virtual void DropAll() = 0;
					///Сброс статистики на диск без обнуления, рестарт сессии
					virtual void DropStatistics() = 0;
				};
			}
		}
	}
}
