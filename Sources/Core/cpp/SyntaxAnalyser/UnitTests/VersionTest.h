//VersionTest.h
#pragma once

//Если тестирование
#ifdef _SS_UNITTESTS

#include "../BlockVersion.h"

namespace SS
{
namespace UnitTests
{
	
	using namespace SS::Syntax::BoardElements;
	
	///Класс для тестирования гипотез
	class CVersionTest
	{
	private:
		///Показать информацию элемента гипотезы
		void ShowElementInfo(PVERELEMENT pVElem, std::wstring wsWord = L" ") const;
		///Выравнивание строки
		void SetStandartWide(std::wstring & ws) const;
		///Преобразование string в wstring
		std::wstring StrToWstr(const std::string & s) const;

	public:
		///Проанализировать гипотезу
		void AnalyseVersion(CBlockVersion* pBlockVersion) const;
		///Проанализировать гипотезу из CBoard
		void AnalyseVersion(BoardElementsIterator itF, BoardElementsIterator itL) const;
	};

}
}

#endif //_SS_UNITTESTS