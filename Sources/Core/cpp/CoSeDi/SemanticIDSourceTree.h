#pragma once


#include "..\ASSInterface\ICoSeDi.h"
#include ".\connect.h"
#include ".\dboSemantic.h"

namespace SS
{
	namespace CoSeDi
	{
		namespace SQL
		{

			/// класс реализующий работу с иерархией слов
			class CSemanticIDSourceTree :
				public SS::CoSeDi::SQL::CConnect,
				public SS::Interface::Core::CoSeDi::ISemanticIDSourceTree
			{
			public:
				/// Конструктор
				/** 
					\param szBaseName - имя базы
				*/
				CSemanticIDSourceTree(const wchar_t* szBaseName);
				/// деструктор
				virtual ~CSemanticIDSourceTree(void);
			public:
				/// возвращает всех родителей текущего ребенка
				/** 
					\param plistOfFullIDSource - указатель на список, содержащий индексы родителей
				*/
				void GetParents(std::list<SS::CoSeDi::DataTypes::SFullIDSource>* plistOfFullIDSource);

				/// добавляет текущую пару (родитель, ребенок)
				void Add();

				/// удаляет пару текущую (родитель, ребенок)
				void Delete();
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
			public:
				/// Возвращает родителя для текущего индекса основы
				/** 
					\param oFullIDSource - 
					\param oPrepared - аксессор для поиска информации по индексу основы
					\param plistOfFullIDSource
				*/
				void GetParent(	
					SS::CoSeDi::DataTypes::SFullIDSource& oFullIDSource,
					SS::CoSeDi::SQL::SQL_ACCESS::Semantic::CPreparedCSSemanticIDSourceTree& oPrepared,
					std::list<SS::CoSeDi::DataTypes::SFullIDSource>* plistOfFullIDSource);

			};
		};
			
		namespace DBMS
		{
			/// класс реализующий работу с иерархией слов
			class CSemanticIDSourceTree :
				public SS::CoSeDi::DBMS::CConnect,
				public SS::Interface::Core::CoSeDi::ISemanticIDSourceTree
			{
			public:
				/// Конструктор
				/** 
					\param szBaseName - имя базы
				*/
				CSemanticIDSourceTree(const wchar_t* szBaseName);
				/// деструктор
				virtual ~CSemanticIDSourceTree(void);
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
			private:
				bool InitConnection();
				
				void GetParents(std::list<SS::CoSeDi::DataTypes::SFullIDSource>* plistOfFullIDSource, SS::CoSeDi::DataTypes::SFullIDSource& oCurChild);
			public:
				/// возвращает всех родителей текущего ребенка
				/** 
					\param plistOfFullIDSource - указатель на список, содержащий индексы родителей
				*/
				void GetParents(std::list<SS::CoSeDi::DataTypes::SFullIDSource>* plistOfFullIDSource);				

				/// добавляет текущую пару (родитель, ребенок)
				void Add();

				/// удаляет пару текущую (родитель, ребенок)
				void Delete();

			protected:
				/*! \var SS::CoSeDi::DataTypes::SSemanticIDSourceTree m_SemanticIDSourceTree
				*  \brief     структура для доступа к таблице семантического дерева  основ
				*/
				SS::CoSeDi::DataTypes::SSemanticIDSourceTree m_SemanticIDSourceTree;

				/*! \var void       * TableRecord[4];
				*  \brief     массив указателей на данные, являющиеся полями в DBMS таблице
				*/
				void       * TableRecord[4];

				/*! \enum EFindMode
				*  \brief     определят поле, по которому будет осуществляться поиск
				*/
				enum EFindMode
				{
					efmByIDParent,
					efmByIDChild
				};
			};
		};
	};
};