#pragma once

#include "..\ASSInterface\ICoSeDi.h"
#include ".\CashCoSeDiEx.h"
#include ".\CombinatoryAnalysis.h"


namespace SS
{
	namespace CoSeDi
	{
		namespace SQL
		{

			class CCombinatoryAnalysisEx : 
				public SS::Interface::Core::CoSeDi::ICombinatoryAnalysis,
				public SS::Core::CommonServices::CBaseCoreClass

			{
			public:
				CCombinatoryAnalysisEx(SS::CoSeDi::SQL::CCashCoSeDiEx*	pCashCoSeDi);
				~CCombinatoryAnalysisEx(void);
			public:

				void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
				{
					SS_TRY;
					SS::Core::CommonServices::CBaseCoreClass::SetLoadManager( pLoadManager);

					m_oCombinatoryAnalysisRus.SetLoadManager( pLoadManager);
					m_oCombinatoryAnalysisEng.SetLoadManager( pLoadManager);


					SS_CATCH(L"");
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
				SS::CoSeDi::SQL::CCombinatoryAnalysis m_oCombinatoryAnalysisRus;
				SS::CoSeDi::SQL::CCombinatoryAnalysis m_oCombinatoryAnalysisEng;
			};

		};

		namespace DBMS
		{

			class CCombinatoryAnalysisEx : 
				public SS::Interface::Core::CoSeDi::ICombinatoryAnalysis,
				public SS::Core::CommonServices::CBaseCoreClass
			{
			public:
				CCombinatoryAnalysisEx(SS::CoSeDi::DBMS::CCashCoSeDiEx*	pCashCoSeDi);
				~CCombinatoryAnalysisEx(void);
			public:

				void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
				{
					SS_TRY;
					SS::Core::CommonServices::CBaseCoreClass::SetLoadManager( pLoadManager);

					m_oCombinatoryAnalysisRus.SetLoadManager( pLoadManager);
					m_oCombinatoryAnalysisEng.SetLoadManager( pLoadManager);


					SS_CATCH(L"");
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
				SS::CoSeDi::DBMS::CCombinatoryAnalysis m_oCombinatoryAnalysisRus;
				SS::CoSeDi::DBMS::CCombinatoryAnalysis m_oCombinatoryAnalysisEng;
			};

		};
	};
};

