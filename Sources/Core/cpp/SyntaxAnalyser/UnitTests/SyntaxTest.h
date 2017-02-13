//SyntaxTest.h
#pragma once 

#ifdef _SS_UNITTESTS

#include "../../ASSInterface/ILexicaLib.h"
#include "../../ASSInterface/ISyntaxAnalyzer.h"
#include "../../ASSInterface/ISemanticAnalyze.h"

namespace SS
{
	namespace UnitTests
	{
		namespace Syntax
		{
			///Класс для тестирования данного модуля
			class CSyntaxTest
			{
			private:
				///Лексический анализатор
				SS::Interface::Core::LexicalAnalyze::ILexica*			m_pLexic;
				///Синтаксический анализатор
				SS::Interface::Core::ISyntaxAnalyzer*					m_pSyntax;
				///Семантический анализатор
				SS::Interface::Core::ISemanticAnalyze*					m_pSemantic;
				///Парамметры запроса
				SS::Interface::Core::MainAnalyse::IAnalyseParams*		m_pQueryParams;
				///Параметры индексации
				SS::Interface::Core::MainAnalyse::IAnalyseParams*		m_pIndexationParams;
				///Менеджер загрузки
				SS::Interface::Core::ResourceManagers::ILoadManager*		m_pLoadManager;
				
				//Тип разбора
				enum EWorkType {eIndexation, eQuery};
				
			private:
				///Магический код для модуля Dictionary
				void RunDictionariesMagicCode(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
				///Получение IText* из установленной строки
				SS::Interface::Core::BlackBox::IText* GetText(std::wstring str, EWorkType type);

			public:
				///Конструктор
				CSyntaxTest(SS::Interface::Core::ResourceManagers::ILoadManager*	pLoadManager);
				///Деструктор
				~CSyntaxTest(){};
				///Общий тест
				void TestCommon();
				///Тестирует скорость разбора для индексации
				void IndexationSpeedTest();
				///Тест для шаблона контейнера
				void TestContainer();
			};
		}
	}
}

#endif