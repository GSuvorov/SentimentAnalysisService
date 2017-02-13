#pragma once

#include "..\assinterface\iblackbox.h"
#include ".\StringService.h"
#include "..\assinterface\IDictionaryManager.h"


namespace SS
{
	namespace Semantic
	{

		typedef TCHAR * TCharType;
		/// определение типа лексем
		class CLexemType
		{
		public:
			CLexemType(void);
			~CLexemType(void);
			/// инициализация указателя на указатель текущего юнита 
			void Init(SS::Interface::Core::BlackBox::IUnit ** ppUnit) { m_ppCurentUnit=ppUnit; };
			
			/// истина если слово равно поданнной строке
			/** 
				\param pcWord - строка
				\param  pUnit - слово
				\return 			bool - истна - если равно
			*/
			bool WordEqualsTo(const TCharType pcWord,SS::Interface::Core::BlackBox::IUnit* pUnit=NULL );
			/// истина, если юнит - слово , и его длина не более uiLessOrEqual
			/** 
				\param uiLessOrEqual=0 - длина слова, если =0 - то длина может быть произвольной
				\param pUnit=NULL
				\return 			bool
			 
			-detailed description- 
			*/
			bool IsWord(UINT uiLessOrEqual=0,SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			/// истина, если первая буква слова в верхнем регистре
			bool IsUpperCase(SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			//bool IsEng(UINT uiLessOrEqual=0,SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			//bool Iseng(UINT uiLessOrEqual=0,SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			//bool IsENG(UINT uiLessOrEqual=0,SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			//bool IsEnG(UINT uiLessOrEqual=0,SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			//bool IsRus(UINT uiLessOrEqual=0,SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			//bool Isrus(UINT uiLessOrEqual=0,SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			//bool IsRUS(UINT uiLessOrEqual=0,SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			/// вернёт истину, если слово - симво
			bool IsSymbol(SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			
			/// вернёт истину, если слово равно данному символу символу
			/** 
				\param pcSymbol - символ
				\param pUnit=NULL  - слово 
			*/
			bool SymbolEqualsTo(TCharType  pcSymbol,SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			/// возвращает длину слова
			/** 
				\param pUnit=NULL - слово
				\return 			UINT - длина
			*/
			UINT GetLexemLength(SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);

			/// истина, если слово есть цифры
			/** 
				\param uiLessOrEqual=0 - длина слова не меньше чем, если 0 - то любая
				\param pUnit=NULL - слово 
				\return 			bool -
			*/
			bool IsDigits(UINT uiLessOrEqual=0,SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			/// истина если слово - кавычки
			bool IsSQuotes(SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			/// истина если слово - тире
			bool IsSTire(SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			/// истина, если слово - дефис
			bool IsSDefis(SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			/// истина, если слово - запятая
			bool IsSComma(SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			/// истина если слово - точка
			bool IsSDot(SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			/// истина, если слово кавычки
			bool IsSApostrophe(SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
			/// истина - если пустое слово
			bool IsEmpty(SS::Interface::Core::BlackBox::IUnit* pUnit=NULL){ return false; };
		private:
			/// указатель на текущий юнит эксперта
			SS::Interface::Core::BlackBox::IUnit ** m_ppCurentUnit;
			/// манипуляции со строками
			CStringService m_StringService;
		};
	}
}