//ServiceIndex.h

#pragma once

#include "..\ASCInterface\ICommonContainers.h"
#include "..\ASSInterface\IBlackBoxUnit.h"
#include "..\ASSInterface\IBlackBox.h"
#include "..\ASCInterface\TCoommonTypes.h"
#include "..\ASSInterface\TDictionaryManager.h"
#include <list>

namespace SS
{
	namespace Core
	{
		namespace CommonContainers
		{

			///Класс реализует функциональность преобразования данных в буфер и обратно
			///Ограничения: Размер не может быть нулевым, Последовательность вызовов строго определена
			class CBufferServiceIndex
			{
			private:
				char* m_pBuff;
				unsigned int m_iSize;
				unsigned int m_iPos;
			
			public:
				CBufferServiceIndex() : m_pBuff(NULL), m_iSize(0), m_iPos(0) {};
				//Деструктор
				~CBufferServiceIndex() 
				{
					if (m_pBuff) 
					{ delete [] m_pBuff; }
				};
				//Выделение памяти под новый буфер(при этом старый уничтожается) 
				void Allocate(unsigned int iSize)
				{
					m_iSize = iSize;
					m_iPos = 0;
					if (m_pBuff)
					{ delete [] m_pBuff; }
					m_pBuff = new char[m_iSize];
				}
				//Добавление в буфер данных
				void PutToBuffer(void* pData, unsigned int iSizeData)
				{
					if ( (!pData) || (iSizeData == 0) || ((m_iPos + iSizeData) > m_iSize) )
					{ SS_THROW(L"Некорректное добавление в буфер."); }
					
					memcpy(m_pBuff + m_iPos, pData, iSizeData);
					m_iPos += iSizeData;
				}
				//Чтение данных из буфера
				void GetFromBuffer(void* pData, unsigned int iSizeData)
				{
					if ( (!pData) || (iSizeData == 0) || ((m_iPos + iSizeData) > m_iSize) )
					{ SS_THROW(L"Некорректное считывание из буфера."); }
					
					memcpy(pData, m_pBuff + m_iPos, iSizeData);
					m_iPos += iSizeData;
				}
				//Установить буфер для чтения из него
				void SetBuffer(char* pBuff, unsigned int iSize)
				{
					if (m_pBuff)
					{ delete [] m_pBuff; }
					m_pBuff = pBuff;
					m_iSize = iSize;
					m_iPos = 0;
				}
				//Получить буфер
				char* GetBuffer()
				{
					return m_pBuff;
				}
				//Сброс значений
				void Reset()
				{
					m_pBuff = NULL;
					m_iSize = 0;
					m_iPos = 0;
				}
				//Оператор==
				bool operator==(const char* pBuff)
				{
					return (m_pBuff == pBuff);
				}
			};
			
			///"сохраняемый индекс" инкапсулирует индекс, сохраняемый/доставаемый в сервисной базе
			/**
			всегда входит в состав IIndexAndPosition (т.е. на одной позиции-один такой индекс)
			обязательно включает в себя индекс для отображения, а также может включать любую дополнительною сохраняемую информацию
			*/
			
			using SS::Core::Types::IndexationSearch::TViewIndex;
			using SS::Interface::Core::BlackBox::IUnit;
			using SS::Interface::Core::BlackBox::IBlackBoxTextStorage;
			using SS::Core::Types::IndexationSearch::TLinguisticProcessorMode;			

			class CServiceIndex : public SS::Interface::Core::CommonContainers::IServiceIndex
			{
			private:
				CBufferServiceIndex			m_oBuffer;
				IUnit*						m_pUnit;
				IBlackBoxTextStorage*		m_pBlackBoxTextStorage;
				TViewIndex					m_oViewIndex;
				TLinguisticProcessorMode*	m_pLinguisticProcessorMode;
			
			private:
				///Удалить все индексы
				void Clear();
				///Занесение в буфер индексов из Unit
				void GetIndexesFromUnit(std::list<TViewIndex> & lstViewIndex, bool bOnlyFirst = false);
			
			public:
				//КОНСТРУКТОРЫ
				CServiceIndex(IUnit* pUnit, TLinguisticProcessorMode* pLinguisticProcessorMode, IBlackBoxTextStorage* pBlackBoxTextStorage);
				virtual ~CServiceIndex();
				///МЕТОДЫ IServiceIndex
				///Вернет индекс для отображения (Позже возвращать константу)
				virtual SS::Core::Types::IndexationSearch::TViewIndex* GetViewIndex();							
				///Вернет буфер дополнительной информации 
				virtual void GetInfoBuff(char **ppBuff, unsigned int* pSize);	
				///Запишет себе буфер дополнительной информации 
				virtual void SetInfoBuff(char *pBuff, unsigned int Size);
			};


		}
	}
}