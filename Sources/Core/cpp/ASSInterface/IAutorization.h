#pragma once

#include <string>
#include <list>

// {9A9F00C8-D506-4f5f-BE15-34731A93D5D4}
static const GUID IID_UsersManager = 
{ 0x9a9f00c8, 0xd506, 0x4f5f, { 0xbe, 0x15, 0x34, 0x73, 0x1a, 0x93, 0xd5, 0xd4 } };

// {859A4AC9-EA7D-4db5-81EA-E40536BF6A4C}
static const GUID CLSID_UsersManager = 
{ 0x859a4ac9, 0xea7d, 0x4db5, { 0x81, 0xea, 0xe4, 0x5, 0x36, 0xbf, 0x6a, 0x4c } };

namespace SS
{
namespace Autorization
{
namespace Types
{  
   typedef unsigned int TUserID;
 
   /// Незащищённые данные о пользователе
   /// UserID - является уникальным идентификатором, который однозначно идентифицирует пользователя
   class TPublicUserInfo
   {
      TUserID m_UserID;              //Идентификатор пользователя
      std::wstring m_wName;          //Имя пользователя

   public:
      /// Идентификатор пользователя read only
      TUserID GetUserID() const { return m_UserID; }

      /// Имя пользователя
      void SetUserName( const std::wstring &Val ){ m_wName = Val; }
      std::wstring GetUserName() const { return m_wName; }

   public: //Внутренние детали реализации, НЕ ВЫЗЫВАТЬ!
      void SetUserID( TUserID Val ){ m_UserID = Val; }
   };
   
   /// Защищённые данные о пользователе, для доступа к которым необходимо знание пароля
   class TPrivateUserInfo: public TPublicUserInfo
   {
      std::wstring m_wPassword;    //Пароль пользователя
      std::wstring m_wDescription;   //Произвольное описание пользователя
      std::wstring m_UserCatalog;    //Каталог польователя

   public:
      /// Произвольное описание пользователя
      void SetDescription( const std::wstring &Val ){ m_wDescription = Val; }
      std::wstring GetDescription() const { return m_wDescription; }

      /// Пароль пользователя
      void SetPassword( const std::wstring &Val ){ m_wPassword = Val; }
      std::wstring GetPassword() const { return m_wPassword; }
      
      /// Личный каталог пользователя
      std::wstring GetUserCatalog() const { return m_UserCatalog; }

   public: //Внутренние детали реализации, НЕ ВЫЗЫВАТЬ!
      void SetUserCatalog( const std::wstring &Val ){ m_UserCatalog = Val; }

   };

   typedef std::list<TUserID> TUserIDList;
}
}

namespace Interface
{
namespace Autorization
{
   /// Управление пользователями
   /// Пароль пока игнорируется
	class IUsersManager : public virtual SS::Interface::IBase
	{
	public:   
		virtual ~IUsersManager() {};

      /// Инициализировать по-умолчанию
      /// Необходимо вызывать перед использованием интерфейса
      /// DataCatalogPath - Каталог с файлами данных (Если пустой, то используется по-умолчанию
      virtual void Init( const std::wstring &DataCatalogPath = L"" ) = 0;

      /// Инициализация
      /// IniFilePath- путь к INI-файлу DBMS
      /// DataCatalogPath - Каталог с файлами данных
      /// virtual void Init( const std::wstring &IniFilePath, const std::wstring &DataCatalogPath ) = 0;

      /// Путь к каталогу с файлам данных 
      virtual std::wstring GetDataCatalogPath() const = 0;

      /// Добавляет пользователя в систему
		/// Автоматически присваивается UserID и заполняются все read only поля oUserName
		virtual void AddUser( /*inout*/SS::Autorization::Types::TPrivateUserInfo &UserInfo ) = 0;

		/// Удаляет пользователя из системы
		virtual void DeleteUser( /*in*/SS::Autorization::Types::TUserID UserID ) = 0;

		/// Изменить информацию о пользователе
		virtual void ChangeUserInfo( /*in*/const SS::Autorization::Types::TPrivateUserInfo &UserInfo ) = 0;

		/// Возвращает список идентификаторов пользователей
		virtual void GetUsersIDList( /*out*/SS::Autorization::Types::TUserIDList& UserIDList) const = 0;

		/// Возвращает полную информацию о пользователе
		/// Пока поле Password игнорируется
		virtual void GetPrivateUsersInfo( 
			/*in*/SS::Autorization::Types::TUserID UserID, 
			/*in*/const std::wstring &Password, 
			/*out*/SS::Autorization::Types::TPrivateUserInfo &UserInfo ) const = 0;

      /// Возвращает информацию о пользователе доступную для широкого просмотра (без знания пароля)
		/// Пока поле Password игнорируется
		virtual void GetPublicUsersInfo( 
			/*in*/SS::Autorization::Types::TUserID UserID, 
			/*out*/SS::Autorization::Types::TPublicUserInfo &UserInfo ) const = 0;

		/// Найти UserID по имени пользователя
		virtual bool FindUserIDByName( 
		/*in*/const std::wstring &Name, 
		/*out*/SS::Autorization::Types::TUserIDList &UserIDList ) const = 0;
	}; 
}
}
}