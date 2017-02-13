#pragma once
#include "..\ASCInterface\ICommonServices.h"
#include "..\ASCInterface\ICommonContainers.h"
#include "..\ASDInterface\INDXQuery.h"
#include ".\Variant.h"
#include ".\SuperTemplate.h"
#include ".\types.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxQuery
{

///	Реализация IParametrizedQuery
/**
	Порядок работы
	Вызвать метод Init с поисковым запросом подлежащим оптимизации
	Использвать остальные методы для получения супертрансформации и вариантов
*/
class CParametrizedQuery :
	public SS::Interface::Core::NdxSE::IParametrizedQuery
{
public:
	///	Конструктор
	CParametrizedQuery(void);

	///	Деструктор
	virtual ~CParametrizedQuery(void);

	/// Инициализация запросом, сгенерированным лингвистическим процессором
	///	Порядок анализа запроса:
	///	Если установлена настройка в ss.dat "NdxProcessor::ForceSearchMode" == 7, 
	///	вызывается Init_AllOrMode, разбирает запрос
	///	Если есть индексы с флагами, вызывается Init_AnswerMode()
	///	Если нет индексов с флагами, вызывается Init_Standard()
	///	\param[in]	pQuery			Поисковый запрос
	///	\param[in]	pSearchParams	Параметры поиска
	virtual void Init(
		SS::Interface::Core::CommonContainers::IQueryResult*	pQuery,
		SS::Core::Types::IndexationSearch::TSearchParams*	pSearchParams);


	/// Возвращает указатель на супертрансформацию
	/**
		Возвращаемый указатель никогда не NULL
		Не уничтожать выданный указатель!!!
		\return	Указатель на супертрансформацию
	*/
	virtual SS::Interface::Core::NdxSE::ISuperTemplate* GetSuperTemplate();

	/// Возвращает количество вариантов запроса
	/**
		\return	Количество вариантов запроса, всегда будет равно количеству трансформаций
	*/
	virtual unsigned int GetVariantCount();

	///	Возвращает вариант по номеру
	///	\return	Указатель на вариант запроса
	virtual SS::Interface::Core::NdxSE::IVariant* GetVariant(unsigned int uiIndex);

	
	virtual BOOL	GetHasAnswers();

	virtual BOOL	GetHasUnionBlocks();

	virtual BOOL	GetHasNonObligatoryWords();

	///	Приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается	*/
	virtual HRESULT	QueryInterface(REFIID pIID, void** pBase); 

	///	Освобождение
	virtual ULONG	Release();

	///	Выводит cодержимое в строку
	virtual	const wchar_t*	ToString();

	///	Выводит сырой запрос в строку
	virtual	const wchar_t*	ToStringRawQuery(SS::Interface::Core::CommonContainers::IQueryResult* pQuery);

	///	Отладочный вывод содержимого объекта
	virtual void	DebugView();

	///	Отладочный вывод неоптимизированного запроса
	virtual void	DebugViewRawQuery(SS::Interface::Core::CommonContainers::IQueryResult* pQuery);

protected:
	///	Указатель на сервер настроек
	SS::Interface::Core::CommonServices::ISettingsServer*	m_pSettingsServer;
	///	Супертрансформация
	auto_ptr<CSuperTemplate>		m_pSuperTemplate;
	/// Контейнер для хранения варианов
	v_Variant						m_Variants;
	///	Флаг HasAnswers, выставляется после Init(...),
	///	является признаком того что в запросе были флаги ответа
	BOOL							m_HasAnswers;
	///	Флаг наличия юнион-блоков, выставляется после Init(...),
	///	является признаком того что в запросе были union-блоки
	BOOL							m_HasUB;
	///	Флаг наличия необязательных слов, выставляется после  Init(),
	///	является признаком того что в запросе были необязательные слова
	BOOL							m_HasNonObligatoryWords;

	///	Разбор запроса, не содержащего индексы с флагами
	///	\param[in]	pQuery	Поисковый запрос
	void Init_Standard(
		SS::Interface::Core::CommonContainers::IQueryResult*				_pQuery,
		SS::Core::Types::IndexationSearch::TQueryParametrizerSettings const&	_Settings);
	///	Разбор запроса, содержащего индексы с флагами
	///	\param[in]	pQuery	Поисковый запрос
	void Init_AnswerMode(
		SS::Interface::Core::CommonContainers::IQueryResult*				_pQuery,
		SS::Core::Types::IndexationSearch::TQueryParametrizerSettings const&	_Settings);
	///	Разбор запроса, всё записываем по OR. AND-блок пустой
	void Init_AllOrMode(
		SS::Interface::Core::CommonContainers::IQueryResult*				_pQuery,
		SS::Core::Types::IndexationSearch::TQueryParametrizerSettings const&	_Settings);

	//
	//	Вспомогательные методы
	//

	void ApplyWordAttributes(s_flagged_word &collection, flagged_word& currentWord);
	///	Раскладывает индексы по контейнерам, сохраняет информацию о последовательностях слов,
	///	о union-блоках в последовательностях.
	void	MakeWords(SS::Interface::Core::CommonContainers::IQueryResult* pQuery);
	///	Контейнер слов, распределённых по [трансформациям][позициям]
	mm_flagged_word			m_TransPos_Word;
	///	Контейнер содержащий информацию о распределении слов по последовательностям
	///	в трансформациях:
	///	[id трансформации][id последовательности][позиция в запросе](указатель на слово,
	///	само слово находится в m_TransPos_Word)
	mmm_pflagged_word		m_TransSeqPos_pWord;
	///	Контейнер, содержащий уникальный набор слов из всех трансформаций: set(слова)
	///	При собирании слов флаги взаимодействуют
	s_flagged_word			m_AllUnique_Words;
	///	Контейнер, содержащий уникальные слова в трансформациях, флаги взаимодействуют
	ms_flagged_word			m_UniqueTrans_Words;
	///	Контейнер, содержащий информацию о union-блоках в последовательностях
	///	[id трансформации][id последовательности](вектор начальных и конечных координат union-блоков в последовательности)
	mmv_UWB					m_UnionBlocksInfo;
	///	Контейнер в который уникально собираются все слова union-блоков, флаги взаимодействуют
	s_flagged_word			m_AllUnionWords;
	///	Строковое представление для ToStirng
	std::wstring			m_AsString;
	///	Строковое представление для ToStirng
	std::wstring			m_AsStringRawQuery;

	///	Очистка объекта
	void	Clear();
};

}
}
}
}


