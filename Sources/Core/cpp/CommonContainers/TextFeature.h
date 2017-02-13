//TextFeature.h

#pragma once

#include "..\ASCInterface\ICommonContainers.h"
#include "..\ASCInterface\ITextFeature.h"
#include "..\ASSInterface\IBlackBox.h"
#include <list>

namespace SS
{
	namespace Core
	{
		namespace CommonContainers
		{

			using SS::Interface::Core::CommonContainers::ITextFeature;
			using SS::Interface::Core::BlackBox::IBlackBoxTextStorage;
			using SS::Interface::Core::CommonContainers::ITitle;
			using SS::Interface::Core::ResourceManagers::ILoadManager;
				
			///инкапсулятор характеристик текста (путь к тексту и проч.)
			/**
			предоставляет также функции сохранения
			в буфер байт и восстановления из
			буфера байт (для обеспечения хранения в сервисной базе)
			*/
			class CTextFeature : public ITextFeature
			{
			private:
				ITextFeature*			m_pItemTextFeature;
				IBlackBoxTextStorage*	m_pBlackBoxTextStorage;
			public:
			  //КОНСТРУКТОРЫ
				CTextFeature();
				virtual ~CTextFeature();

			  //ПЕРЕОПРЕДЕЛЯЮ МЕТОД IBase
				///устанавливает указатель на лоад мэнеджер
				virtual void SetLoadManager(ILoadManager* pLoadManager);

			  //МЕТОДЫ IBase
				virtual ULONG Release();
				virtual HRESULT QueryInterface(REFIID pIID, void** pBase);	//приведение к интерфейсу с поданным REFIID. 
																			//pBase будет NULL, если данный интерфейс не поддерживается
			  //МЕТОДЫ ITextFeature
				virtual ITitle* GetFirstTitle();
				virtual void AddTitle(int nBegin, int nEnd, int iType);
				
				virtual void GetTextFeatureBuff(char** ppBuff, unsigned int* pSize);
				virtual void SetTextFeatureBuff(char* pBuff, unsigned int Size);
				///получить путь к тексту
				virtual const wchar_t* GetTextPath();										
				///записать путь к тексту
				virtual void SetTextPath(const wchar_t* szPath);							
				///получить имя текста
				virtual const wchar_t* GetTextName();										
				///записать имя текста
				virtual void SetTextName(const wchar_t* szName);							
				///получить права доступа
				virtual const char* GetAccessRights();									
				///записать права доступа
				virtual void SetAccessRights(const char* szAccessRights);				
				///получить имя базы
				//virtual const char* GetBaseName();										
				///записать имя базы
				//virtual void SetBaseName(const char* szName);							
				///очистка значений характеристики
				virtual void Clear();													
				///установление абсолютного номера первого предложения текста
				virtual void SetFirstSentenceAbsNumber(unsigned int uiValue);			
				///получение абсолютного номера первого предложения текста
				virtual unsigned int GetFirstSentenceAbsNumber();						
				///установление номера текста
				virtual void SetTextNumber(unsigned int uiValue);
				///получение номера текста
				virtual unsigned int GetTextNumber();
				///установление размера текста
				virtual void SetTextSize(unsigned int uiValue);
				///получение размера текста
				virtual unsigned int GetTextSize();
				///вернет время последнего изменения файла с текстом
				virtual const FILETIME GetTextFileTime() const;
				///устанавливает время последнего изменения файла с текстом
				virtual void SetTextFileTime(const FILETIME* pFileTime);
				///устанавливает размер файла текста
				virtual void SetTextFileSize(const unsigned int ulFileSize);
				///возвращает размер файла текста
				virtual const unsigned int GetTextFileSize() const;
				///устанавливает ID плагина
				virtual void SetPluginID(const unsigned int uiPluginID);
				///возвращает ID плагина
				virtual const unsigned int GetPluginID() const;
			};


		}
	}
}