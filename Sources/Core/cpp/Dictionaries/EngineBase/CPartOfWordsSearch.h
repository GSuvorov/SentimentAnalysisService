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

#include "IPartOfWordsSearch.h"

using namespace SS::Dictionary::Types;
using namespace SS::Dictionary::Core;
using namespace SS::Interface::Core;

namespace SS{ 
namespace Dictionary{
namespace Core{

		class CPartOfWordsSearch : public IPartOfWordsSearch, public OBJECT_LEVEL_LOCKABLE_SYNC_DEP<CPartOfWordsSearch, SCriticalSectionImplStategy, CURRENT_THREAD_DEBUG_INFO>   {

			public:

				/// Конструктор
				CPartOfWordsSearch(VOID);

				/// Конструктор вызывающий Init()
				CPartOfWordsSearch(
					/// Путь к файлам данных
					CONST _TCHAR * pszPathToDataFiles,
					/// Указатель на интерфейс таблицы IDataTable (для вызова Create())
					DBMS::IDataTable * pIDataTable);

				/// Инициализация путь к файлам данных и указатель на интерфейс таблицы IDataTable (для вызова Create())
				BOOL Init(
					/// Путь к файлам данных
					CONST _TCHAR * pszPathToDataFiles,
					/// Указатель на интерфейс таблицы IDataTable (для вызова Create())
					DBMS::IDataTable * pIDataTable);

				/// Деструктор
				~CPartOfWordsSearch(VOID);

				/// Cоздает файлы, содержащие дерево и массив поиска
				BOOL Create(VOID);

				/// Загружает/перезагружает словарь
				BOOL Load(VOID);

            //Если в указанном каталоге нет файлов с деревом то вызывается Create, иначе Load;
            BOOL LoadIfFilesExists();

            /// Загружает словарь только если он ещё не был загружен
            BOOL LoadIfNeed();

            //Загружены ли данные
            BOOL Empty(){ return !m_pTreeData && !m_pIdData; }

				/// Освобождает память
				BOOL Clear(VOID);

				/// Поиск в словаре всех возможных гипотез о слове, при этом гипотезой считается любое слово, у которого с искомым словом совпадают несколько символов с конца.
				BOOL GetHypothesis(
					/// Входное слово для поиска
					IN CONST WCHAR * pwsPartOfWord,
					/// Позиция в слове, с которой необходимо начать поиск;  т.е. если необходимо найти слово “abc“ индекс будет 2, “ab” – 1 и т.д.
					IN INT iBeginPosition,
					/// Список выходных данных, перед вызовом необходимо вызвать reserve
					OUT std::vector< SS::Dictionary::Types::SHypothesisInfo > * p_v_HypothesisInfo,
					/// Минимальное количество совпадений, при котором слово записывается в <p_v_SHypothesisInfo>
					IN INT iMinMatchSymbols);

				/// Ищет все возможные пары частей слов и их индексов
				BOOL GetByForm(
					/// Входное слово для поиска
					IN CONST WCHAR * pwsPartOfWord,
					/// Позиция в слове, с которой необходимо начать поиск
					IN INT iBeginPosition,
					/// Список выходных данных, перед вызовом необходимо вызвать reserve
					OUT std::vector< SPartOfWord > * p_v_PartOfWord,
					/// Критерий поиска: до начала слова или нет
					IN BOOL isCompleteWord = FALSE);

				/// Ищет часть слова по индексу
				BOOL GetFormById(
					/// Индекс для поиска
					IN UINT iID, 
					/// Найденная часть слова
					OUT std::wstring * pwsPartOfWord);

			private:

				BYTE                    * m_pTreeData;
				UINT                      m_iTreeDataSize;
				BYTE                    * m_pTPtr;
				BYTE                    * m_pTLastPtr;

				BYTE                    * m_pIdData;
				UINT                      m_iIdDataSize;
				UINT32                  * m_pIdArr;
				UINT                      m_iIdNum;

				SPartOfWord               m_oPartOfWord;

				SHypothesisInfo                              m_oHypothesisInfo;
				std::vector< SHypothesisInfo >             * m_p_v_HypothesisInfo;
				INT                                          m_iHypLetterIdx;
				INT                                          m_iHypMatchSymbols;

				typedef BYTE *                               HypTreeBranch; 
				typedef std::set < HypTreeBranch >			 HypTreeBranchSet;
				typedef HypTreeBranchSet::const_iterator     HypTreeBranchItr;
				typedef std::pair< HypTreeBranchItr, bool >  HypTreeBranchPair;

				HypTreeBranchSet                             m_oHypTreeBranchSet; 
//				HypTreeBranchItr                             m_oHypTreeBranchItr;
				HypTreeBranchPair                            m_oHypTreeBranchPair;


				std::vector<std::wstring> m_oLfVecor;
				std::vector<UINT32>       m_oIdVecor;

				VOID                      m_fRectifyTreePtrs_RecurFunc(BYTE * pMem);
				VOID                      m_fCollectIdsForHypothesis_RecurFunc(BYTE * pMem, INT iLevel);
				VOID                      m_fCollectIdsForHypothesis_RecurFunc_ForLast(BYTE * pMem);
				BOOL                      m_fRectifyIdArrPtrs_IterFunc(UINT32 * pIdArr);
				VOID                      m_fMakeTreeBranchRecurFunc(UINT iCurNumOfLeters, std::vector<UINT> & roVectOfAllowedIdxs);
				INT                       m_fFindLetterIdx(wchar_t wcLetter, wchar_t * pLettersArr, UINT16 iLettersNum);
				BOOL                      m_fReadData(VOID);

				STLSTR                    m_sPathToDataFiles;
				DBMS::IDataTable        * m_pIDataTable;

		}; // class CPartOfWordsSearch : 

		/// Короткое МАКРО для __FUNCTION__
		#define _F __FUNCTION__

		namespace PartOfWordsSearchDbg
		{
			/// Запись сообщения об ошибке в журнал системы
			HRESULT LogFail(char * pFuncName, char * pFormatStr, ...);

			/// Запись простого сообщения в журнал системы
			VOID    LogMess(char * pFormatStr, ...);
		}


		/// Предопределенный резерв памяти для создания дерева поиска
		#define TREE_MAX_SIZE   (10*1024*1024-128)

		/// Вспом. макро
		#define MEM_UINT32(pMem)                   *((UINT32 *)(pMem))
		/// Вспом. макро
		#define MEM_UINT16(pMem)                   *((UINT16 *)(pMem))
		/// Вспом. макро
		#define SET_MEM_UINT32_AND_INC(pMem,iVal)  *((UINT32 *)(pMem))=iVal, pMem+=sizeof(UINT32)
		/// Вспом. макро
		#define SET_MEM_UINT16_AND_INC(pMem,iVal)  *((UINT16 *)(pMem))=iVal, pMem+=sizeof(UINT16)


} //namespace SS
} //namespace Dictionary
} //namespace Core




