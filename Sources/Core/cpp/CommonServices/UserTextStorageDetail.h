#pragma once

namespace SS {
namespace CommonServices {
namespace UserTextStorage {
namespace Detail {

///////////////////////////////////////////////////////////////////////////////
// Упрправление пользователями
// Для каждого пользователя содаётся каталог
///////////////////////////////////////////////////////////////////////////////   
class TUserTextStorage: public NonCopyable
{
   struct TRecord
   {
      INT32 UserID;
      std::wstring Name;
      std::wstring Description;
      std::wstring DirectoryName;

      void *rgRecMap[4];

      TRecord(): UserID(0) 
      {
         rgRecMap[0] = &UserID;
         rgRecMap[1] = &Name;
         rgRecMap[2] = &Description;
         rgRecMap[3] = &DirectoryName;
      }
   };
   
   typedef ClassLevelLockable<TUserTextStorage> TMutex;  //Блокирование таблицы при добавлении

private:
   IDataBase *m_pDataBase;
   mutable CTablePtr m_pTable;
   TString m_DataCatalogPath;
   TMutex m_Mutex;


private:
   // Создаёт из пары UserName и UserID имя каталога пользователя
   void MakeUserCatalogName( const TString &UserName, INT32 UserID, TString &CatalogName )
   {
      std::basic_stringstream<TCHAR> Stream; 
      Stream << UserName << _T("  ") << UserID;

      CatalogName = Stream.str();
   }

public:
   TUserTextStorage():  m_pDataBase(0) {}
   ~TUserTextStorage() { Close(); }

   void Close();
   void Open( ILoadManager *pLoadManager, const TString &IniFilePath, const TString &DataCatalogPath );

   const TString &GetDataCatalogPath() const { return m_DataCatalogPath; }

   /// Добавляет пользователя в систему
   /// Автоматически присваивается UserID и заполняются все read only поля oUserName
   void AddUser( TPrivateUserInfo &oUserName );

   /// Удаляет пользователя из системы
   void DeleteUser( TUserID UserID );

   /// Изменить информацию о пользователе
   void ChangeUserInfo( const TPrivateUserInfo &oUserName );

   /// Возвращает список идентификаторов пользователей
   void GetUsersIDList( TUserIDList& oUserIDList) const;

   /// Возвращает полную информацию о пользователе
	/// Пока поле Password игнорируется
	void GetPrivateUsersInfo( TUserID UserID, const std::wstring &Password, TPrivateUserInfo &oUserInfo ) const;

   /// Возвращает информацию о пользователе доступную для широкого просмотра (без знания пароля)
	/// Пока поле Password игнорируется
	void GetPublicUsersInfo( TUserID UserID, TPublicUserInfo &oUserInfo ) const;

	/// Найти UserID по имени пользователя
	bool FindUserIDByName( const std::wstring &Name, TUserIDList &oUserIDList ) const;
};

///////////////////////////////////////////////////////////////////////////////
// Класс позволяет создавать и удалять и читать текстовые файлы в некотором 
// каталоге
///////////////////////////////////////////////////////////////////////////////
class TText
{
  std::wstring m_DirName;

public:
   /// Открыть каталог с файлами
   void OpenDir( const std::wstring &DirName ) { APL_ASSERT(!DirName.empty()); m_DirName = DirName; m_DirName += _T('\\'); } 
   
   /// Прочитать текст из файла
   /// Возвр: Есть ли такой файл
   bool GetFileText( const std::wstring &FileName,  std::wstring &Content ) const;

   /// Записать текст в файл
   void SetFileTextAnsi( const std::wstring &FileName, const std::wstring &Content );
   void SetFileTextUnicode( const std::wstring &FileName, const std::wstring &Content );

   /// Удалить файл
   /// Возвр: Файл найден и был удалён
   bool DeleteFileText( const std::wstring &FileName );

   /// Список файлов в каталоге
   void GetFileNames( TStringsList &FileNameList ) const;
};

///////////////////////////////////////////////////////////////////////////////
// Ксласс позволяет управлять каталогами  текстов для некоторого пользователя
///////////////////////////////////////////////////////////////////////////////
class TTextsManager
{
   std::wstring m_DirName;

public:
   /// Открыть каталог с файлами
   void OpenDir( const std::wstring &DirName ); 

   //Текущий каталог
   const std::wstring &GetCurDir() const { return m_DirName; }

   /// Добавить каталог
   /// Возвр: false - каталог уже был создан
   bool AddSubDir( const std::wstring &DirName );

   /// Есть ли подкаталог
   bool IsExistSubDir( const std::wstring &DirName ) const;

   /// Удалить подкаталог
   /// Возвр: каталог был успешно удалён
   bool DeleteSubDir( const std::wstring &DirName );

   /// Список подкаталогов
   void GetSubDirNames( TStringsList &DirNameList ) const;
};

} //namespace Detail 
} //namespace UserTextStorage
} //namespace CommonServices
} //namespace SS
