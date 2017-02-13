#pragma once

#include "..\ASSInterface\ISyntaxConstructions.h"

#include "..\ASSInterface\ICoSeDi.h"


namespace SS
{
	namespace SyntaxConstructions
	{
		namespace SQL
		{
			class CSyntaxConstructionGroupsNames : public SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionGroupsNames
			{
			public:
				///Конструктор
				CSyntaxConstructionGroupsNames(void);
				///Деструктор
				~CSyntaxConstructionGroupsNames(void);
			
				/// Возвращает список всех номеров конструкций и их имена
				void Get(/*out*/std::list<SS::SyntaxConstructions::Types::SSyntaxConstructionGroupName> * p_list_SyntaxConstructionGroup)
				{
					/*SS::SyntaxConstructions::DataTypes::SSyntaxConstructionGroupName oSyntaxConstructionGroupName;
					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgIndefinable;
					oSyntaxConstructionGroupName.m_sName	= "общий поток";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgVerbObjecrBinder;
					oSyntaxConstructionGroupName.m_sName	= "общий группа определения связи глаголов и объектов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgInfinitiveVerbChainMorphoResolve;
					oSyntaxConstructionGroupName.m_sName	= "группа разрешения морфологической омонимии инфинитивных глагоных цепочек";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgModalVerbChainMorphoResolve;
					oSyntaxConstructionGroupName.m_sName	= "группа разрешения морфологической омонимии глагоных цепочек модальных глаголов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgModalVerbChainBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения глагоных цепочек модальных глаголов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgPrepositionPhraseBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения предложных групп";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgNounPostModifierBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения постмодификаторов существительного";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgAuxVerbChainMorphoResolve;
					oSyntaxConstructionGroupName.m_sName	= "группа разрешения морфологической омонимии глагоных цепочек вспомогательных глаголов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgInfinitiveVerbChainBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения инфинитивных глагоных цепочек";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgNounPhraseBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения групп существительного";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgSubjectVerbBinder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения субъекта финитного предложения";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgSubjectVerbBinderIndexation;
					oSyntaxConstructionGroupName.m_sName	= "группа определения субъекта финитного предложения(индексация)";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgSubjectVerbBinderQuery;
					oSyntaxConstructionGroupName.m_sName	= "группа определения субъекта финитного предложения(запрос)";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgAuxVerbChainBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения глагоных цепочек вспомогательных глаголов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgSubjectMainBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения связи субъектов и объектов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgUtility;
					oSyntaxConstructionGroupName.m_sName	= "группа вспомогательных экспертов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgQuestionWordDetector;
					oSyntaxConstructionGroupName.m_sName	= "группа определения вопросительных слов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgNounChainBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа построения цепочек существительных";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);
					
					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgVerbAdverbBinder;
					oSyntaxConstructionGroupName.m_sName	= "группа связывания глаголов со служебными частями";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);
					
					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgQuestionVerbChainBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения глагольных цепочек в запросе";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgMorphologyAmbiguityResolveCommon;
					oSyntaxConstructionGroupName.m_sName	= "группа разрешения морфологической омонимии для всех видов разбора";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgMorphologyAmbiguityResolveIndexation;
					oSyntaxConstructionGroupName.m_sName	= "группа разрешения морфологической омонимии для индексации";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgMorphologyAmbiguityResolveQuery;
					oSyntaxConstructionGroupName.m_sName	= "группа разрешения морфологической омонимии для разбора запроса";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgModAuxQuestVerbBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения связи вспомогательного глагола с основным в запросе";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgAdjectivePhraseBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения связи прилагательного с артиклем";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgInfinitiveParticiplePhraseBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения связи инфинитива и причастия";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgIntransitiveRemover;
					oSyntaxConstructionGroupName.m_sName	= "группа, удаляющая гипотезы с непереходными глаголами";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgVerbComplementBinder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения связи глаголов и дополнений к субъекту";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgBeNounParticipleBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа, связывающая be + Noun + Participle";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgTimePlacePrepBuilder;
					oSyntaxConstructionGroupName.m_sName	= "Группа связи специальных классов предлогов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgTimePlacePrepPostBuilder;
					oSyntaxConstructionGroupName.m_sName	= "Группа связи специальных классов предлогов с глаголами";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgUniformMembersBuilder;
					oSyntaxConstructionGroupName.m_sName	= "Группа связи однородных членов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);*/
				}
			public:
				/// сохраняет в бинарный вид структуры данных 
				bool ToBinary(){};
				/// удаляет текущую конструкцию
				void Delete()
				{}
				/// добаляет текущую конструкцию
				void Add()
				{}
				/// обновляет текущую конструкцию
				void Update()
				{}
			};
		};

		namespace DBMS
		{
			class CSyntaxConstructionGroupsNames : public SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionGroupsNames
			{
			public:
				/// конструктор
				CSyntaxConstructionGroupsNames(void);
				/// деструктор
				~CSyntaxConstructionGroupsNames(void);
			
				/// Возвращает список всех номеров конструкций и их имена
				void Get(/*out*/std::list<SS::SyntaxConstructions::Types::SSyntaxConstructionGroupName> * p_list_SyntaxConstructionGroup)
				{
					/*SS::SyntaxConstructions::Types::SSyntaxConstructionGroupName oSyntaxConstructionGroupName;

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgUtilityQuery;
					oSyntaxConstructionGroupName.m_sName	= "группа вспомогательных экспертов для разбора запроса";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					//oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgUniformMembersBuilder;
					//oSyntaxConstructionGroupName.m_sName	= "группа связи однородных членов";
					//p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgIndefinable;
					oSyntaxConstructionGroupName.m_sName	= "общий поток";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);
					
					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgMorphologyAmbiguityResolveCommon;
					oSyntaxConstructionGroupName.m_sName	= "группа разрешения морфологической омонимии для всех видов разбора";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgMorphologyAmbiguityResolveIndexation;
					oSyntaxConstructionGroupName.m_sName	= "группа разрешения морфологической омонимии для индексации";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgMorphologyAmbiguityResolveQuery;
					oSyntaxConstructionGroupName.m_sName	= "группа разрешения морфологической омонимии при разборе запроса";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgInfinitiveVerbChainMorphoResolve;
					oSyntaxConstructionGroupName.m_sName	= "группа разрешения морфологической омонимии инфинитивных глагоных цепочек";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgModalVerbChainMorphoResolve;
					oSyntaxConstructionGroupName.m_sName	= "группа разрешения морфологической омонимии глагоных цепочек модальных глаголов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgUtility;
					oSyntaxConstructionGroupName.m_sName	= "группа вспомогательных экспертов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgAuxVerbChainMorphoResolve;
					oSyntaxConstructionGroupName.m_sName	= "группа разрешения морфологической омонимии глагоных цепочек вспомогательных глаголов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgNounChainBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа построения цепочек существительных";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgNounPhraseBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения групп существительного";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgBeNounParticipleBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа, связывающая be + Noun + Participle";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgSubjectVerbBinder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения связывающая субъект и следующий за ним глагол //07.09";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgSubjectVerbBinderIndexation;
					oSyntaxConstructionGroupName.m_sName	= "";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgSubjectVerbBinderQuery;
					oSyntaxConstructionGroupName.m_sName	= "";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgModalVerbChainBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения глагоных цепочек модальных глаголов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgAuxVerbChainBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения глагоных цепочек вспомогательных глаголов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgQuestionWordDetector;
					oSyntaxConstructionGroupName.m_sName	= "группа определения вопросительных слов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgVerbAdverbBinder;
					oSyntaxConstructionGroupName.m_sName	= "группа связывания глаголов со служебными частями";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgAdjectivePhraseBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения связи прилагательного с артиклем";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgTimePlacePrepBuilder;
					oSyntaxConstructionGroupName.m_sName	= "Группа связи специальных классов предлогов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgPrepositionPhraseBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения предложных групп";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgTimePlacePrepPostBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа связи специальных классов предлогов с глаголами";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgIntransitiveRemover;
					oSyntaxConstructionGroupName.m_sName	= "группа, удаляющая гипотезы с непереходными глаголами";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgInfinitiveParticiplePhraseBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения связи инфинитива и причастия";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgInfinitiveVerbChainBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения инфинитивных глагоных цепочек";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgNounPostModifierBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения постмодификаторов существительного";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgSubjectMainBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения связи субъектов и объектов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgModAuxQuestVerbBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения связи вспомогательного глагола с основным в запросе";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgQuestionVerbChainBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения глагольных цепочек в запросе";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgVerbComplementBinder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения связи глаголов и дополнений к субъекту";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgVerbObjecrBinder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения связи глаголов и объектов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgAllBinder;
					oSyntaxConstructionGroupName.m_sName	= "группа для досвязывания всех не связанных элементов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);
					//Павленко
					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgUniformMembersBinderPostHyp;
					oSyntaxConstructionGroupName.m_sName	= "группа связи однородных членов после разбиения на гипотезы";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);
					//
					//oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgAuxVerbChainBuilderIndexation;
					//oSyntaxConstructionGroupName.m_sName	= "группа определения глагольных цепочек вспомогательных глаголов";
					//p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);
					//
					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgUniformMembersBinderFinal;
					oSyntaxConstructionGroupName.m_sName	= "группа связи однородных членов после разбиения на гипотезы";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);


					/*oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgIndefinable;
					oSyntaxConstructionGroupName.m_sName	= "общий поток";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgVerbObjecrBinder;
					oSyntaxConstructionGroupName.m_sName	= "общий группа определения связи глаголов и объектов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgInfinitiveVerbChainMorphoResolve;
					oSyntaxConstructionGroupName.m_sName	= "группа разрешения морфологической омонимии инфинитивных глагоных цепочек";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgModalVerbChainMorphoResolve;
					oSyntaxConstructionGroupName.m_sName	= "группа разрешения морфологической омонимии глагоных цепочек модальных глаголов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgModalVerbChainBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения глагоных цепочек модальных глаголов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgPrepositionPhraseBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения предложных групп";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgNounPostModifierBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения постмодификаторов существительного";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgAuxVerbChainMorphoResolve;
					oSyntaxConstructionGroupName.m_sName	= "группа разрешения морфологической омонимии глагоных цепочек вспомогательных глаголов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgInfinitiveVerbChainBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения инфинитивных глагоных цепочек";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgNounPhraseBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения групп существительного";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgSubjectVerbBinder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения субъекта финитного предложения";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgAuxVerbChainBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения глагоных цепочек вспомогательных глаголов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgSubjectMainBuilder;
					oSyntaxConstructionGroupName.m_sName	= "группа определения связи субъектов и объектов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgUtility;
					oSyntaxConstructionGroupName.m_sName	= "группа вспомогательных экспертов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);

					oSyntaxConstructionGroupName.m_ID		= SS::Syntax::Types::tcgAllBinder;
					oSyntaxConstructionGroupName.m_sName	= "группа для досвязывания всех не связанных элементов";
					p_list_SyntaxConstructionGroup->push_back(oSyntaxConstructionGroupName);*/
				}
			public:
				/// удаляет текущую конструкцию
				void Delete()
				{}
				/// добаляет текущую конструкцию
				void Add()
				{}
				/// обновляет текущую конструкцию
				void Update()
				{}
			};
		};
	};
};
