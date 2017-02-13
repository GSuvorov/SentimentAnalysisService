#pragma once

#include "..\ASSInterface\ICoSeDi.h"
#include ".\CashCoSeDiEx.h"
#include ".\SemanticAnalysis.h"


namespace SS
{
	namespace CoSeDi
	{
		namespace SQL
		{
			class CSemanticAnalysisEx : 
				public SS::Interface::Core::CoSeDi::ISemanticAnalysis,
				public SS::Core::CommonServices::CBaseCoreClass
			{
			public:
				CSemanticAnalysisEx(SS::CoSeDi::SQL::CCashCoSeDiEx*	pCashCoSeDi, std::wstring wTableName);
				~CSemanticAnalysisEx(void);

			public:

				void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
				{
					SS_TRY;
					SS::Core::CommonServices::CBaseCoreClass::SetLoadManager( pLoadManager);

					if(m_pSemanticAnalysisRus)
						m_pSemanticAnalysisRus->SetLoadManager( pLoadManager);
					if(m_pSemanticAnalysisEng)
						m_pSemanticAnalysisEng->SetLoadManager( pLoadManager);


					if(m_pSemanticAnalysisSynonimsRus)
						m_pSemanticAnalysisSynonimsRus->SetLoadManager( pLoadManager);
					if(m_pSemanticAnalysisSynonimsEng)
						m_pSemanticAnalysisSynonimsEng->SetLoadManager( pLoadManager);

					SS_CATCH(L"");
				}
			public:
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
				SS::CoSeDi::SQL::CSemanticAnalysis* m_pSemanticAnalysisRus;
				SS::CoSeDi::SQL::CSemanticAnalysis* m_pSemanticAnalysisEng;

				SS::CoSeDi::SQL::CSemanticAnalysisSynonims* m_pSemanticAnalysisSynonimsRus;
				SS::CoSeDi::SQL::CSemanticAnalysisSynonims* m_pSemanticAnalysisSynonimsEng;
			};

		};

		namespace DBMS
		{
			class CSemanticAnalysisEx : 
				public SS::Interface::Core::CoSeDi::ISemanticAnalysis,
				public SS::Core::CommonServices::CBaseCoreClass
			{
			public:
				CSemanticAnalysisEx(SS::CoSeDi::DBMS::CCashCoSeDiEx*	pCashCoSeDi, std::wstring wTableName);
				~CSemanticAnalysisEx(void);
			public:

				void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
				{
					SS_TRY
					{
						SS::Core::CommonServices::CBaseCoreClass::SetLoadManager( pLoadManager);

						m_oSemanticAnalysisRus.SetLoadManager( SS::Core::CommonServices::CBaseCoreClass::GetLoadManager() );
						m_oSemanticAnalysisEng.SetLoadManager( SS::Core::CommonServices::CBaseCoreClass::GetLoadManager() );
					}
					SS_CATCH(L"")
				}

			public:
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
				bool	DeleteWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex);
				
				/// удаляет текущий семантический класс 
				bool	DeleteClass(SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber oDictionariesNumber);

				/// Создает класс и добавляет к нему слово
				/** 
					\param oTFormInfo - все координаты слова, которое необходимо добавить
					\return возращает номер созданного класса
				*/
				int	CreateClassAndAddWord(/*in*/ SS::Dictionary::Types::TDictionaryIndex & oDictionaryIndex);

			private:
				SS::CoSeDi::DBMS::CSemanticAnalysis m_oSemanticAnalysisRus;
				SS::CoSeDi::DBMS::CSemanticAnalysis m_oSemanticAnalysisEng;
			};

		};
	};
};