#pragma once

#include "IBlackBoxUnit.h"
#include "TMainAnalyseTypes.h"

#include "..\ASCInterface\ITextFeature.h"
#include "IXML.h"

#include <list>
using namespace std;


namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			namespace BlackBox
			{
				using SS::Interface::Core::CommonContainers::ITextFeature;

				/// перечисление этапов обработки предложения
				enum eProcessed
				{
					/// не обработано
					None,
					/// произведен синтаксический анализ
					Syntax,
					/// выделены сущности
					Entities
				};

				///ПРЕДЛОЖЕНИЕ
				class ISentence
				{
				public:
					///добавление юнита в конец
					virtual void AddUnit(IUnit* pUnit) = 0;

					///получение указателя на следующее предложение
					virtual ISentence* GetNextSentence() = 0;
					///получение указателя на предыдующее предложение
					virtual ISentence* GetPrevSentence() = 0;

					///получение указателя на первый юнит
					virtual IUnit* GetFirstUnit() = 0;
					///получение указателя на последний юнит
					virtual IUnit* GetLastUnit() = 0;

					///очистка предложения, включая обнуление переменных и удаление всех юнитов
					virtual void ClearSentence() = 0;
					///удаление предложения, вернет указатель на следующее(справа) предложение
					virtual ISentence* ReleaseSentence() = 0;

					/// устанавливает язык предложения
					virtual void SetLanguage(SS::Core::Types::EAnalyseLanguage eLanguage) = 0;
					/// возвращает язык предложения
					virtual SS::Core::Types::EAnalyseLanguage GetLanguage() = 0;

					/// устанавливает релевантность предложения
					virtual void SetRelevance(unsigned int uiRelevance) = 0;
					/// возвращает релевантность предложения
					virtual unsigned int GetRelevance() = 0;				

					virtual int isEnabled() = 0;

					virtual void SetEnabled(int test) = 0;

					virtual void GetLinearUnits(list<IUnit*>& allUnits) = 0;

					virtual void GetLinearUnitsFull(list<IUnit*>& allUnits) = 0;

					virtual void GetLinearUnitsNonEmpty(list<IUnit*>& allUnits) = 0;

					virtual int IsDigest() = 0;

					virtual void SetDigest(int val) = 0;

					virtual int IsPreDigest() = 0;

					virtual void SetPreDigest(int val) = 0;

					virtual int GetToneNeg() = 0;

					virtual void SetToneNeg(int val) = 0;

					virtual int GetTonePos() = 0;

					virtual void SetTonePos(int val) = 0;

					virtual ISentence* GetPrevSentenceLinear() = 0;

					virtual void SetPrevSentenceLinear(ISentence* val) = 0;

					virtual wstring GetMarkers() = 0;

					virtual void SetMarkers(wstring val) = 0;

					/// возвращает пройденный этап обработки предложения
					virtual eProcessed GetProcessed() = 0;
					/// задает пройденный этап обработки предложения
					virtual void SetProcessed(eProcessed val) = 0;
				};

				///ПАРАГРАФ
				class IParagraph
				{
				public:
					///добавление предложения в конец
					virtual void AddSentence(ISentence* pSentence) = 0;

					///получение указателя на следующий параграф
					virtual IParagraph* GetNextParagraph() = 0;
					///получение указателя на предыдующий параграф
					virtual IParagraph* GetPrevParagraph() = 0;

					///получение указателя на первое предложение
					virtual ISentence* GetFirstSentence() = 0;
					///получение указателя на последнее предложение
					virtual ISentence* GetLastSentence() = 0;

					///очистка параграфа, включая обнуление переменных и удаление всех сентенсов
					virtual void ClearParagraph() = 0;
					///удаление параграфа, вернет указатель на следующий(справа) параграф
					virtual IParagraph* ReleaseParagraph() = 0;

					///присвоение "имени" типа индекса данной текстовой части
					virtual void SetIndexTypeName(const wchar_t* wszName) = 0;
					///получение "имени" типа индекса данной текстовой части
					virtual const wchar_t* GetIndexTypeName() = 0;
					/// устанавливает релевантность параграфа
					virtual void SetRelevance(int iRelevance) = 0;
					/// возвращает релевантность параграфа
					virtual int GetRelevance() = 0;

					/// возвращает тип текст блок 
					virtual SS::Core::Types::IndexationSearch::ETextCutType GetType() = 0;
					/// устанавливает тип текст блока
					virtual void SetType(SS::Core::Types::IndexationSearch::ETextCutType eTextCutType) = 0;
					/// возвращает уровень заголовка
					virtual unsigned int GetLevel() = 0;
					/// устанавливает уровень заголовка
					virtual void SetLevel(unsigned int uiLevel) = 0;
					/// возвращает номер группы текст блок катов
					virtual unsigned int GetNumber() = 0;
					/// устанавливает номер группы текст блок катов
					virtual void SetNumber(unsigned int uiNumber) = 0;
				};

				///ТЕКСТ
				class IText : public SS::Interface::Common::IXMLStorage
				{
				public:
					///получение указателя на характеристики текста
					virtual ITextFeature* GetTextFeature() = 0;

					///добавление параграфа в конец
					virtual void AddParagraph(IParagraph* pParagraph) = 0;

					///получение указателя на первый параграф
					virtual IParagraph* GetFirstParagraph() = 0;
					///получение указателя на последний параграф
					virtual IParagraph* GetLastParagraph() = 0;

					///очистка текста, включая обнуление переменных и удаление всех параграфов
					virtual void ClearText() = 0;
					///удаление текста
					virtual void ReleaseText() = 0;

					///спускает семантические и синтаксические индексы верхних юнитов на самый нижний уровень
					virtual void Normalize() = 0;
				};

			}
		}
	}
}
