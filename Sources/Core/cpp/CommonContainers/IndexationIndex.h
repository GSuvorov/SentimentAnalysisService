//IndexationIndex.h

#pragma once

#include "..\ASCInterface\ICommonContainers.h"
#include "..\ASCInterface\TCoommonTypes.h"
#include "..\ASSInterface\IBlackBoxUnit.h"
#include "..\ASSInterface\TDictionaryManager.h"
#include "..\ASSInterface\IBlackBox.h"
#include <vector>

namespace SS
{
	namespace Core
	{
		namespace CommonContainers
		{
			using SS::Interface::Core::BlackBox::IUnit;
			using SS::Interface::Core::BlackBox::IIndex;
			using SS::Core::Types::IndexationSearch::TLinguisticProcessorMode;
			using SS::Interface::Core::BlackBox::IBlackBoxTextStorage;
			
			///Является хранилищем "индексируемых индексов"
			class CIndexationIndex : public SS::Interface::Core::CommonContainers::IIndexationIndex
			{
			private:
				IUnit*						m_pUnit;
				IIndex*						m_pCurrIndex;
				TLinguisticProcessorMode*	m_pLinguisticProcessorMode;
				IBlackBoxTextStorage*		m_pBlackBoxTextStorage;

			private:
				///Оценить применимость типа индекса к заданному режиму
				bool ApplyLinguisticProcessorMode(SS::Dictionary::Types::TDictionaryIndex & oIndex);
				///Получить очередной индекс
				unsigned int GetIndex();

			public:
				///КОНСТРУКТОРЫ
				CIndexationIndex(IUnit* pUnit, TLinguisticProcessorMode* pLinguisticProcessorMode, IBlackBoxTextStorage* pBlackBoxTextStorage);
				virtual ~CIndexationIndex();
				///МЕТОДЫ IIndexationIndex
				///получение первого индекса коллекции (0 - если нет)
				virtual unsigned int GetFirstIndex();				
				///получение очередного индекса коллекции (0 - если закончились)
				virtual unsigned int GetNextIndex();				
				///очистка коллекции (удалит все индексы)
				virtual void Clear();								
				///добавление индекса в коллекцию
				virtual void AddIndex(unsigned int uiIndex);
			};


		}
	}
}