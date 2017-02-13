//TextBlockEx.h
#pragma once
#include "TextBlock.h"
#include "..\ASSInterface\ITextBlockEx.h"
#include "..\ASSInterface\IBlackBoxUnit.h"
#include "..\ASSInterface\IBlackBoxItems.h"
#include "..\ASSInterface\IBlackBox.h"

namespace SS
{
	namespace Core
	{
		namespace CommonContainers
		{

			using SS::Interface::Core::CommonContainers::ITextFeature;
			using SS::Interface::Core::CommonContainers::ITextBlockEx;
			using SS::Interface::Core::CommonContainers::IIndexAndPosition;
			using SS::Interface::Core::ResourceManagers::ILoadManager;
			using SS::Interface::Core::BlackBox::IText;
			using SS::Interface::Core::BlackBox::IUnit;
			using SS::Core::Types::IndexationSearch::TIndexationParams;
			using SS::Core::Types::IndexationSearch::TSearchParams;
			using SS::Core::Types::IndexationSearch::TLinguisticProcessorMode;
			using SS::Interface::Core::BlackBox::IBlackBoxTextStorage;

			///реализует функциональность ITextBlock + его заполнение из IText + формирование IText из заполненного ITextBlock
			class CTextBlockEx : public ITextBlockEx, public CTextBlock
			{
			private:
				ILoadManager*				m_pLoadManager;
				IBlackBoxTextStorage*		m_pBlackBoxTextStorage;
				TLinguisticProcessorMode	m_LinguisticProcessorMode;
			
			private:
			    ///Устанавливает индексы IndicsationIndex и ServiceIndex
				void SetIndexes(ITextBlockCut* pTextBlockCut, IUnit* pUnit);
				///Копирует TextFeature
				void CopyTextFeature(ITextFeature* ptfTo, ITextFeature* ptfFrom);
				///Получение BlackBoxTextStorage
				void InitConnection();
				void ViewTextBlockCut(ITextBlockCut* pTextBlockCut);
				///Копирование в блок-кат информации из параграф
				inline void ParagraphToCut( SS::Interface::Core::BlackBox::IParagraph* pParagraph,
											SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut);
				///Копирование в параграф информации из блок-ката
				inline void CutToParagraph( SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut,
											SS::Interface::Core::BlackBox::IParagraph* pParagraph);
			
			public:
			    ///Конструктор
				CTextBlockEx(ILoadManager* pLoadManager);
				///Деструктор
				virtual ~CTextBlockEx();
			    ///МЕТОДЫ ITextBlockEx
				///Заполнение ITextBlock из поданного IText
				virtual void SetIText(IText* pIText, bool bIsCopyTextFeature = true);
				///Формирование IText из полного ITextBlock
				virtual IText* GenerateIText();
				///Установление режима лингвистического процессора
				virtual void SetLinguisticProcessorMode(TLinguisticProcessorMode* pLinguisticProcessorMode);
				///Выполняет обновление релевантности
				virtual void UpdateRelevance();
				///ПЕРЕОПРЕДЕЛЯЮ ИЗ CTextBlock
				///Создание текстовой фичи
				virtual ITextFeature* GetTextFeature();
				///ПЕРЕОПРЕДЕЛЯЮ ИЗ ITextBlock
				///Добавит вырезку в хранилище (добавит себе и вернет ссылку)
				virtual ITextBlockCut* AddBlockCut();
			};

		}
	}
}