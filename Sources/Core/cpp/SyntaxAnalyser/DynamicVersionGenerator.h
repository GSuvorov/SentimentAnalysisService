//DynamicVersionGenerator.h

#pragma once
#include "BlockVersion.h"
#include "BoardElement.h"

namespace SS
{
namespace Syntax
{
namespace BoardElements
{

	///Класс для генерации динамических гипотез
	class CDynamicVersionGenerator
	{
		///Узел (элемент и текущая морфология)
		struct SNode
		{
			///Юнит
			CBoardElement* pElem;
			///Текущая морфология
			PMORPHO pMorpho;
			///Конструктор
			SNode() : pElem(NULL), pMorpho(NULL) {};
		};

	private:
		///Коллекция для инициализации итераторов
		BoardCollection m_NullCollection;
		bool isNull;
		///Первый элемент блока
		BoardElementsIterator m_itF;
		///Запоследним элемент блока
		BoardElementsIterator m_itL;
		///Гипотеза для динамического изменения
		CBlockVersion* m_pVersion;
		///Хранилище узлов
		std::vector<SNode> m_NodeCollection;
		///Итератор хранилища
		typedef std::vector<SNode>::iterator IteratorNC;
		typedef std::vector<SNode>::const_iterator ConstIteratorNC;

	private:
		///Проверка пересечения по AgreementType
		bool IsIntersectByAT( SS::Core::Features::CMorphoFeature & oMorphoOne,
							  SS::Core::Features::CMorphoFeature & oMorphoTwo,
							  SS::Core::Features::Types::AgreementType & oAt ) const;
		///Записать гипотезу
		void WriteVersion() const;
		///Найти подходящую гипотезу
		bool FindNormalVersion();
		///Сгенерировать очередную гипотезу
		bool GenerateVersion();
		///Проверить гипотезу на совпадение по АТ
		bool CheckVersion() const;

	public:
		///Конструктор
		CDynamicVersionGenerator() : m_pVersion(NULL)
		{
			//m_itF = m_NullCollection.end();
			//m_itL = m_NullCollection.end();
			isNull = true;
		}
		///Установить элементы
		void SetBlockRange(BoardElementsIterator itF, BoardElementsIterator itL);
		///Установить гипотезу 
		void SetVersion(CBlockVersion* pV) { m_pVersion = pV; };
		///Получить первую гипотезу
		bool GetFirstVersion();
		///Получить следующую гипотезу
		bool GetNextVersion();
	};

}
}
}