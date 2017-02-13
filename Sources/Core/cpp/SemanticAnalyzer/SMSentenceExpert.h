#pragma once
#include "smexpert.h"
namespace SS
{
	namespace Semantic
	{
		/// базовый класс для экспертов работающих с предложениями
		class CSMSentenceExpert : public CSMExpert
		{
		public:
			CSMSentenceExpert(void);
			virtual ~CSMSentenceExpert(void);
			/// анализ текста BlackBox
			/** 
			\return bool - истина - если анализ прошёл успешно

			задаёт единый интерфейс к поведению экспертов
			*/
			bool Analize();
			bool Init(IText * pIText);
			
		protected:
			/// анализ текущего предложения
			virtual bool AnalizeSentence()=0;
			/// множество предетерминантов
			set<wstring> m_PreDeterminerStrings;
			/// лексемы соответствующие некоторым предлогам
			set<wstring> m_StopPretexts;
			/// лексемы соответствующие некоторым наречиям
			set<wstring> m_AdverbStrings;
			/// контейнер для коллекции морфологии
			SS::Core::Features::CMorphoFeature m_MorphoFeature;
			/// контейнер для коллекций семантики
			SS::Core::Features::CSemanticFeature m_SemanticFeature;

			/// поиск предлога перед текущим юнитом для англиского
			/** 
			\param uiDepth - глубина поиска - слов не более
			\param pUnit - юнит перед которым ищется предлог

			\return 		IUnit - указатель на предлог
			*/
			IUnit * IsPrepositionBeforeE(UINT uiDepth,IUnit * pUnit,bool bPlaceTime=false);
			/// истина, если юнит есть предопределитель
			bool IsPredeterminer(IUnit* pUnit);

			/// истина, если юнит есть предлог
			bool IsPretext(IUnit * pUnit);

			/// истина, если юнит есть определитель
			bool IsDeterminer(IUnit * pUnit);

			/// поиск предлога перед текущим юнитом для русского
			/** 
			\param uiDepth - глубина поиска - слов не более
			\param pUnit - юнит перед которым ищется предлог

			\return 		IUnit - указатель на предлог
			*/
			IUnit * IsPrepositionBeforeR(UINT uiDepth,IUnit * pUnit);
			
			/// фильтрует синтаксические категории, оставляя только нужного типа ( место или время) 
			/** 
				\param l_Syntax - список категорий
				\param bTime - тип: истина - время, ложь - место 
				\return 			bool - если результирующий список не пуст
			*/
			bool FilterSyntax(list<Types::SyntaxCategories> & l_Syntax, bool bTime);
			/// проверяет что слово - стоп предлог
			bool IsStopPretextOrAimAdverb(bool bStop);
			bool IsConjunction(IUnit * pUnit);
			bool IsUpConjunction();

		};

	}
}