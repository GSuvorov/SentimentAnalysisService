#pragma once

namespace SS {
namespace CommonServices {
namespace UserTextStorage {


///////////////////////////////////////////////////////////////////////////////
// Реализация интерфейса IUsersManager
///////////////////////////////////////////////////////////////////////////////
class TUsersManager: public IUsersManager
{
   Detail::TUserTextStorage m_UserTextStorage;

public:     
   /// Инициализация
   /// IniFilePath- путь к INI-файлу DBMS
   /// DataCatalogPath - Каталог с файлами данных
   void Init( const std::wstring &IniFilePath, const std::wstring &DataCatalogPath );

   /// Инициализация по-умолчанию
   /// Необходимо вызывать перед использованием интерфейса
   /// DataCatalogPath - Каталог с файлами данных (Если пустой, то используется по-умолчанию
   void Init( const std::wstring &DataCatalogPath = L"" );

   /// Путь к каталогу с файлам данных 
   std::wstring GetDataCatalogPath() const;

   /// Добавляет пользователя в систему
   /// Автоматически присваивается UserID и заполняются все read only поля oUserName
   void AddUser( TPrivateUserInfo &UserInfo );

   /// Удаляет пользователя из системы
   void DeleteUser( TUserID UserID );

   /// Изменить информацию о пользователе
   void ChangeUserInfo( const TPrivateUserInfo &UserInfo );

   /// Возвращает список идентификаторов пользователей
   void GetUsersIDList(TUserIDList& UserIDList) const;

   /// Возвращает полную информацию о пользователе
   /// Пока поле Password игнорируется
   void GetPrivateUsersInfo( TUserID UserID, const std::wstring &Password, TPrivateUserInfo &UserInfo ) const;

   /// Возвращает информацию о пользователе доступную для широкого просмотра (без знания пароля)
   /// Пока поле Password игнорируется
   void GetPublicUsersInfo( TUserID UserID, TPublicUserInfo &UserInfo ) const;

   /// Найти UserID по имени пользователя
   bool FindUserIDByName( const std::wstring &Name, TUserIDList &UserIDList ) const;

public:
   RELEASE_IMPL()

   BEGIN_QI_MAP()
      QI_MAP_INHERIT_ENTRY( IID_UsersManager, IUsersManager )
      QI_MAP_INHERIT_ENTRY( IID_Base, IBase )
   END_QI_MAP()
};

///////////////////////////////////////////////////////////////////////////////
// Реализация интерфейса IText
///////////////////////////////////////////////////////////////////////////////
class TText: public IText
{
   Detail::TText m_Text;

public:
   /// Прочитать часть текста с именем PartName в Content
   /// Формат файла опредиляется автоматически
   bool GetTextPart( const std::wstring &PartName,  std::wstring &Content ) const;

   /// Записать отдельную часть текста с именем PartName в Content используя формат файла FileFormat
   void SetTextPart( const std::wstring &PartName,  const std::wstring &Content, EFileFormat FileFormat = FFUNICODE );

   /// Удалить отдельную часть текста с именем PartName
   bool DeleteTextPart( const std::wstring &PartName );

   /// Получить названия всех частей
   void GetTextPartNames( TStringsList &PartNameList );

public: //Функциии не относящияся к интерфейсу IText
   void OpenDir( const std::wstring &DirName );

public:
   RELEASE_IMPL()

   BEGIN_QI_MAP()
      QI_MAP_INHERIT_ENTRY( IID_Text, IText )
      QI_MAP_INHERIT_ENTRY( IID_Base, IBase )
   END_QI_MAP()
};

///////////////////////////////////////////////////////////////////////////////
// Реализация интерфейса ITextsManager
///////////////////////////////////////////////////////////////////////////////
class TTextsManager: public ITextsManager
{
   Detail::TTextsManager m_TextsManager;

public:   
   /// Открыть тексты пользователя. Инициализация.
   void Open( TUserID UserID, const std::wstring &Password, IUsersManager *pUsersManager );

   /// Добавить к текстам пользователя текст с именем TextName
   bool AddText( const std::wstring &TextName, IText *pText );

   /// Возвращает текст с именем TextName
   bool GetText( const std::wstring &TextName, IText *pText ) const;

   /// Удалить из текстов пользователя текст с именем TextName
   bool DeleteText( const std::wstring &TextName );

   /// Получить названия всех текстов пользователя
   void GetUserTextNames( TStringsList &TextNameList ) const;

public:
   RELEASE_IMPL()

   BEGIN_QI_MAP()
      QI_MAP_INHERIT_ENTRY( IID_TextsManager, ITextsManager )
      QI_MAP_INHERIT_ENTRY( IID_Base, IBase )
   END_QI_MAP()
};

} //namespace UserTextStorage
} //namespace CommonServices
} //namespace SS
