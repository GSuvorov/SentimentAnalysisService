#pragma once

#include "..\ASSInterface\ICoSeDi.h"
#include ".\CashCoSeDi.h"
#include ".\connect.h"
#include ".\dboSyntax.h"


namespace SS
{
	namespace CoSeDi
	{
		namespace SQL
		{
			/// класс реализующий работу с базой хранящей синтаксическую информацию
			class CSyntaxAnalysis :
				public SS::CoSeDi::SQL::CConnect,
				public SS::Interface::Core::CoSeDi::ISyntaxAnalysis
			{
			public:
				/// деструктор
				virtual ~CSyntaxAnalysis(void)
				{
				};
				/// Конструктор
				/** 
					\param SS::CoSeDi::DBMS::CCashCoSeDi*pCashCoSeDi - указатель на кэш
					\param szBaseName - имя базы
				*/
				CSyntaxAnalysis(SS::CoSeDi::SQL::CCashCoSeDi*	pCashCoSeDi, const wchar_t* szBaseName) : SS::CoSeDi::SQL::CConnect(szBaseName)   
				{
					m_pCashCoSeDi = pCashCoSeDi;
				};
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
			public: // from SS::CoSeDi::Intrefaces::ISyntaxAnalysis
								/// возвращает список, содержащий синтаксические характеристики для текущей основы
				/** 
					\param p_list_OfSyntaxInfo - указатель на список, содержащий синтаксические характеристики для текущей основы
				*/
				void Get(/*out*/std::list<SS::CoSeDi::DataTypes::TSyntaxInfo>* p_list_OfSyntaxInfo);

				/// добавляет список, содержащий синтаксические характеристики для текущей основы
				/** 
					\param p_list_OfSyntaxInfo - указатель на список, содержащий синтаксические характеристики для текущей основы
				*/
				void Add(/*in*/std::list<SS::CoSeDi::DataTypes::TSyntaxInfo>* p_list_OfSyntaxInfo);

				/// удаляет всю синтаксическую информацию для текущей основы
				void Delete();

				/// удаляет список, содержащий синтаксические характеристики для текущей основы
				/** 
					\param p_list_OfSyntaxInfo - указатель на список, содержащий синтаксические характеристики для текущей основы
				*/
				void Delete(/*in*/std::list<SS::CoSeDi::DataTypes::TSyntaxInfo>* p_list_OfSyntaxInfo);
				
			private:
				/*! \var SS::CoSeDi::SQL::CCashCoSeDi								* m_pCashCoSeDi
				*  \brief	- указатель на кэш     
				*/
				SS::CoSeDi::SQL::CCashCoSeDi				* m_pCashCoSeDi;							
			};
		};

		namespace DBMS
		{
			/// класс реализующий работу с базой хранящей синтаксическую информацию
			class CSyntaxAnalysis : 
				public SS::CoSeDi::DBMS::CConnect,
				public SS::Interface::Core::CoSeDi::ISyntaxAnalysis
			{
			public:
				/// Конструктор
				/** 
					\param SS::CoSeDi::DBMS::CCashCoSeDi*pCashCoSeDi - указатель на кэш
					\param szBaseName - имя базы
				*/
				CSyntaxAnalysis(SS::CoSeDi::DBMS::CCashCoSeDi*	pCashCoSeDi, const wchar_t* szBaseName) : 
				  SS::CoSeDi::DBMS::CConnect(szBaseName)
				{
					m_pCashCoSeDi = pCashCoSeDi;
					m_SSyntaxAccess.DictionaryNumber = 0;
					m_SSyntaxAccess.IDSource = 0;
					m_SSyntaxAccess.SyntaxInfo = 0;
					TableRecord[0] = NULL;
					TableRecord[1] = NULL;
					TableRecord[2] = NULL;
				};
				/// деструктор
				virtual ~CSyntaxAnalysis(void)
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
			public: // from SS::CoSeDi::Intrefaces::ISyntaxAnalysis
								/// возвращает список, содержащий синтаксические характеристики для текущей основы
				/** 
					\param p_list_OfSyntaxInfo - указатель на список, содержащий синтаксические характеристики для текущей основы
				*/
				void Get(/*out*/std::list<SS::CoSeDi::DataTypes::TSyntaxInfo>* p_list_OfSyntaxInfo);

				/// добавляет список, содержащий синтаксические характеристики для текущей основы
				/** 
					\param p_list_OfSyntaxInfo - указатель на список, содержащий синтаксические характеристики для текущей основы
				*/
				void Add(/*in*/std::list<SS::CoSeDi::DataTypes::TSyntaxInfo>* p_list_OfSyntaxInfo);

				/// удаляет всю синтаксическую информацию для текущей основы
				void Delete();

				/// удаляет список, содержащий синтаксические характеристики для текущей основы
				/** 
					\param p_list_OfSyntaxInfo - указатель на список, содержащий синтаксические характеристики для текущей основы
				*/
				void Delete(/*in*/std::list<SS::CoSeDi::DataTypes::TSyntaxInfo>* p_list_OfSyntaxInfo);
			private:	
				/// Инициализация соединений с таблицами
				bool	InitConnection(void);
			private:
				/*! \var SS::CoSeDi::DataTypes::SSyntaxAccess m_SSyntaxAccess;
				*  \brief     - структура, содержащая поля для доступа к таблицам
				*/
				SS::CoSeDi::DataTypes::SSyntaxAccess m_SSyntaxAccess;

				/*! \var SS::CoSeDi::SQL::CCashCoSeDi								* m_pCashCoSeDi
				*  \brief	- указатель на кэш     
				*/
				SS::CoSeDi::DBMS::CCashCoSeDi	* m_pCashCoSeDi;		
				
				/*! \var void       * TableRecord[2];
				*  \brief     массив указателей на данные, являющиеся полями в DBMS таблице
				*/
				void* TableRecord[3];

				/*! \enum EFindMode
				*  \brief     определят поле, по которому будет осуществляться поиск
				*/
				enum EFindMode
				{
					efmByIDSource
				};

			};
		};
	};
};
