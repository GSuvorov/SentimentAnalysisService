#pragma once

#include "..\ASSInterface\ICoSeDi.h"

#include ".\BaseCashCoSeDi.h"

#include ".\connect.h"
#include ".\dboCoSeDi.h"

using namespace SS::CoSeDi::DataTypes;

namespace SS
{
	namespace CoSeDi
	{
		namespace SQL
		{
			/// SQL-кэш TKC			
			class CCashCoSeDi : 
				public SS::Interface::Core::CoSeDi::ICoSeDiLoad, 
				public SS::CoSeDi::CBaseCashCoSeDi,
				public SS::CoSeDi::SQL::CConnect
			{
			public:
				/// конструктор 
				/** 
					\param szBaseName - имя базы					
				*/
				CCashCoSeDi(const wchar_t * szBaseName) :  SS::CoSeDi::SQL::CConnect(szBaseName), m_bIsLoad(false)
				{
				}
				/// деструктор
				virtual ~CCashCoSeDi(void)
				{
				}			
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
			public:	//	from ILoad
				/// очищает словарь
				bool Clear();
				///	начальная загрузка	
				bool Load();
				///	перезагрузка	
				bool ReLoad();
				///	преобразование к бинарному виду	
				bool ToBinary();
			public:
				/// добавляет основу в кэш
				/** 
					\param uiIDSource - индекс основы
					\param ewr - область в которой появилась основы
					\return bool - результат выполнения операции
				*/
				bool AddIDSource(unsigned int uiIDSource, SS::CoSeDi::DataTypes::EWorkRegion ewr);
			private:										
				/// Сохраняет таблицу SemanticClassesTree в бинарный вид
				/** 
					\return bool - результат выполнения операции
				*/
				bool ToBinarySemanticClassesTree();

				/// Сохраняет таблицу SemanticClassesWords в бинарный вид
				/** 
					\return bool - результат выполнения операции
				*/
				bool ToBinarySemanticClassesWords();

				/// Сохраняет таблицу SynonimsSemantics в бинарный вид
				/** 
					\return bool - результат выполнения операции
				*/
				bool ToBinarySynonimsSemantics();

				/// Сохраняет таблицу Semantics в бинарный вид
				/** 
					\return bool - результат выполнения операции
				*/
				bool ToBinarySemantics();

				/// Сохраняет таблицу SemanticsExtra в бинарный вид
				/** 
					\return bool - результат выполнения операции
				*/
				bool ToBinarySemanticsExtra();

				/// Сохраняет кэш в бинарный вид
				/** 
					\return bool - результат выполнения операции
				*/
				bool ToBinaryExistence();


				/// Сохраняет таблицу Combinatory в НОВЫЙ бинарный вид
				/** 
					\return bool - результат выполнения операции
				*/
				bool ToBinaryCombinatory();

				/// Сохраняет таблицу CombinatoryExtra в НОВЫЙ бинарный вид
				/** 
					\return bool - результат выполнения операции
				*/
				bool ToBinaryCombinatoryExtra();

				/// Сохраняет таблицу CombiSynonyms в бинарный вид
				/** 
					\return bool - результат выполнения операции
				*/
				bool ToBinaryCombiSynonyms();

				/// Сохраняет синтаксис в бинарный вид
				/** 
					\return bool - результат выполнения операции
				*/
				bool ToBinarySyntax();
				/// Сохраняет таблицу SemanticIDSourceTree в бинарный вид
				/** 
					\return bool - результат выполнения операции
				*/
				bool ToBinarySemanticIDSourceTree();
			private:						
				/*! \var bool m_bIsLoad
				*  \brief     указывает загружел ли кэш
				*/
				bool m_bIsLoad;
			};
		};
///////////////////////////////////////////////////////////////////////////////////////////////////
		namespace DBMS
		{
			/// DBMS-кэш TKC		
			class CCashCoSeDi : public SS::Interface::Core::CoSeDi::ICoSeDiLoad, 
				public SS::CoSeDi::CBaseCashCoSeDi,
				public SS::CoSeDi::DBMS::CConnect
			{
			public:
				/// конструктор 
				/** 
					\param szBaseName - имя базы					
				*/
				CCashCoSeDi(const wchar_t * szBaseName) : SS::CoSeDi::DBMS::CConnect(szBaseName), m_bIsLoad(false)
				{
					m_SExistence.IDExistence = 0;
					m_SExistence.IDSource = 0;
				}				
				/// деструктор
				virtual ~CCashCoSeDi(void)
				{		
				}							

			public: // function
				/// устанавливает указатель на актиный менеджер соединений
				/** 
					\param pLoadManager - указатель на актиный менеджер соединений
				*/				
				void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
				{
					SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);
					InitConnection();
				}
			public:	//	from ILoad
				/// очищает словарь
				bool Clear();
				///	начальная загрузка	
				bool Load();
				///	перезагрузка	
				bool ReLoad();
				///	преобразование к бинарному виду	
				bool ToBinary();
			private:						
				/// Инициализация соединений с таблицами
				bool InitConnection();
			
			private:						
				/*! \var bool m_bIsLoad
				*  \brief     указывает загружел ли кэш
				*/
				bool m_bIsLoad;		

				/*! \var SS::CoSeDi::DataTypes::SExistence m_SExistence
				*  \brief     структура соответвстия индекса основы и ее существования
				*/
				SS::CoSeDi::DataTypes::SExistence m_SExistence;
				/*! \var void* TableRecord[2];
				*  \brief     массив указателей на данные, являющиеся полями в DBMS таблице
				*/
				//--UNUSED-- void* TableRecord[2];	
				
			};
		};
	};
};

