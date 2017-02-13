#pragma once

#include "..\ASSInterface\ICoSeDi.h"
#include "..\ASSInterface\TCoSeDiDataTypes.h"
#include "..\ASSInterface\ILingvoBaseManager.h"
#include "..\ASSInterface\TDictionaryNames.h"
#include "..\ASCInterface\IDBMS.h"

//#include "..\ASSInterface\TBaseNames.h"

//#include ".\dboCombinatory.h"
//#include ".\CombinatoryAnalysis.h"
//#include ".\cl_CLoadManagerAgent.h"

using namespace SS::Interface::Core::ResourceManagers;
using namespace SS::Interface::Core::CoSeDi;
using namespace SS::Interface::Core::DBMS;
using namespace SS::CoSeDi::DataTypes;

namespace SS   {
	namespace CoSeDi   {
		namespace DBMS   {

			/// Запись сообщения об ошибке в журнал системы
			HRESULT CombiErr(WCHAR * pFormatStr, ...);

			// --------------------------------------------------------------------------------------
			/// Класс CSimpleConnect
			class CSimpleConnect   {

				public:

					/// Конструктор
					CSimpleConnect(void) : m_pDataBase(NULL), m_pDataTable(NULL), m_poLingvoBaseManager(NULL)
					{
						memset(m_wsBaseName, 0, sizeof(wchar_t) * 100);
					}

					/// деструктор
					~CSimpleConnect(void) { CloseConnection(); }

					/// Закрытие соединений с базами			
					void CloseConnection(void);

					/// Инициализация соединений с базами				
					bool InitConnection(ILoadManager * pLoadManager, const wchar_t * pwsBaseName, const wchar_t * pwsTableName);

					/// Возвращает указатель на IDataTable
					IDataTable * GetDataTable(void);

				private:

					/// Указатель на IDataBase
					IDataBase * m_pDataBase;

					/// Указатель на IDataTable
					IDataTable * m_pDataTable;

					/// Указатель на ILingvoBaseManager
					ILingvoBaseManager * m_poLingvoBaseManager;

					wchar_t m_wsBaseName[100];

			};


			// --------------------------------------------------------------------------------------
			/// Класс CCombinatory
			class CCombinatory :	public SS::Interface::Core::CoSeDi::ICombinatory   {

				public:

					/// Конструктор
					CCombinatory() {}

					/// Деструктор
					~CCombinatory(void)   {
						zm_ConnectEng.CloseConnection();
						zm_ConnectRus.CloseConnection();
					}

					BOOL InitConnection(WCHAR * pwszDataTablesName, BOOL bExtraTables);
				
				public: 

					/// Возвращает список комбинаций для поступившего списка наборов IDSource-ов
					/** 
						\param  itrStartItr_In_ListOfSetsOfSourcesAndForms - стартовый итератор списка
						\param  iNumOfWordsToProcess                       - число наборов IDSource-ов для обработки
						\param  p_listCombinationInfo                      - выходной список информации о комбинациях
						\return bool                                       - результат выполнения операции
					*/
					bool GetCombinationsInfo(
						/*in */ TListOfSetsOfSourcesAndFormsItr itrStartItr_In_ListOfSetsOfSourcesAndForms,
						/*in */ UINT iNumOfWordsToProcess,
						/*out*/ std::list<SCombinationNewInfo> * p_listCombinationInfo);

					/// Возвращает список комбинаций для поступившего списка наборов IDSource-ов (Mode 2 : порядок слов не важен)
					/** 
						\param  itrStartItr_In_ListOfSetsOfSourcesAndForms - стартовый итератор списка
						\param  iNumOfWordsToProcess                       - число наборов IDSource-ов для обработки
						\param  p_listCombinationInfo                      - выходной список информации о комбинациях
						\return bool                                       - результат выполнения операции
					*/
					bool GetCombinationsInfo_2(
						/*in */ TListOfSetsOfSourcesAndFormsItr itrStartItr_In_ListOfSetsOfSourcesAndForms,
						/*in */ UINT iNumOfWordsToProcess,
						/*out*/ std::list<SCombinationNewInfo> * p_listCombinationInfo);


					/// Возвращает список комбинаций для поступившего списка наборов IDSource-ов
					/** 
						\param  itrBegin_ListOfSetsOfSourcesAndForms - стартовый итератор списка
						\param  itrEnd_ListOfSetsOfSourcesAndForms   - конечный итератор списка
						\param  p_listCombinationInfo          - выходной список структур информациЙ о комбинациях
						\return bool                           - результат выполнения операции
					*/
					bool GetCombinationsInfo(
						/*in */ TListOfSetsOfSourcesAndFormsItr itrBegin_ListOfSetsOfSourcesAndForms,
						/*in */ TListOfSetsOfSourcesAndFormsItr itrEnd_ListOfSetsOfSourcesAndForms,
						/*out*/ std::list<SCombinationNewInfo> * p_listCombinationInfo);

					/// Возвращает список комбинаций для поступившего списка наборов IDSource-ов (Mode 2 : порядок слов не важен)
					/** 
						\param  itrBegin_ListOfSetsOfSourcesAndForms - стартовый итератор списка
						\param  itrEnd_ListOfSetsOfSourcesAndForms   - конечный итератор списка
						\param  p_listCombinationInfo          - выходной список структур информациЙ о комбинациях
						\return bool                           - результат выполнения операции
					*/
					bool GetCombinationsInfo_2(
						/*in */ TListOfSetsOfSourcesAndFormsItr itrBegin_ListOfSetsOfSourcesAndForms,
						/*in */ TListOfSetsOfSourcesAndFormsItr itrEnd_ListOfSetsOfSourcesAndForms,
						/*out*/ std::list<SCombinationNewInfo> * p_listCombinationInfo);
					
					/// Возвращает список комбинации, содержащих заданный IDSource
					/** 
						\param iIDSource              - заданный IDSource
						\param  p_listCombinationsIDs - выходной список индексов комбинаций
						\return bool                  - результат выполнения операции
					*/
					bool GetCombinationsByIDSource(/*in*/int iIDSource, /*out*/ std::list<int> * p_listCombinationsIDs);	
					
					/// Возвращает информацию о комбинации
					/** 
						\param iIDCombination    - номер комбинации
						\param p_CombinationInfo - указатель на структуру, хранящую информацию о комбинации
						\return bool             - результат выполнения операции
					*/
					bool GetCombinationInfo(/*in*/int iIDCombination, /*out*/ SCombinationNewInfo * p_CombinationInfo);	

					/// Создает комбинацию
					/** 
						\param p_CombinationInfo - указатель на структуру, содержащую информацию о комбинации
						\return bool             - результат выполнения операции
					*/
					bool CreateCombination(/*in out*/ SCombinationNewInfoForAdd * p_CombinationInfo);			

					/// Удаляет комбинацию
					/** 
						\param iIDCombination - номер комбинации
						\return bool          - результат выполнения операции
					*/
					bool DeleteCombination(/*in*/int iIDCombination);

				public:

					/// Работа связанная с ILoadManager
					void SetLoadManager(ILoadManager * pLoadManager);

				private:

					/// Поле поиска в DBMS таблицах 
					enum EFindMode   {
						efmByID,
						efmByHashKey,
						efmByHashKey_2,
						efmByIDSource
					};

					ILoadManager                     * zm_pLoadManager;
					BOOL                               m_bExtraTables;

					BOOL                               zm_fRecursiveFunc(  UINT iLevel);
					BOOL                               zm_fRecursiveFunc_2(UINT iLevel);
					BOOL                               zm_fCheckCombination(  VOID);
					BOOL                               zm_fCheckCombination_2(VOID);

					SCombinationNewInfo                zm_oCombinationInfo;
					VOID                             * zm_apTableRecord[16];
					
					TListOfPtrsToSetsOfSourcesAndForms       zm_SubListForCheck;
					TListOfPtrsToSetsOfSourcesAndFormsItr    zm_SubListForCheckItr;

					UINT                               zm_SubListForCheckSise;  
					UINT32                             zm_SubListIDSourceArr[MAX_NUM_OF_COMBINATION_MEMBERS];
					UINT32                             zm_SubListIDFormArr[  MAX_NUM_OF_COMBINATION_MEMBERS];

					CSimpleConnect                     zm_ConnectEng;
					CSimpleConnect                     zm_ConnectRus;

					TSetOfHashKeys                     zm_setExistingHashKeys;
					TSetOfHashKeys                     zm_setExistingHashKeys_2;
					TSetOfSourcesAndForms              zm_setExistingFirstSourcesAndForms;

					UINT32                             zm_fGetHashKey_2(UINT32 aiDataArr[]);

					std::list<SCombinationNewInfo>   * zm_plistCombinationInfo;
					UINT                               zm_iPosInClauseCnt;

			}; 

		} // namespace DBMS   {
	} // namespace CoSeDi   {
} // namespace SS
