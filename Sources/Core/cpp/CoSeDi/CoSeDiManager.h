#pragma once

#include "..\ASSInterface\ICoSeDi.h"

#include ".\CashCoSeDiEx.h"
#include ".\Combinatory.h"
#include ".\SemanticanAlysisEx.h"
#include ".\SemanticClassesTreeEx.h"
#include ".\SemanticAnalysis.h"
#include ".\SyntaxanAlysisex.h"
#include ".\SemanticClassesTree.h"
#include ".\SemanticClassesWords.h"

#include ".\SemanticIDSourceTreeEx.h"


namespace SS
{
	namespace CoSeDi
	{
		/*! \enum EDataBaseType
		*  \brief     список возможных типов баз данных 
		*/
		enum EDataBaseType
		{
			edbtDBMS,
			edbtSQL
		} ;

		/// реализует методы интерфейса SS::Interface::Core::CoSeDi::ICoSeDiManager
		class CCoSeDiManager : public SS::Interface::Core::CoSeDi::ICoSeDiManager
		{
		public:			
			/// конструктор
			/** 
				\param szBaseName - имя базы, с которой будем работать
			*/
			CCoSeDiManager();
			/// деструктор
			~CCoSeDiManager(void);
		public: // from SS::Interface::IBase 
			///освобождение
			ULONG Release();
			///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
			HRESULT QueryInterface(REFIID pIID, void** pBase); 
		public: // from SS::SMInterface::IWorkWithManagerConnections
			/// Устанавливает указатель на текущий менеджер соединений 
			/** 
				\param pLoadManager - указатель на текущий менеджер соединений
			*/
			void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager) 			
			{
				SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);
			}
		public: 
				/// возвращает интерфес для загрузки словаря
				SS::Interface::Core::CoSeDi::ICoSeDiLoad                  * GetLoader();
				///	возвращает интерфес для семантического анализа	
				SS::Interface::Core::CoSeDi::ISemanticAnalysis            * GetSemanticAnalysis();
				///	возвращает интерфес для синтаксического анализа
				SS::Interface::Core::CoSeDi::ISyntaxAnalysis              * GetSyntaxAnalysis();
				///	возвращает интерфес для работы с деревом семантических классов	
				SS::Interface::Core::CoSeDi::ISemanticClassesTree         * GetSemanticClassesTree();
				///	возвращает интерфес для работы с основами, связанными с деревом семантических классов	
				SS::Interface::Core::CoSeDi::ISemanticClassesWords        * GetSemanticClassesWords();
				///	возвращает интерфес для работы с семантическими деревьями конкретных основ	
				SS::Interface::Core::CoSeDi::ISemanticIDSourceTree        * GetSemanticIDSourceTree();
				///	возвращает интерфес для семантического анализа при работе с комбинаторными синонимами
				SS::Interface::Core::CoSeDi::ISemanticAnalysis            * GetCombiSynonimsAnalysis();
				///	возвращает интерфес для семантического анализа	
				SS::Interface::Core::CoSeDi::ISemanticAnalysis            * GetRestaurantSemanticAnalysis();
				///	возвращает интерфес для семантического анализа при работе с синонимами
				SS::Interface::Core::CoSeDi::ISemanticAnalysis            * GetSynonimsSemanticAnalysis();
				///	возвращает интерфес для комбинаторного анализа	
				SS::Interface::Core::CoSeDi::ICombinatory                 * GetCombinatory();
				///	возвращает интерфес для комбинаторного анализа (доп. комбинаторная таблица)	
				SS::Interface::Core::CoSeDi::ICombinatory                 * GetCombinatoryExtra();
		
				
		private:
			/*! \enum m_eDataBaseType - тип базы */
			EDataBaseType m_eDataBaseType;
			/*! \var m_pCashCoSeDiDBMS
			*  \brief     кэш при работе с DBMS частью ТКС
			*/
			SS::CoSeDi::DBMS::CCashCoSeDiEx         * m_pCashCoSeDiDBMS;

			/*! \var m_pCashCoSeDiSQL
			*  \brief  кэш при работе с SQL частью ТКС
			*/
			SS::CoSeDi::SQL::CCashCoSeDiEx          * m_pCashCoSeDiSQL;

			/*! \var m_pSyntaxAnalysisDBMS
			*  \brief - указатель на объект класса SS::CoSeDi::DBMS::CSyntaxAnalysis
			*/
			SS::CoSeDi::DBMS::CSyntaxAnalysisEx     * m_pSyntaxAnalysisDBMS;
			
			/*! \var m_pSyntaxAnalysisSQL
			*  \brief - указатель на объект класса SS::CoSeDi::SQL::CSyntaxAnalysis
			*/
			SS::CoSeDi::SQL::CSyntaxAnalysisEx      * m_pSyntaxAnalysisSQL;
			
			/*! \var m_pCombinatory
			*  \brief - указатель на объект класса SS::CoSeDi::CCombinatory
			*/
			SS::CoSeDi::DBMS::CCombinatory          * m_pCombinatory;

			/*! \var m_pCombinatoryExtra
			*  \brief - указатель на объект класса SS::CoSeDi::CCombinatory (доп. комбинаторная таблица)
			*/
			SS::CoSeDi::DBMS::CCombinatory          * m_pCombinatoryExtra;
			
			/*! \var m_pSemanticAnalysisDBMS
			*  \brief - указатель на объект класса SS::CoSeDi::DBMS::CSemanticAnalysisEx
			*/
			SS::CoSeDi::DBMS::CSemanticAnalysisEx   * m_pSemanticAnalysisDBMS;

			/*! \var m_pSemanticAnalysisExtraDBMS
			*  \brief - указатель на объект класса SS::CoSeDi::DBMS::CSemanticAnalysisEx (доп. семантическая таблица)
			*/
			SS::CoSeDi::DBMS::CSemanticAnalysisEx   * m_pSemanticAnalysisExtraDBMS;
			
			/*! \var m_pSemanticAnalysisSQL
			*  \brief - указатель на объект класса SS::CoSeDi::SQL::CSemanticAnalysisEx
			*/
			SS::CoSeDi::SQL::CSemanticAnalysisEx    * m_pSemanticAnalysisSQL;

			/*! \var m_pSemanticAnalysisExtraSQL
			*  \brief - указатель на объект класса SS::CoSeDi::SQL::CSemanticAnalysisEx (доп. семантическая таблица)
			*/
			SS::CoSeDi::SQL::CSemanticAnalysisEx    * m_pSemanticAnalysisExtraSQL;

			/*! \var m_pSemanticAnalysis
			*  \brief - указатель на объект класса SS::CoSeDi::CSemanticAnalysis
			*/
//			SS::CoSeDi::CSemanticAnalysis * m_pSemanticAnalysis;
			
			/*! \var m_pSemanticClassesTree
			*  \brief - указатель на объект класса SS::CoSeDi::CSemanticClassesTree
			*/
			SS::CoSeDi::CSemanticClassesTree        * m_pSemanticClassesTree;

			/*! \var m_pSemanticClassesWord
			*  \brief - указатель на объект класса SS::CoSeDi::CSemanticClassesWords
			*/
			SS::CoSeDi::CSemanticClassesWords       * m_pSemanticClassesWords;

//			! \var m_pSemanticClassesTreeDBMS
//			  \brief - указатель на объект класса SS::CoSeDi::DBMS::CSemanticClassesTree
//			/
//			SS::CoSeDi::DBMS::CSemanticClassesTreeEx * m_pSemanticClassesTreeDBMS;

//			! \var m_pSemanticClassesTreeSQL
//			  \brief - указатель на объект класса SS::CoSeDi::SQL::CSemanticClassesTree
//			/
//			SS::CoSeDi::SQL::CSemanticClassesTreeEx  * m_pSemanticClassesTreeSQL;

			/*! \var m_pSemanticIDSourceTreeDBMS
			*  \brief - указатель на объект класса SS::CoSeDi::DBMS::CSemanticIDSourceTree
			*/
			SS::CoSeDi::DBMS::CSemanticIDSourceTreeEx* m_pSemanticIDSourceTreeDBMS;
			
			/*! \var m_pSemanticIDSourceTreeSQL
			*  \brief - указатель на объект класса SS::CoSeDi::SQL::CSemanticIDSourceTree
			*/
			SS::CoSeDi::SQL::CSemanticIDSourceTreeEx * m_pSemanticIDSourceTreeSQL;
			
			/*! \var m_pSemanticAnalysisDBMS
			*  \brief - указатель на объект класса SS::CoSeDi::DBMS::CSemanticAnalysisEx
			*/
			SS::CoSeDi::DBMS::CSemanticAnalysisEx    * m_pSemanticCombiSynonimsDBMS;
			
			/*! \var m_pSemanticAnalysisSQL
			*  \brief - указатель на объект класса SS::CoSeDi::SQL::CSemanticAnalysisEx
			*/
			SS::CoSeDi::SQL::CSemanticAnalysisEx     * m_pSemanticCombiSynonimsSQL;

			/*! \var m_pSemanticAnalysisDBMS
			*  \brief - указатель на объект класса SS::CoSeDi::DBMS::CSemanticAnalysisEx
			*/
			SS::CoSeDi::DBMS::CSemanticAnalysisEx    * m_pSemanticAnalysisSynonimsDBMS;
			
			/*! \var m_pSemanticAnalysisSQL
			*  \brief - указатель на объект класса SS::CoSeDi::SQL::CSemanticAnalysisEx
			*/
			SS::CoSeDi::SQL::CSemanticAnalysisEx     * m_pSemanticAnalysisSynonimsSQL;
			

			///*! \var m_szBaseName
			//*  \brief - строка, содержащая имя базы
			//*/
			//char m_szBaseName[100];

		};
	};

};
