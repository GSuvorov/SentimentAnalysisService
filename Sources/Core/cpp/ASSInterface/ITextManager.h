#pragma once

#include "IAutorization.h"

// {5309D5B4-9C46-4cf4-8F5C-8CFD1698FCFC}
static const GUID IID_Text = 
{ 0x5309d5b4, 0x9c46, 0x4cf4, { 0x8f, 0x5c, 0x8c, 0xfd, 0x16, 0x98, 0xfc, 0xfc } };

// {84A154E8-4B3F-4841-8E35-55D4FF265293}
static const GUID CLSID_Text = 
{ 0x84a154e8, 0x4b3f, 0x4841, { 0x8e, 0x35, 0x55, 0xd4, 0xff, 0x26, 0x52, 0x93 } };

// {EFD48B7A-0787-4b00-8478-6B65F86AC0ED}
static const GUID IID_TextsManager = 
{ 0xefd48b7a, 0x787, 0x4b00, { 0x84, 0x78, 0x6b, 0x65, 0xf8, 0x6a, 0xc0, 0xed } };

// {8E1E4F51-71D7-4aec-AF6A-A6B601B525CD}
static const GUID CLSID_TextsManager = 
{ 0x8e1e4f51, 0x71d7, 0x4aec, { 0xaf, 0x6a, 0xa6, 0xb6, 0x1, 0xb5, 0x25, 0xcd } };


namespace SS
{
namespace TextsMangers
{
namespace Types
{
	typedef std::list<std::wstring> TStringsList;

   //Формат сохраняемого файла
   enum EFileFormat
   {
      FFANSI,
      FFUNICODE
   };
}
}

namespace Interface
{
namespace TextsMangers
{   
   /// Информация о тексте пользователя
   /// Каждый текст состоит из нескольких частей (которые представляются как строки)
   class IText: public virtual SS::Interface::IBase
   {
   public:
		virtual ~IText() {};
		
      /// Прочитать часть текста с именем PartName в Content
      /// Формат файла опредиляется автоматически
      virtual bool GetTextPart( const std::wstring &PartName,  std::wstring &Content ) const = 0;
		
      /// Записать отдельную часть текста с именем PartName в Content используя формат файла FileFormat
		virtual void SetTextPart( 
         const std::wstring &PartName,  const std::wstring &Content, 
         SS::TextsMangers::Types::EFileFormat FileFormat = SS::TextsMangers::Types::FFUNICODE 
      ) = 0;
      
      /// Удалить отдельную часть текста с именем PartName
      virtual bool DeleteTextPart( const std::wstring &PartName ) = 0;
      
      /// Получить названия всех частей
      virtual void GetTextPartNames( SS::TextsMangers::Types::TStringsList &PartNameList ) = 0;
	};

	/// Управление списками текстов пользователя
	class ITextsManager : public virtual SS::Interface::IBase
	{
	public:   
		virtual ~ITextsManager() {}   

		/// Открыть тексты пользователя. Инициализация.
      virtual void Open( SS::Autorization::Types::TUserID UserID, const std::wstring &Password, SS::Interface::Autorization::IUsersManager *pUsersManager ) = 0;

		/// Добавить к текстам пользователя текст с именем TextName
      virtual bool AddText( const std::wstring &TextName, 
         SS::Interface::TextsMangers::IText *pText ) = 0;

		/// Возвращает текст с именем TextName
		virtual bool GetText( 
			const std::wstring &TextName, 
			SS::Interface::TextsMangers::IText *pText ) const = 0;

		/// Удалить из текстов пользователя текст с именем TextName
		virtual bool DeleteText( const std::wstring &TextName ) = 0;

		/// Получить названия всех текстов пользователя
		virtual void GetUserTextNames( SS::TextsMangers::Types::TStringsList &TextNameList ) const = 0;
   };
}
}
}