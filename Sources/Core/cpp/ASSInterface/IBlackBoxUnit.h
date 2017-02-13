#pragma once

#include "IBlackBoxIndex.h"
#include "IBlackBoxSyntaxUnit.h"
#include "CObligatory.h"
#include "TCitation.h"
#include "..\ASCInterface\TCoommonTypes.h"
#include <string>
#include <list>

namespace SS
{
	namespace Core
	{
		namespace BlackBox
		{
			namespace Types
			{
				struct SUnitInfo
				{
					SUnitInfo()
					{
						Clear();
					}

					void Clear()
					{
						_bIsSyntax = false;
						_shCommonFeature = 0;
						_uiFeature = 0;
						_PlaceType.SetValue(0);
						_TimeType.SetValue(0);
					}

					UINT _uiFeature;
					SS::Core::Features::Types::TSPlaceType _PlaceType;
					SS::Core::Features::Types::TSTimeIntervalType _TimeType;
					short _shCommonFeature;
					bool _bIsSyntax;
				};
			}
		}
	}
}


namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			namespace BlackBox
			{

				///юнит предложения (квази-слово)
				/**
					Юнит представляет собой некую 
					лингвистическую сущность,
					имеющую свое строковое значение и 
					обладающую набором Индексов.
					Юнит может иметь в качестве детей другие юниты.
					Например, Юнит "UN" может иметь детей одного уровня 
					"United" и "Nations"
					*/
				class IUnit : public CObligation
				{
				public:
					/// добавление дочернего юнита
					virtual void AddUnitChild(IUnit *pUnit) = 0;
					/// возврат списка дочерних юнитов
					virtual std::list<IUnit*> GetChildList() = 0;

					/// устанавливает позицию в трансформации
					virtual void SetTransPosition(unsigned char uchTransPosition) = 0;
					/// возвращает позицию в трансформации
					virtual unsigned char GetTransPosition() = 0;
					
					/// устанавливает абсолютный номер юнита в тексте
					virtual void SetPosition(unsigned int uiPosition) = 0;
					/// возвращает абсолютный номер юнита в тексте
					virtual unsigned int GetPosition() = 0;				

					/// получение порядкового номера в тексте первого символа слова
					virtual unsigned int GetFirstCharPosition() = 0;
					/// установление порядкового номера в тексте первого символа слова
					virtual void SetFirstCharPosition(unsigned int uiPosition) = 0;

					/// вернет тип подсветки индекса
					virtual SS::Core::Types::IndexationSearch::EHighlightType GetHighlightType() = 0;
					/// установит тип подсветки индекса
					virtual void SetHighlightType(SS::Core::Types::IndexationSearch::EHighlightType hlt) = 0;

					/// вернет тип подсветки индекса
					virtual bool GetIsInAnswerSentence() = 0;
					/// установит тип подсветки индекса
					virtual void SetIsInAnswerSentence(bool bIn) = 0;

					///получение строкового значения слова
					virtual const wchar_t* GetWord() = 0;
					///установление строкового значения слова
					virtual void SetWord(const wchar_t* wcWord) = 0;

					///устанавливает значение флага синтаксиса
					virtual void SetSyntaxFlag(bool bl) = 0;
					///получение значения флага синтаксиса
					virtual bool GetSyntaxFlag() = 0;

					///получение указателя на инкапсулятор синтаксических характеристик
					virtual ISyntaxFeature* GetSyntaxFeature() = 0;
					///создание и получение указателя на инкапсулятор синтаксических характеристик
					virtual ISyntaxFeature* CreateSyntaxFeature() = 0;

					///добавление индекса
					virtual void AddIndex(IIndex* pIndex) = 0;
					///получение первого индекса
					virtual IIndex* GetFirstIndex() = 0;
					///получение следующего индекса
					virtual IIndex* GetNextIndex() = 0;

					///добавление юнита на уровень ниже
					virtual void AddUnitVertical(IUnit* pUnit) = 0;
					///добавление юнита справа от этого
					virtual void AddUnitHorizontal(IUnit* pUnit) = 0;

					///добавление итема, который становится родителем для этого итема, а его родителем становится родитель этого итема 
					virtual void InsertUnitVertical(IUnit* pUnit) = 0;

					///объединение последовательности юнитов
					/**данный юнит объединяет непрерывную последовательность
					юнитов от pLeft до pRight
					т.е. становится родителем этим юнитам
					соответственно юниты должны иметь одного родителя и идти подряд*/
					virtual void JoinUpInterval(IUnit* pLeft, IUnit* pRight) = 0;

					///достает юнит из дерева, при этом не убивая ничего, вернет ссылку на юнит справа
					virtual IUnit* Pop() = 0;

					///удаление итема и его подитемов
					/**удаление итема и его подитемов
					вернет указатель на правый итем*/
					virtual IUnit* ReleaseUnitHorizontal() = 0;
					///удаление итема и его подитемов
					/**удаление итема и его подитемов
					вернет указатель на родителя*/
					virtual IUnit* ReleaseUnitVertical() = 0;

					///очистка внутренних переменных, включая удаление подитемов
					virtual void ClearUnit() = 0;

					///получение родителя этого итема
					virtual IUnit* GetParentUnit() = 0;

					///получение самого левого(первого)подитема этого итема
					virtual IUnit* GetLeftChildUnit() = 0;
					///получение самого правого(последнего)подитема этого итема
					virtual IUnit* GetRightChildUnit() = 0;

					///получение итема слева от этого
					virtual IUnit* GetLeftUnit() = 0;
					///получение итема справа от этого
					virtual IUnit* GetRightUnit() = 0;

					///получение количества детей
					virtual unsigned int GetChildCount(void) = 0;
					///проверяет наличие детей
					virtual bool HasChilds(void) = 0;
					/// проверяет наличие данного типа индексов у текущего юнита
					virtual bool HasIndexType(SS::Interface::Core::BlackBox::EIndexType oIndexType, bool bIsAnalyseChild = false) = 0;

					/// получить структуру SUnitInfo
					virtual SS::Core::BlackBox::Types::SUnitInfo* GetInfo() = 0;
					/// очистить структуру SUnitInfo
					virtual void ClearInfo() = 0;

					virtual int GetIsTargetObjectUnit() = 0;

					virtual int GetIsTargetObjectUnitFromParent() = 0;

					virtual void SetIsTargetObjectUnit(int bIsTargetObjectUnit) = 0;

					virtual unsigned int GetFirstCharPositionNew() = 0;
                    virtual int GetLastCharPositionNew(const wchar_t *pText) = 0;

					virtual void SetCitation(TCitation *cit) = 0;

					virtual TCitation* GetCitation() = 0;					

					virtual std::wstring GetWordFull() = 0;

					virtual IUnit* GetReferenceUnit() = 0;

					virtual void SetReferenceUnit(IUnit* unit) = 0;

					virtual bool IsVisual() = 0;

					virtual void SetIsTargetObjectUnitFromChilds(int bIsTargetObjectUnit) = 0;

					virtual int GetIsTargetObjectUnitFromChilds() = 0;

					virtual void SetTone(int nTone) = 0;

					virtual void SetToneStrong(int nTone) = 0;

					virtual int GetTone() = 0;
					
					virtual void SetRealTone(int nTone) = 0;

					virtual int GetRealTone() = 0;

					virtual void SetRealInversion(bool bInv) = 0;

	                virtual bool GetRealInversion() = 0;

					virtual void SetInversion(int nInversion) = 0;

					virtual int GetInversion() = 0;

					virtual void SetInverted(bool bInv) = 0;

					virtual bool GetInverted() = 0;

					virtual void SetTermNumber(int nTermNumber) = 0;

					virtual int GetTermNumber() = 0;

					virtual void SetStopWordUnit(int val) = 0;

					virtual int GetStopWordUnit() = 0;

					virtual void SetUnitToneDict(wchar_t* val) = 0;

					virtual wchar_t* GetUnitToneDict() = 0;

					virtual bool IsRusInitial() = 0;

					virtual bool IsInitial() = 0;

					virtual void SetFreeForTone(bool val) = 0;

					virtual bool GetFreeForTone() = 0;

					virtual void SetUnknownWord(bool val) = 0;

					virtual bool GetUnknownWord() = 0;

					virtual void SetPatternName(std::wstring val) = 0;

					virtual std::wstring GetPatternName() = 0;

					virtual void SetHashKey(__int64 val) = 0;

					virtual __int64 GetHashKey() = 0;

					virtual bool IsEvristicAbbr() = 0;
					
					virtual void SetEvristicLN(bool val) = 0;

					virtual bool GetEvristicLN() = 0;

					virtual void SetToneTermCount(int nTermNumber) = 0;

					virtual int GetToneTermCount() = 0;

					virtual void SetNumber(int val) = 0;

					virtual int GetNumber() = 0;	

					virtual void SetIsNominalOnly(bool val) = 0;

					virtual bool GetIsNominalOnly() = 0;

					//virtual void AddSemanticIndex(IIndex* pMorphoIndex, IIndex* pSemanticIndex) = 0;
				};
			}
		}
	}
}
