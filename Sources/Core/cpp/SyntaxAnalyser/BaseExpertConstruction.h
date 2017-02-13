//BaseExpertConstruction.h
#pragma once

#include "BaseExpert.h"
#include "ModifiedConstruction.h"

namespace SS
{
namespace Syntax
{
namespace ExpertModel
{
	using namespace SS::Syntax::ModifiedConstruction;
	
	///Базовый эксперт для экспертов загружаемых из базы
	class CBaseExpertConstruction : public CBaseExpert
	{
	private:
		///Конструкция из базы
		SS::SyntaxConstructions::Types::CConstructionEx m_oConstruction;
		///Идентификатор группы
		int m_IdGroup;
		///Детект-слова
		std::list< std::set<std::wstring> > m_DetectWords;
		
	protected:
		///Модифицированная конструкция
		IModifiedConstruction* m_pModifiedConstruction;
		///Первый елемент поступившего предложения
		BoardElementsIterator m_itBeginBoard;
		///Первый элемент блока
		BoardElementsIterator m_itBlockBegin;
		///ЗаПоследним элемент блока
		BoardElementsIterator m_itBlockEnd;
		///Первый элемент всего диапазона поиска
		BoardElementsIterator m_itFirst;
		///Последний элемент всего диапазона поиска
		BoardElementsIterator m_itLast;
		///Первый элемент правого(от детект-элемента) диапазона
		BoardElementsIterator m_itFirstR;
		///Последний элемент левого(от детект-элемента) диапазона
		BoardElementsIterator m_itLastL;
		///Коллекция для инициализации итераторов
		BoardCollection m_NullCollection;

	private:
		///Установить элементам диапазон
		void SetRange();
		///Установить детект-слова
		void SetDetectWords(SS::Interface::Core::Dictionary::IDictionary* pDictionary);
		///Расчитать IdParent с учетом SameObject
		inline unsigned int GetIdParentReal(unsigned int idParent);
		///Освободить память
		inline void Delete();
		
	protected:
		///Применить конструкцию
		void ApplyConstruction(unsigned int iCountApplyedExpertAT = 0);
		///Проверка применимости эксперта к предложению
		bool IsApplicable();

	public:
		///Конструктор
		CBaseExpertConstruction( SS::SyntaxConstructions::Types::CConstructionEx & oConstruction,
								 SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager,
								 SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams,
								 int IdGroup, ESyntaxBase eBase );
		///Деструктор
		virtual ~CBaseExpertConstruction();
		///Возвращает текстовое название эксперта
		virtual std::wstring GetName() const;
	};

}
}
}