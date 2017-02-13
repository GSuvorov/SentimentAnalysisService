#pragma once

#include "..\ASSInterface\ICoSeDi.h"

#include ".\connect.h"

#include ".\CashCoSeDi.h"

#include ".\dboCombinatory.h"

#include "..\ASSInterface\TCoSeDiDataTypes.h"

#include "../ASCInterface/IDBMS.h"
#include "..\ASSInterface\TDictionaryNames.h"




namespace SS
{
	namespace CoSeDi
	{
		namespace SQL
		{
			/// класс реализующий интерфейсы комбинаторного анализа для SQL
			class CCombinatoryAnalysis : 
				public SS::CoSeDi::SQL::CConnect,
				public SS::Interface::Core::CoSeDi::ICombinatoryAnalysis				
			{
			public:
				/// конструктор
				CCombinatoryAnalysis(SS::CoSeDi::SQL::CCashCoSeDi*	pCashCoSeDi, const wchar_t* szBaseName) : 
				  SS::CoSeDi::SQL::CConnect(szBaseName)
				{
					m_pCashCoSeDi = pCashCoSeDi;					
				}
				/// деструктор
				virtual ~CCombinatoryAnalysis(void){};			
			public: // from SS::CoSeDi::Intrefaces::ICombinatoryAnalysis
				/// возвращает список комбинаций для поступившего слова
				/** 
					\param oFormInfo - координаты слова, для которого необходимо выполнить
					\param p_list_CombinationList - указатель на список конструкций
					\return bool - результат выполнения операции
				*/
				bool	GetCombinations(/*in*/ SS::CoSeDi::DataTypes::TFormInfo & oFormInfo, /*out*/SS::CoSeDi::DataTypes::TlistOfCombination * p_list_CombinationList);
				
				/// создает комбинацию и возвращает ее номер
				/** 
					\param p_CombinationInfo - указатель на создаваемую комбинацию
					\return 
				*/
				int		CreateCombination(/*in*/ SS::CoSeDi::DataTypes::TCombinationInfo * p_CombinationInfo);
				
				/// возвращает для текущей комбинации все ее элементы
				/** 
					\param p_CombinationInfo - указатель на структуру, хранящую полную информацию о комбинации
					\return 
				*/
				void	GetCombinationInfo(/*out*/ SS::CoSeDi::DataTypes::TCombinationInfo * p_CombinationInfo);	

				/// удаляет текущую комбинацию из базы
				bool	DeleteCombination();
			public: // function
				/// устанавливает указатель на актиный менеджер соединений
				/** 
					\param pLoadManager - указатель на актиный менеджер соединений
				*/				
				void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
				{
					SS_TRY;

					SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);
					InitConnection();

					SS_CATCH(L"");
					return ;
				}
			private: // function
				/// возвращает последний используемый индекс комбинации
				int GetLastCombinationID();
			private: // variables				
				/*! \var SS::CoSeDi::SQL::CCashCoSeDi					* m_pCashCoSeDi
				*  \brief	- указатель на кэш     
				*/
				SS::CoSeDi::SQL::CCashCoSeDi								* m_pCashCoSeDi;
				
				/*! \var SS::CoSeDi::SQL::SQL_ACCESS::CPreparedSMaxTCombinatory	* pSMaxTCombinatory
				*  \brief     - указатель на класс, получающий максимальное значение ID из комбинаторики
				*/
				SS::CoSeDi::SQL::SQL_ACCESS::Combinatory::CPreparedSMaxTCombinatory	* pSMaxTCombinatory;
				/*! \var SS::CoSeDi::SQL::SQL_ACCESS::Combinatory::CPreparedSMaxTCombinatory	* pSMaxTCombinatory
				*  \brief     - указатель на класс, вставляющий значения в комбинаторику
				*/
				SS::CoSeDi::SQL::SQL_ACCESS::Combinatory::CPreparedITCombinatory		* pITCombinatory;
				/*! \var SS::CoSeDi::SQL::SQL_ACCESS::Combinatory::CPreparedSMaxTCombinatory	* pSMaxTCombinatory
				*  \brief     - указатель на класс, получающий комбинаторную информацию из базы
				*/
				SS::CoSeDi::SQL::SQL_ACCESS::Combinatory::CPreparedSTCombinatoryInfo	* pSTCombinatoryInfo;
				/*! \var SS::CoSeDi::SQL::SQL_ACCESS::Combinatory::CPreparedSMaxTCombinatory	* pSMaxTCombinatory
				*  \brief     - указатель на класс, получающий все комбинации из базы
				*/
				SS::CoSeDi::SQL::SQL_ACCESS::Combinatory::CPreparedSAllCombination	* pSAllCombination;

			};
		};
		namespace DBMS
		{

			/// класс реализующий интерфейсы комбинаторного анализа для DBMS
			class CCombinatoryAnalysis : 
				public SS::CoSeDi::DBMS::CConnect,
				public SS::Interface::Core::CoSeDi::ICombinatoryAnalysis	
			{
			public:
				/// конструктор
				CCombinatoryAnalysis(SS::CoSeDi::DBMS::CCashCoSeDi*	pCashCoSeDi, const wchar_t* szBaseName) :
					SS::CoSeDi::DBMS::CConnect(szBaseName)
				{
					m_pCashCoSeDi = pCashCoSeDi;
				}
				/// деструктор
				virtual ~CCombinatoryAnalysis(void)
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
					SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);
					InitConnection();
				}

			public: // from SS::CoSeDi::Intrefaces::ICombinatoryAnalysis
				/// возвращает список комбинаций для поступившего слова
				/** 
					\param oFormInfo - координаты слова, для которого необходимо выполнить
					\param p_list_CombinationList - указатель на список конструкций
					\return bool - результат выполнения операции
				*/
				bool	GetCombinations(/*in*/ SS::CoSeDi::DataTypes::TFormInfo & oFormInfo, /*out*/SS::CoSeDi::DataTypes::TlistOfCombination * p_list_CombinationList);
				
				/// создает комбинацию и возвращает ее номер
				/** 
					\param p_CombinationInfo - указатель на создаваемую комбинацию
					\return 
				*/
				int		CreateCombination(/*in*/ SS::CoSeDi::DataTypes::TCombinationInfo * p_CombinationInfo);
				
				/// возвращает для текущей комбинации все ее элементы
				/** 
					\param p_CombinationInfo - указатель на структуру, хранящую полную информацию о комбинации
					\return 
				*/
				void	GetCombinationInfo(/*out*/ SS::CoSeDi::DataTypes::TCombinationInfo * p_CombinationInfo);	

				/// удаляет текущую комбинацию из базы
				bool	DeleteCombination();
			private:
				/// Закрытие соединений с таблицами				
				bool InitConnection(void);
			private:
				/*! \var SS::CoSeDi::SQL::CCashCoSeDi								* m_pCashCoSeDi
				*  \brief	- указатель на кэш     
				*/
				SS::CoSeDi::DBMS::CCashCoSeDi*	m_pCashCoSeDi;
				
				/*! \var SS::CoSeDi::DataTypes::SCombinatoryAccess m_SCombinatoryAccess
				*  \brief     Структура, служащая аксессором для доступа к DBMS таблице
				*/
				SS::CoSeDi::DataTypes::SCombinatoryAccess m_SCombinatoryAccess;
				/*! \var void       * TableRecord[5];
				*  \brief     массив указателей на данные, являющиеся полями в DBMS таблице
				*/
				void       * TableRecord[7];
				/*! \enum EFindMode
				*  \brief     определят поле, по которому будет осуществляться поиск
				*/
				enum EFindMode
				{
					efmByID,
					efmByIDSource
				};

				
			};
		};
	
	};
};
