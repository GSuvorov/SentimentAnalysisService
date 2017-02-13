#pragma once

#include "..\ASSInterface\ICoSeDi.h"
#include ".\CashCoSeDi.h"
#include ".\connect.h"
#include ".\dboSemantic.h"

namespace SS
{
	namespace CoSeDi
	{
		using namespace SS::CoSeDi::DataTypes;

		namespace SQL
		{
			using namespace SS::CoSeDi::SQL::SQL_ACCESS::Semantic;
		
			/// Класс отвечающий за получение и сохранение семантической информации о слове из SQL базы
			class CSemanticAnalysis : 
				public SS::CoSeDi::SQL::CConnect,
				public SS::Interface::Core::CoSeDi::ISemanticAnalysis
			{
			public:
				/// Конструктор
				/** 
					\param SS::CoSeDi::DBMS::CCashCoSeDi*pCashCoSeDi - указатель на кэш
					\param szBaseName - имя базы
				*/
				CSemanticAnalysis(SS::CoSeDi::SQL::CCashCoSeDi*	pCashCoSeDi, const wchar_t* szBaseName) : 
				  SS::CoSeDi::SQL::CConnect(szBaseName)
				{
					m_pCashCoSeDi = pCashCoSeDi;
				};
				/// деструктор
				virtual ~CSemanticAnalysis(void){};
			public: // from SS::Interface::Core::CoSeDi::ISemanticAnalysis
				/// возвращает номера семантических классов по индексу слова
				/** 
					\param oTFormInfo - полный индекс слова
					\param p_list_OfClass - указатель на список индексов семантических классов
				*/
				bool	GetClasses(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex, /*out*/ std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_Class);

				/// возвращает список слов, которые относятся к текущему семантическому классу
				/** 
					\param p_list_FormInfo - указатель на список, содержащий слова, относящиеся к семантическому классу
				*/
				void	GetWords(/*out*/ std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_DictionaryIndex);	

				/// Добавляет слова к текущему семантическому классу
				/** 
					\param oTFormInfo - все координаты слова, которое необходимо добавить
					\return возращает результат выполнения операции
				*/
				bool	AddWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex);

				/// удаляет слово, относящееся к текущему семантическому классу и содеражащиеся в TFormInfo
				/** 
					\param oTFormInfo - все координаты слова, которое необходимо удалить
					\return bool	- результат операции
				*/
				bool	DeleteWord (/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex);
				
				/// удаляет текущий семантический класс 
				bool	DeleteClass(SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber oDictionariesNumber);

				/// Создает класс и добавляет к нему слово
				/** 
					\param oTFormInfo - все координаты слова, которое необходимо добавить
					\return возращает номер созданного класса
				*/
				int	CreateClassAndAddWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex);

			public: // function
				/// устанавливает указатель на актиный менеджер соединений
				/** 
					\param pLoadManager - указатель на актиный менеджер соединений
					\return void
				*/				
				void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
				{
					SS_TRY;
	
					SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);
					InitConnection();

					SS_CATCH(L"");
				}
			private:
				/*! \var SS::CoSeDi::SQL::CCashCoSeDi								* m_pCashCoSeDi
				*  \brief	- указатель на кэш     
				*/
				SS::CoSeDi::SQL::CCashCoSeDi				* m_pCashCoSeDi;	
			
			};

						/// Класс отвечающий за получение и сохранение семантической информации о слове из SQL базы
			class CSemanticAnalysisSynonims : 
				public SS::CoSeDi::SQL::CConnect,
				public SS::Interface::Core::CoSeDi::ISemanticAnalysis
			{
			public:
				/// Конструктор
				/** 
					\param SS::CoSeDi::DBMS::CCashCoSeDi*pCashCoSeDi - указатель на кэш
					\param szBaseName - имя базы
				*/
				CSemanticAnalysisSynonims(SS::CoSeDi::SQL::CCashCoSeDi*	pCashCoSeDi, const wchar_t* szBaseName) : 
				  SS::CoSeDi::SQL::CConnect(szBaseName)
				{
					m_pCashCoSeDi = pCashCoSeDi;
				};
				/// деструктор
				virtual ~CSemanticAnalysisSynonims(void){};
			public: // from SS::Interface::Core::CoSeDi::ISemanticAnalysis
				/// возвращает номера семантических классов по индексу слова
				/** 
					\param oTFormInfo - полный индекс слова
					\param p_list_OfClass - указатель на список индексов семантических классов
				*/
				bool	GetClasses(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex, /*out*/ std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_Class);

				/// возвращает список слов, которые относятся к текущему семантическому классу
				/** 
					\param p_list_FormInfo - указатель на список, содержащий слова, относящиеся к семантическому классу
				*/
				void	GetWords(/*out*/ std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_DictionaryIndex);	

				/// Добавляет слова к текущему семантическому классу
				/** 
					\param oTFormInfo - все координаты слова, которое необходимо добавить
					\return возращает результат выполнения операции
				*/
				bool	AddWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex);

				/// удаляет слово, относящееся к текущему семантическому классу и содеражащиеся в TFormInfo
				/** 
					\param oTFormInfo - все координаты слова, которое необходимо удалить
					\return bool	- результат операции
				*/
				bool	DeleteWord (/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex);
				
				/// удаляет текущий семантический класс 
				bool	DeleteClass(SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber oDictionariesNumber);

				/// Создает класс и добавляет к нему слово
				/** 
					\param oTFormInfo - все координаты слова, которое необходимо добавить
					\return возращает номер созданного класса
				*/
				int	CreateClassAndAddWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex);


			public: // function
				/// устанавливает указатель на актиный менеджер соединений
				/** 
					\param pLoadManager - указатель на актиный менеджер соединений
					\return void
				*/				
				void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
				{
					SS_TRY;
	
					SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);
					InitConnection();

					SS_CATCH(L"");
				}
			private:
				/*! \var SS::CoSeDi::SQL::CCashCoSeDi								* m_pCashCoSeDi
				*  \brief	- указатель на кэш     
				*/
				SS::CoSeDi::SQL::CCashCoSeDi				* m_pCashCoSeDi;	
			
			};

		};
		namespace DBMS
		{
			/// Класс отвечающий за получение и сохранение семантической информации о слове из DBMS базы
			class CSemanticAnalysis : 
				public SS::Interface::Core::CoSeDi::ISemanticAnalysis,
				public SS::CoSeDi::DBMS::CConnect
			{
			public:
				/// Конструктор
				/** 
					\param SS::CoSeDi::DBMS::CCashCoSeDi*pCashCoSeDi - указатель на кэш
					\param szBaseName - имя базы
				*/
				CSemanticAnalysis(SS::CoSeDi::DBMS::CCashCoSeDi* pCashCoSeDi, const wchar_t* szBaseName, std::wstring wTableName) :
				  SS::CoSeDi::DBMS::CConnect(szBaseName), m_wDataTableName(wTableName)
				{
					m_pCashCoSeDi = pCashCoSeDi;
					m_SSemanticsAccess.DictionaryNumber = 0;
					m_SSemanticsAccess.IDClass = 0;
					m_SSemanticsAccess.IDSource = 0;
					TableRecord[0] = NULL;
					TableRecord[1] = NULL;
					TableRecord[2] = NULL;
				};
				/// деструктор
				virtual ~CSemanticAnalysis(void)
				{
				};
			public: // function
				/// устанавливает указатель на актиный менеджер соединений
				/** 
					\param pLoadManager - указатель на актиный менеджер соединений
					\return void
				*/				
				void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
				{
					SS_TRY
					{
						SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);
						InitConnection();
					}
					SS_CATCH(L"")
					
				}

			public: // from SS::Interface::Core::CoSeDi::ISemanticAnalysis				

				/// возвращает номера семантических классов по индексу слова
				/** 
					\param oTFormInfo - полный индекс слова
					\param p_list_OfClass - указатель на список индексов семантических классов
				*/
				bool	GetClasses(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex, /*out*/ std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_Class);

				/// возвращает список слов, которые относятся к текущему семантическому классу
				/** 
					\param p_list_FormInfo - указатель на список, содержащий слова, относящиеся к семантическому классу
				*/
				void	GetWords(/*out*/ std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_DictionaryIndex);	

				/// Добавляет слова к текущему семантическому классу
				/** 
					\param oTFormInfo - все координаты слова, которое необходимо добавить
					\return возращает результат выполнения операции
				*/
				bool	AddWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex);

				/// удаляет слово, относящееся к текущему семантическому классу и содеражащиеся в TFormInfo
				/** 
					\param oTFormInfo - все координаты слова, которое необходимо удалить
					\return bool	- результат операции
				*/
				bool	DeleteWord (/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex);
				
				/// удаляет текущий семантический класс 
				bool	DeleteClass(SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber oDictionariesNumber);

				/// Создает класс и добавляет к нему слово
				/** 
					\param oTFormInfo - все координаты слова, которое необходимо добавить
					\return возращает номер созданного класса
				*/
				int	CreateClassAndAddWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex);

			
			private:
				/// Инициализация соединений с таблицами				
				bool InitConnection();				
			private:
				/*! \var SS::CoSeDi::SQL::CCashCoSeDi								* m_pCashCoSeDi
				*  \brief	- указатель на кэш     
				*/
				SS::CoSeDi::DBMS::CCashCoSeDi	* m_pCashCoSeDi;				

				SSemanticsAccess m_SSemanticsAccess;
				/*! \var void       * TableRecord[3];
				*  \brief     массив указателей на данные, являющиеся полями в DBMS таблице
				*/
				void* TableRecord[3];
				/*! \enum EFindMode
				*  \brief     определят поле, по которому будет осуществляться поиск
				*/
				enum EFindMode
				{
					efmByIDClass,
					efmByIDSource
				};

				/*	имя таблицы	*/
				std::wstring m_wDataTableName;		
			};
		};
	};
};

