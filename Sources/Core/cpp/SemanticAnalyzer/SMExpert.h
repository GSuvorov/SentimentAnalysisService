#pragma once
#include "..\ASSInterface\IAMConverter.h"
#include "..\assinterface\iblackbox.h"

#include "StringService.h"
#include ".\lexemtype.h"
#include ".\types.h"

#include ".\OfficialVirtualSemanticDictionary.h"
#include "..\assinterface\IDictionaryManager.h"
#include "..\assinterface\TDictionaryNames.h"


using namespace SS::Interface::Core::BlackBox;

namespace SS
{
	namespace Semantic
	{
		class CSMManager;

		typedef list<IUnit*> TBoxUnits;
		/// состояние обхода ВВ
		struct SCurrentPosition
		{
			IText * _pIText;
			IParagraph * _pParagraph;
			ISentence * _pSentence;
			IUnit * _pUnit;
		};
	
		/// задаёт интерфейс и реализует общую функциональность
		class CSMExpert : virtual public SS::Interface::Core::Common::CAnalyseParams
		{
		public:
			CSMExpert(void);
			virtual ~CSMExpert(void);
			
			/// анализ текста BlackBox
			/** 
				\return bool - истина - если анализ прошёл успешно
			 
			задаёт единый интерфейс к поведению экспертов
			*/
			virtual bool Analize()=0;
			
			///инициализация эксперта текстом pIText
			virtual bool Init(IText * pIText);

			///возвращает описание эксперта
			wstring& GetDescription()
			{ 
				return m_sDescription; 
			};

			///устанавливает указатель на менеджер разбора
			void SetSMManager(CSMManager * pManager);

			/// истина, если должен эксперт выполняться
			bool IsExecuteExpert(SS::Core::Types::EAnalyseLanguage eAnalyseLanguage)
			{
				if(eAnalyseLanguage==SS::Core::Types::ealUndefined) return true;
				if(m_AnalyseLanguage==SS::Core::Types::ealUndefined) return true;
				return (eAnalyseLanguage == m_AnalyseLanguage) ; 
			};


			void SetCurrentSentence(ISentence * pSentence){ m_pCurrentSentence=pSentence; };

		protected:
			/// строковое описание эксперта
			wstring m_sDescription;
			///морфологический конвертор 
			SS::Interface::Core::AMConverter::IAMConverterMorpho * m_pIAMCMorpho;
			///семантический конвертор
			SS::Interface::Core::AMConverter::IAMConverterSemantic * m_pIAMCSemantic;
			/// словарь категорий
			SS::Semantic::VirtualDictionaries::COfficialVirtualSyntaxDictionary * m_pOfficialVirtualSyntax;
			/// фабрика блэк бокса
			SS::Interface::Core::BlackBox::IBlackBoxTextStorage * m_pIBlackBoxFactory;
			/// мэнеджер по связям со словарями 
			CSMManager * m_pSMManager;
			///текст к анализу
			IText * m_pIText;
			/// текущий параграф
			IParagraph * m_pCurrentParagraph;
			/// текущее предложение
			ISentence * m_pCurrentSentence;
			/// текущий юнит
			IUnit * m_pCurrentUnit;
			/// текущий индекс текущего юнита
			IIndex * m_pCurrentIndex;
			/// память
			SCurrentPosition m_oMem;
			/// память1
			SCurrentPosition m_oMem1;
			/// память2
			SCurrentPosition m_oMem2;
			/// обработка строк
			SS::Semantic::CStringService m_StringService;
			/// определение типа лексем
			CLexemType m_LexemType;
			/// язык анализа
			SS::Core::Types::EAnalyseLanguage m_AnalyseLanguage;
			/// коллекция морфологии для функции CollectMorpho
			SS::Core::Features::CMorphoFeature m_MorphoFeatureCollect;
			/// истина, если эксперт инициализирован
			bool m_bInit;

			/// возвращает истину, если режим анализа
			bool IsQuestionMode(){ return false;};
			/// истина, если режим запроса по ключевым словам
			bool IsStrictMode(){ return false; };
			/// запомнить текущую позицию
			void Mem();
			/// перейти к запомненой позыции
			void GotoMem();
			void Mem1();
			void GotoMem1();
			void Mem2();
			void GotoMem2();
			/// переход к юниту в рамках 1 предложения
			void GotoUnit(IUnit * pUnit);
			/// убирает юнит и возвращает истину, если есть юниты спарава, ложь в противном случае ( текущим становится предыдущий)
			bool HideCurrentUnit();


			/// устанавливает эксперта на первый параграф
			bool GetFirstParagraph();
			/// следующий параграф
			bool GetNextParagraph();
			/// первое предложение текущего параграфа
			bool GetFirstSentence();
			/// очередное предложение
			bool GetNextSentence();
			/// первый юнит текущего предложения
			IUnit * GetFirstUnit();
			/// последний юнит текущего предложения
			IUnit * GetLastUnit();
			/// очередной юнит
			IUnit * GetNextUnit();
			/// очередной юнит с учетом детей
			IUnit * GetNextUnitWithChild();
			/// предыдущий юнит
			IUnit * GetPrevUnit();
			/// первый индекс юнита
			IIndex * GetFirstIndex(IUnit * pUnit=NULL);
			/// очередной индекс
			IIndex * GetNextIndex(IUnit * pUnit=NULL);

		private:
			/// добавляет характеристику слову
			/** 
				\param uiFeature - характеристика
				\param pUnit=NULL  - слово
			*/
			void AddFeature(UINT uiFeature, IUnit * pUnit=NULL);
		protected:
			void AddFeature(CommonFeature eFeature,IUnit * pUnit=NULL);
			void AddFeature(ERusFeature eFeature,IUnit * pUnit=NULL);
			void AddFeature(EEngFeature eFeature,IUnit * pUnit=NULL);

			/// истина, если слово содержит данную характеристику
			/** 
			\param uiFeature - характеристика
			\param pUnit=NULL  - слово
			*/
			//private:

			bool IsContainFeature(UINT uiFeature, IUnit * pUnit=NULL);
			bool IsContainFeature(CommonFeature eFeature,IUnit * pUnit=NULL);
			bool IsContainFeature(ERusFeature eFeature,IUnit * pUnit=NULL);
			bool IsContainFeature(EEngFeature eFeature,IUnit * pUnit=NULL);

			void RemoveFeature(CommonFeature eFeature,IUnit * pUnit=NULL);

			void SetSpecialType(TSpecialUnitType eSpecialType, IUnit * pUnit=NULL);
			bool SpecialTypeEquals(TSpecialUnitType eSpecialType, IUnit * pUnit=NULL);
			
			//void SetSyntaxType(TSyntaxUnitType eSyntaxType, IUnit * pUnit=NULL);
			//bool SyntaxTypeEquals(TSyntaxUnitType eSyntaxType, IUnit * pUnit=NULL);
			
			/// устанавливает флаг синтаксиса - истина, если юнит входит в синтаксическую категорию
			/** 
				\param pUnit=NULL - слово 
				\param bFlag=true - флаг
			*/
			void SetSyntax(IUnit * pUnit=NULL, bool bFlag=true);
			/// возвращает значение флага синтаксиса
			bool IsSyntax(IUnit * pUnit=NULL);
			/// истина, если текущее слово есть предлог с синтаксисом
			bool IsPrepositionSyntax(IUnit * pUnit=NULL);
			/// собирает в коллекцию морфологические характеристики
			/** 
				\param oMorphoFeature - коллекция
				\param pUnit=NULL - юнит
			*/
			void CollectMorphoFeature(SS::Core::Features::CMorphoFeature & oMorphoFeature, IUnit * pUnit=NULL);
			/// собирает семантические индексы юнита в коллекцию
			/** 
			\param pUnit - юнит
			\param oSemanticFeature - коллекция
			\return 			bool - истина, если семантика есть
			*/
			bool CollectSemanticFeature(SS::Core::Features::CSemanticFeature & oSemanticFeature, IUnit * pUnit=NULL);
			/// оставляет юниту только идексы указанного семантического типа
			/** 
			\param oSemType - семантический тип который будет оставлен
			\param l_Sem - список оставленных семантических индексов
			\return 			bool - истина, если есть оставленые индексы
			*/
			bool KeepSpecifedSemanticTypes(const SS::Core::Features::Types::TSemanticType & oSemType, list<SS::Dictionary::Types::TDictionaryIndex> & l_Sem);

			/// возвращает список индексов основы слова
			/** 
				\param pWord - слово 
				\param l_Sources - список индексов 
				\return 			bool - истина - если список не пуст 
			*/
			bool GetWordSourceIndices(const TCHAR * pWord,list<UINT> & l_Sources);
			/// возвращает морфологию слова
			/** 
				\param pWord - слово 
				\param l_Info - морфология
				\return 			bool - истина , если морфология у слова есть
			*/
			bool GetWordMorphoInfo(const TCHAR * pWord,list<SS::Dictionary::Types::TWordInfo> & l_Info);
			/// группирует юниты в контейнере 
			/** 
				\param oUnits - группа юнитов
				\return 			IUnit * - юнит родитель группы
			*/
			IUnit * JoinUpUnits(TBoxUnits & oUnits);


			/// выставляет синтаксическую категорию юниту
			/** 
				\param pUnit - юнит
				\param oCategory - категория
			*/
			void SetSyntaxCategory(IUnit * pUnit, const Types::SyntaxCategories & oCategory);
			/// добавление синтаксической категории юниту
			/** 
			\param pUnit - юнит
			\param oCategory - категория
			*/
			void AddSyntaxCategory(IUnit * pUnit, const Types::SyntaxCategories & oCategory);
			/// удаляет синтаксической категорию у юнита
			/** 
			\param pUnit - юнит
			\param oCategory - категория
			*/
			void DeleteSyntaxCategory(IUnit * pUnit, const Types::SyntaxCategories & oCategory);
			/// проверяет наличие синтаксической категории у юнита
			/** 
			\param pUnit - юнит
			\param oCategory - категория
			*/
			bool IsContainSyntaxCategory(IUnit * pUnit, const Types::SyntaxCategories & oCategory);
			/// добавляет индекс синтаксической категории юниту
			/** 
			\param pUnit
			\param l_oCategory
			*/
			void AddSyntaxCategoryIndices(list<Types::SyntaxCategories> & l_oCategory, IUnit * pUnit=NULL);
			/// добавляет индекс синтаксической категории юниту
			/** 
			\param pUnit
			\param l_oCategory
			*/
			void AddSyntaxCategoryIndices(list<UINT> & l_oCategory, IUnit * pUnit=NULL);
			void RemoveSyntaxCategoryIndices(list<UINT> & l_oCategory, IUnit * pUnit=NULL);
			/// добавляет индекс синтаксической категории юниту
			/** 
			\param pUnit
			\param oCategory
			*/
			void AddSyntaxCategoryIndex(const Types::SyntaxCategories & oCategory, IUnit * pUnit=NULL);
			/// удаляет индекс категории у юнита
			/** 
				\param oCategory - категория
				\param pUnit=NULL - юнит
			*/
			void RemoveSyntaxCategoryIndex(const Types::SyntaxCategories & oCategory, IUnit * pUnit=NULL);
			bool IsContainSyntaxCategoryIndex(const Types::SyntaxCategories & oCategory, IUnit * pUnit=NULL);
			/// ищет индекс с данным индексом словаря
			/** 
				\param uiIndex - индекс словаря ( основы ) 
				\param pUnit - слово 
				\return 			IIndex * - индекс 
			*/
			IIndex * FindIndex(UINT uiIndex, IUnit * pUnit=NULL ); 
			/// добавляет индекс юниту
			/** 
				\param uiIndex - индекс
				\param pUnit - юнит
			*/
			void AddIndex(SS::Dictionary::Types::TDictionaryIndex uiIndex, IUnit * pUnit=NULL );
			/// удаляет семантический индексы у юнита и переносит их в список 
			/** 
				\param pUnit - юнит
				\param l_Sem - список в который будут помещены удаляемые индексы
			*/
			void RemoveSemanticIndicesFromUnit(list<SS::Dictionary::Types::TDictionaryIndex> & l_Sem, IUnit * pUnit=NULL);

			/// устанавливает семантические индексы юниту
			/** 
				\param pUnit - юнит
				\param l_Sem - семантические индексы
			*/
			void SetSemanticIndices(list<SS::Dictionary::Types::TDictionaryIndex> & l_Sem, IUnit * pUnit=NULL);

			/// устанавливает семантические индексы юниту
			/** 
				\param pUnit - юнит
				\param l_Sem - семантические индексы
			*/
			void SetSemanticIndicesWithOutGeneric(list<SS::Dictionary::Types::TDictionaryIndex> & l_Sem, IUnit * pUnit=NULL);


			/// добавляет семантические индексы юниту
			/** 
				\param lSem - семантика
				\param pUnit=NULL - юнит
			*/
			void AddSemanticIndex(SS::Dictionary::Types::TDictionaryIndex  lSem,  IUnit * pUnit=NULL);
			/// проверка наличия указанного семантического индекса
			/** 
				\param lSem -семантика
				\param pUnit=NULL - юнит
				\return 			bool - истина, если есть указанная семантика
			*/
			bool IsContainSemanticIndex(long  lSem,  IUnit * pUnit=NULL);
			/// добавляет синтаксический индекс юниту
			/** 
				\param pUnit - юнит 
				\param uiIndex - индекс
			*/
			void AddSyntaxIndex(UINT uiIndex,IUnit * pUnit=NULL);
			/// истина если индекс семантический
			bool IsSemanticIndex(IIndex* pIndex);
			/// истина если индекс морфологический
			bool IsMorphoIndex(IIndex* pIndex=NULL);
};

	}
}

