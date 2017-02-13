//SyntaxConstructionsTest.h
#pragma once
#include "..\ASSInterface\ISyntaxConstructions.h"

namespace SS
{
namespace UnitTests
{
namespace SyntaxConstructions
{
	///Класс для тестирования модуля	
	class CSyntaxConstructionsTest
	{
	private:
		///Менеджер для работы с конструкциями
		SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionsManager*	m_pSyntaxConstructionsManagerSQL;
		///Менеджер для получения интерфейсов
		SS::Interface::Core::ResourceManagers::ILoadManager*						m_pLoadManager;
	
	private:
		///Установить менеджер для работы с конструкциями
		void SetConstructionsManager();

	public:
		///Конструктор
		CSyntaxConstructionsTest(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
		///Деструктор
		~CSyntaxConstructionsTest(){};
		///Тест работы заполднения DBMS
		void TestToBinary();
	};

}
}
}