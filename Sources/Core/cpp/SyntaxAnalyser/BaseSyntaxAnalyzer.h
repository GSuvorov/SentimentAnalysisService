#pragma once

#include "./IExpertController.h"
#include "../ASSInterface/ISyntaxAnalyzer.h"

#include "./ExpertController.h"

namespace SS
{
	namespace Syntax
	{
		///Класс, осуществляющий синтаксический анализ текста.
		class CBaseSyntaxAnalyzer :	public SS::Interface::Core::ISyntaxAnalyzer
		{
		private:
			///Контроллер экспертов
			SS::Syntax::ExpertModel::CExpertController m_ExpertController;
		
		private:
			///Производит загрузку групп конструкций
			void Init();
			///Сброс флага индексации для всех пустых индексов.
			void ResetIndexationFlags( SS::Interface::Core::BlackBox::IUnit* first,
									   SS::Interface::Core::BlackBox::IUnit* last );
			///Создание для каждого пришедшего юнита Синтаксической фичи по умолчанию
			void CreateSyntax(SS::Interface::Core::BlackBox::ISentence* pSentence) const;
		
		public:
			/// Конструктор
			CBaseSyntaxAnalyzer(SS::Syntax::ExpertModel::ESyntaxBase eBase);
			///Производит синтаксический анализ текста
			virtual void AnalyzeNext(SS::Interface::Core::BlackBox::IText* text);
			///Производит синтаксический анализ предложения
			virtual void AnalyzeNext(SS::Interface::Core::BlackBox::ISentence* sentence);
			///освобождение
			ULONG Release();
			///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
			HRESULT QueryInterface(REFIID pIID, void** pBase);
			///переопределяю от IBase
			void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
			///переопределяю от CAnalyseParams
			void SetAnalyseParams(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams);
		};
	}
}