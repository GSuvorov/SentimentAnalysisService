//ITextFeature.h
#pragma once

#include "IInterface.h"
// {82E5D81C-04A6-4e82-83F5-9380FAB6E732}
static const GUID CLSID_TextFeature = 
{ 0x82e5d81c, 0x4a6, 0x4e82, { 0x83, 0xf5, 0x93, 0x80, 0xfa, 0xb6, 0xe7, 0x32 } };
// {5D601B65-40C4-4875-91A7-36ED4F2E0DF1}
static const GUID IID_TextFeature = 
{ 0x5d601b65, 0x40c4, 0x4875, { 0x91, 0xa7, 0x36, 0xed, 0x4f, 0x2e, 0xd, 0xf1 } };

namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			namespace CommonContainers
			{
				class ITitle
				{
				public:
					virtual ~ITitle() {}
					///абсолютный номер первого слова заголовка
					virtual int GetFirstWord() = 0;
					///абсолютный номер последнего слова заголовка
					virtual int GetLastWord() = 0;
					///тип заголовка
					virtual int GetType() = 0;
					///указатель на следующий заголовок
					virtual ITitle* GetNextTitle() = 0;
				};

				///инкапсулятор характеристик текста (путь к тексту и проч.)
				/**
				предоставляет также функции сохранения
				в буфер байт и восстановления из
				буфера байт (для обеспечения хранения в сервисной базе)
				*/
				class ITextFeature : public IBase
				{
				public:
					virtual ~ITextFeature(){};


					virtual void GetTextFeatureBuff(char** ppBuff, unsigned int* pSize) = 0;
					virtual void SetTextFeatureBuff(char* pBuff, unsigned int Size) = 0;


					///получить путь к тексту
					virtual const wchar_t* GetTextPath() = 0;
					///записать путь к тексту
					virtual void SetTextPath(const wchar_t* szPath) = 0;

					///получить имя текста
					virtual const wchar_t* GetTextName() = 0;
					///записать имя текста
					virtual void SetTextName(const wchar_t* szName) = 0;

					///Получить права доступа
					virtual const char* GetAccessRights() = 0;
					///записать права доступа
					virtual void SetAccessRights(const char* szAccessRights) = 0;

					/////получить имя базы
					//virtual const char* GetBaseName() = 0;
					/////записать имя базы
					//virtual void SetBaseName(const char* szName) = 0;

					///установление абсолютного номера первого предложения текста
					virtual void SetFirstSentenceAbsNumber(unsigned int uiValue) = 0;
					///получение абсолютного номера первого предложения текста
					virtual unsigned int GetFirstSentenceAbsNumber() = 0;

					///установление номера текста
					virtual void SetTextNumber(unsigned int uiValue) = 0;
					///получение номера текста
					virtual unsigned int GetTextNumber(void) = 0;

					///установление размера текста
					virtual void SetTextSize(unsigned int uiValue) = 0;
					///получение размера текста
					virtual unsigned int GetTextSize(void) = 0;

					///вернет время последнего изменения файла с текстом
					virtual const FILETIME GetTextFileTime() const = 0;
					///устанавливает время последнего изменения файла с текстом
					virtual void SetTextFileTime(const FILETIME* pFileTime) = 0;

					///устанавливает размер файла текста
					virtual void SetTextFileSize(const unsigned int ulFileSize) = 0;
					///возвращает размер файла текста
					virtual const unsigned int GetTextFileSize() const  = 0;

					///устанавливает ID плагина
					virtual void SetPluginID(const unsigned int uiPluginID) = 0;
					/// возвращает ID плагина
					virtual const unsigned int GetPluginID() const  = 0;

					///очистка значений характеристики
					virtual void Clear() = 0;

					virtual ITitle* GetFirstTitle() = 0;
					virtual void AddTitle(int nBegin, int nEnd, int iType) = 0;
				};
			


			}
		}
	}
}