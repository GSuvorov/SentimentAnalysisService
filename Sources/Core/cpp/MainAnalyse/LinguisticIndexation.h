#pragma once

#include "../ASCInterface/ILinguisticProcessor.h"

#include "./LinguisticWorkingInterfaces.h"
#include "./TextIndexation.h"
#include "./TableIndexation.h"

namespace SS
{
namespace LinguisticProcessor
{
namespace Indexation
{
	unsigned int GetLastPositon(SS::Interface::Core::BlackBox::IText * pText);
	unsigned int GetFistPositon(SS::Interface::Core::BlackBox::IText * pText);
	unsigned int GetUnitLastPositon(SS::Interface::Core::BlackBox::IUnit * pUnit);

	class CLinguisticIndexation : public SS::Interface::Core::ILinguisticIndexation

	{
		bool m_bIsTextIndexation;
		bool m_bIsTableIndexation;
		SS::LinguisticProcessor::Indexation::CTextIndexation			m_oTextIndexation;
		SS::LinguisticProcessor::Indexation::CTableIndexation		m_oTableIndexation;
		unsigned int m_uiCurPosition;
		unsigned int uiFistCharPosition;

	public:
		CLinguisticIndexation(void);
		~CLinguisticIndexation(void);
	public:
		///запуск пакетной индексации документа
		HRESULT StartDocumentIndexation();

		/// инициирует процесс индексации очередного текстового блока очередного текста 
		void Init(
			/// контейнер с индексируемым текстом
			SS::Core::Types::IndexationSearch::TNativeTextForIndexation* pNativeText
			/// параметры индексации текста
			, SS::Core::Types::IndexationSearch::TIndexationParams* pIndexationParams
			);

		/// инициирует процесс индексации очередной таблицы очередного текста 
		void Init(
			std::list< std::list<SS::Core::Types::IndexationSearch::TNativeTextForIndexation*> > * p_l_l_NativeText
			/// параметры индексации текста
			, SS::Core::Types::IndexationSearch::TIndexationParams* pIndexationParams
			);

		/// генерирует очередную порцию индекса
		/**
		вернет:
			0 - если есть еще части для анализа
			1 - если больше нет
			2 - если была ошибка
		*/
		unsigned int GenerateNext(
			/// указатель на контейнер, который необходимо заполнить
			SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult
			);
		///останов пакетной индексации документа
		HRESULT EndDocumentIndexation();


	public:
		virtual void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
	public:
		void SetMode(SS::Core::Types::IndexationSearch::TLinguisticProcessorMode* pMode);
	private:
		//void SaveText(int iTextNumber, const wchar_t* wzText, const wchar_t* wzTextType);
		//void SaveNativeText(SS::Core::Types::IndexationSearch::TNativeTextForIndexation* pNativeText); 

	};
}
}
}