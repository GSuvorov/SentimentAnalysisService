#pragma once

#include "BaseExpert.h"
#include "../ASSInterface/ISyntaxConstructions.h"

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{
			///Группа экспертов. Хранит некоторое подмножество экспертов из CExpertManager.
			class CExpertGroup
			{
			private:
				///Контейнер экспертов
				std::vector<CBaseExpert*> m_vExperts;
				///Название группы
				int m_iIdGroup;
				///Принадлежность к группам разрешения омонимии
				bool m_bIsAmbiguity;
				///Принадлежность к группам объединения блоков
				bool m_bIsUnionBlocks;

			public:
				///Тип итератора по экспертам
				typedef std::vector<CBaseExpert*>::iterator TExpertPtrItr;
				///Конструктор
				CExpertGroup(int iIdGroup) : m_iIdGroup(iIdGroup), m_bIsAmbiguity(false), m_bIsUnionBlocks(false){};
				///Деструктор
				~CExpertGroup()
				{
					for (TExpertPtrItr it = m_vExperts.begin(); it != m_vExperts.end(); ++it)
					{ delete (*it); }
				}
				///Итератор на первого эксперта
				TExpertPtrItr begin()
				{
					return m_vExperts.begin();
				}
				///Итератор после последнего эксперта
				TExpertPtrItr end()
				{
					return m_vExperts.end();
				}
				///Добавление эксперта к группе
				void AddExpert(CBaseExpert* pBaseExpert)
				{
					m_vExperts.push_back(pBaseExpert);
				}
				///Название группы экспертов
				int GetIdGroup() const
				{
					return m_iIdGroup;
				}
				///Установить группой разрешения омонимии
				void SetAsAmbiguity()
				{
					m_bIsAmbiguity = true;
					for (TExpertPtrItr it = m_vExperts.begin(); it != m_vExperts.end(); ++it)
					{ (*it)->SetAsAmbiguity(); }
				}
				///Является ли группой разрешения омонимии
				bool IsAmbiguity() const
				{
					return m_bIsAmbiguity;
				}
				///Установить группой объединения блоков
				void SetAsUnionBlocks()
				{
					m_bIsUnionBlocks = true;
				}
				///Является ли группой объединения блоков
				bool IsUnionBlocks() const
				{
					return m_bIsUnionBlocks;
				}
			};

		}
	}
}





