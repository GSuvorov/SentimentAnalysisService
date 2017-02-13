#pragma once

//	Windows Header Files

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <wchar.h>
#include <ctype.h>
#include <time.h>
#include <string>

//	SQL Std
#include <atldbcli.h>
#include <sqloledb.h>

//	STL
#include <algorithm>
#include <hash_map>
#include <hash_set>
#include <utility>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>

#include <iomanip>
#include <iostream>

#include "IMorphoTypesSearch.h"

using namespace SS::Dictionary::Types;
using namespace SS::Dictionary::Core;
using namespace SS::Interface::Core;

namespace SS{ 
namespace Dictionary{
namespace Core{


		class CMorphoTypesSearch : public IMorphoTypesSearch, public OBJECT_LEVEL_LOCKABLE_SYNC_DEP<CMorphoTypesSearch, SCriticalSectionImplStategy, CURRENT_THREAD_DEBUG_INFO>   {

			public:

				/// Конструктор
				CMorphoTypesSearch(VOID);

				/// Деструктор
				~CMorphoTypesSearch(VOID);

				/// Инициализация путь к файлам данных и указатель на интерфейс базы IDataBase (для вызова Load())
				BOOL Init(
					/// Путь к файлам данных
					IN  CONST _TCHAR * pszPathToDataFiles,
					/// Указатель на интерфейс базы IDataTable (для вызова Load())
					IN  SS::Interface::Core::DBMS::IDataBase * pIDataBase);

				/// Загружает/перезагружает словарь
				BOOL Load(VOID);

            /// Загружает словарь только если он ещё не был загружен
            BOOL LoadIfNeed();
            
            //Загружены ли данные
            BOOL Empty(){ return m_oIDTypeMap.empty() ; }

				/// Освобождает память
				BOOL Clear(VOID);

			public:

				/// добавляет часть информации для последующего поиска
				/** 
					\param oIDType - номер типа морфообразования
					\param oIDForm - номер окончания
					\param oIDEnding - индекс окончания
					\param oMorphoInfo - закодированная основная морфологическая информация
					\param oSpecialMorpho - закодированная дополнительная морфологическая информация
					\return bool - результат операции
				*/
				BOOL AddInfo(
					IN  SS::Dictionary::Types::TIDType        oIDType,
					IN  SS::Dictionary::Types::TMorphoInfo    oMorphoInfo, 
					IN  SS::Dictionary::Types::TIDForm        oIDForm, 	
					IN  SS::Dictionary::Types::TIDEnding      oIDEnding, 
					IN  SS::Dictionary::Types::TSpecialMorpho oSpecialMorpho);

				/// осуществляет поиск информации об окончании по индексу литерального представления
				/** 
					\param oIDType - номер типа морфообразования
					\param oIDEnding - индекс окончания
					\param p_l_MorphoType - указатель на список структур, содержащих полную информацию об окончании
					\return bool - результат операции
				*/
				BOOL GetByForm(
					IN  SS::Dictionary::Types::TIDType oIDType, 
					IN  SS::Dictionary::Types::TIDEnding oIDEnding, 
					OUT std::list<SS::Dictionary::Types::SMorphoType> * p_l_MorphoType);

				/// осуществляет поиск информации об индексе литерального представления по индексу словоформы
				/** 
					\param oIDType	- тип морфообразования
					\param oIDForm	- индекс окончания
					\param pIDEnding - индекс окончания
					\return bool - результат операции
				*/
				BOOL GetIDEndingByIDTypeAndIDForm(
					IN  SS::Dictionary::Types::TIDType oIDType, 
					IN  SS::Dictionary::Types::TIDForm oIDForm, 
					OUT SS::Dictionary::Types::TIDEnding * pIDEnding);

				/// осуществляет поиск морфологической информации по индексу окончания в типе морфообразования 
				/** 
					\param oIDType - тип морфообразования
					\param oIDForm - номер окончания
					\param pMorphoType - указатель на список структур, содержащих полную информацию об окончании
					\return bool - результат операции
				*/
				BOOL GetMorphoTypeByIDTypeAndIDForm(
					IN  SS::Dictionary::Types::TIDType oIDType, 
					IN  SS::Dictionary::Types::TIDForm oIDForm, 
					OUT SS::Dictionary::Types::SMorphoType * pMorphoType);

				/// осуществляет поиск информации о всех окончаниях в типе морфообразования
				/** 
					\param oIDType - тип морфообразования
					\param p_l_MorphoType - указатель на список структур, содержащих полную информацию об окончании
					\return bool - результат операции
				*/
				BOOL GetMorphoTypesListByIDType(
					IN  SS::Dictionary::Types::TIDType oIDType, 
					OUT std::list<SS::Dictionary::Types::SMorphoType> * p_l_MorphoType);

				/// осуществляет поиск морфологической информации по типу морфообразования
				/** 
					\param oIDType - тип морфообразования
					\param oIDForm - номер окончания
					\param oIDType - тип морфообразования
					\param pMorphoInfo - закодированная основная морфологическая информация
					\param pSpecialMorpho - закодированная дополнительная морфологическая информация
					\return bool - результат операции
				*/
				BOOL GetMorphoInfoByIDType(
					IN  SS::Dictionary::Types::TIDType oIDType, 
					IN  SS::Dictionary::Types::TIDForm oIDForm, 
					OUT SS::Dictionary::Types::TMorphoInfo * pMorphoInfo, 
					OUT SS::Dictionary::Types::TSpecialMorpho * pSpecialMorpho);


            /// Находит по IDType соотвествующий MorphoInfo
            /// Шестеркин Дмитрий 12.04.05
            virtual BOOL GetMorphoInfoByIDType( 
               IN   SS::Dictionary::Types::TIDType oIDType,
               OUT SS::Dictionary::Types::TMorphoInfo *pMorphoInfo
               );

			private:

				typedef struct   {
				//	TIDForm              m_oIDForm;
					TIDEnding            m_oIDEnding;
					TSpecialMorpho       m_oSpecialMorpho;
				} TMorFormInfo;

				typedef std::vector< TMorFormInfo >                   TMorFormInfoVect;
				typedef TMorFormInfoVect::const_iterator              TMorFormInfoItr;

				typedef std::vector< TIDForm >                        TIDFormVect;
				typedef TIDFormVect::const_iterator                   TIDFormItr;

				typedef stdext::hash_map < TIDEnding, TIDFormVect >   TEndingsAndFormsMap;
				typedef std::pair        < TIDEnding, TIDFormVect >   TEndingsAndFormsPair;
				typedef TEndingsAndFormsMap::const_iterator           TEndingsAndFormsItr;
				typedef std::pair< TEndingsAndFormsItr, bool >        TEndingsAndFormsRet;

				typedef struct   {
					TMorphoInfo          m_oMorphoInfo;
					TMorFormInfoVect     m_oMorFormInfoVect;
					TEndingsAndFormsMap  m_oEndingsAndFormsMap;
				} TIDTypeInfo;

				typedef stdext::hash_map < TIDType, TIDTypeInfo >     TIDTypeMap;
				typedef std::pair        < TIDType, TIDTypeInfo >     TIDTypePair;
 				typedef TIDTypeMap::const_iterator                    TIDTypeItr;
				typedef std::pair< TIDTypeItr, bool >                 TIDTypeRet;

				TIDTypeMap                     m_oIDTypeMap; 
				TIDTypeItr                     m_oIDTypeItr;
				TIDTypeRet                     m_oIDTypeRet;

				TEndingsAndFormsItr            m_oEndingsAndFormsItr; 
				TEndingsAndFormsRet            m_oEndingsAndFormsRet;

				TIDTypeInfo                    m_oIDTypeInfoEmpty;
				TIDFormVect                    m_oIDFormVectEmpty;
				TMorFormInfo                   m_oMorFormInfoEmpty;

				STLSTR                         m_sPathToDataFiles;
				DBMS::IDataBase              * m_pIDataBase;

		}; // class CMorphoTypesSearch : 

		/// Короткое МАКРО для __FUNCTION__
		#define _F __FUNCTION__

		namespace CMorphoTypesSearchDbg
		{
			/// Запись сообщения об ошибке в журнал системы
			HRESULT LogFail(char * pFuncName, char * pFormatStr, ...);

			/// Запись простого сообщения в журнал системы
			VOID    LogMess(char * pFormatStr, ...);
		}

} //namespace SS
} //namespace Dictionary
} //namespace Core





