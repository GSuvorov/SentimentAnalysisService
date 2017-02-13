#pragma once

//#include "IInterface.h"

// --------------------------------------------------------------------------------
// GUIDs & IIDs :

/// Идентификатор класса DataBaseTransformer
static const GUID CLSID_DataBaseTransformer = { 0x4ba98624, 0x2133, 0x4b90, { 0xad, 0x44, 0xc, 0xf, 0xe7, 0x1a, 0xd2, 0xb0 } };
/// Идентификатор интерфейса DataBaseTransformer
static const GUID IID_DataBaseTransformer = { 0xf3471565, 0xf588, 0x468d, { 0xb9, 0xfb, 0x6b, 0x2f, 0xb0, 0xef, 0x42, 0xd6 } };


namespace SS   {
	namespace Interface   {
		namespace SubSidary   {	
			namespace DataBaseTransformer   {

				/// Идентификаторы баз данных
				enum TDataBaseId   {
					/// Английская база
					BASE_ENGLISH = 0,
					/// Русская база
					BASE_RUSSIAN = 1
				};

				/// Интерфейс для преобразования морфологической базы данных 
				class IDataBaseTransformer : public virtual SS::Interface::IBase   {

					public:
						/// Подготовка базы к преобразованию в DBMS
						/**
							\param char      * pszDataSource            - имя источника данных (например "localhost")
							\param char      * pszUserLogin             - имя пользователя
							\param char      * pszUserPassword          - пароль пользователя
							\param TDataBaseId eDataBaseId              - идентификатор базы данных
							\param _TCHAR    * pszSqlDataBaseNamePrefix - префикс, изменяющий имена баз (пример: "Test01_" приведет к "Test01_MorphoRussian")

						Подготовка базы к преобразованию в DBMS
						*/
						virtual HRESULT Prepare(char * pszDataSource, char * pszUserLogin, char * pszUserPassword, TDataBaseId eDataBaseId, _TCHAR * pszSqlDataBaseNamePrefix = NULL) = 0;

						/// Выгрузка базы из SQL в DBMS
						/**
							\param char      * pszDataSource            - имя источника данных (например "localhost")
							\param char      * pszUserLogin             - имя пользователя
							\param char      * pszUserPassword          - пароль пользователя
							\param TDataBaseId eDataBaseId              - идентификатор базы данных
							\param _TCHAR    * pszSqlDataBaseNamePrefix - префикс, изменяющий имена баз (пример: "Test01_" приведет к "Test01_MorphoRussian")

						Выгрузка базы из SQL в DBMS
						*/
						virtual HRESULT ToDBMS(char * pszDataSource, char * pszUserLogin, char * pszUserPassword, TDataBaseId eDataBaseId, _TCHAR * pszSqlDataBaseNamePrefix = NULL) = 0;

						/// Выгрузка базы из DBMS в SQL
						/**
							\param char      * pszDataSource            - имя источника данных (например "localhost")
							\param char      * pszUserLogin             - имя пользователя
							\param char      * pszUserPassword          - пароль пользователя
							\param TDataBaseId eDataBaseId              - идентификатор базы данных
							\param _TCHAR    * pszSqlDataBaseNamePrefix - префикс, изменяющий имена баз (пример: "Test01_" приведет к "Test01_MorphoRussian")

						Выгрузка базы из DBMS в SQL
						*/
						virtual HRESULT ToSQL(char * pszDataSource, char * pszUserLogin, char * pszUserPassword, TDataBaseId eDataBaseId, _TCHAR * pszSqlDataBaseNamePrefix = NULL) = 0;

				};

			}
		}
	}
}