#pragma once

#include "..\ASCInterface\ICommonServices.h"

namespace SS
{
namespace Core
{
namespace CommonServices
{

//--------------------------------------------------------------------//

///хранение временных характеристик выполнения участка кода
class CTickMeasure
{
public:
	CTickMeasure():m_iStartTick(0), m_iStopTick(0), m_iSumTick(0), m_uiStartsCount(0){};
	virtual ~CTickMeasure(){};

	///количество замеров
	unsigned int m_uiStartsCount;
	///текущий старт
	__int64 m_iStartTick;
	///текущий останов
	__int64 m_iStopTick;
	///общее время выполнения
	__int64 m_iSumTick;
};

//--------------------------------------------------------------------//

///управление подсчетом времени выполнения участка кода
class CPerformanceMeasure : public SS::Interface::Core::CommonServices::IPerformanceMeasure
{
public:
	typedef stdext::hash_map<std::wstring, CTickMeasure> TTickMeasures;

	CPerformanceMeasure();
	virtual ~CPerformanceMeasure();

	///запуск подсчета времени выполнения
	void	StartCalculation(const wchar_t* szID);
	///очистка тикера по идентификатору
	void	ClearCalculation(const wchar_t* szID);
	///очистка тикеров
	void	ClearCalculations();
	///останов подсчета времени выполнения
	void	StopCalculation(const wchar_t* szID);
	///возврат времени выполнения
	double	GetCalculation(const wchar_t* szID);
	///отображение времени выполнения
	void	ViewCalculation(const wchar_t* szID);
	///отображение всех замеров времени выполнения
	void	ViewCalculations(void);
	///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
	HRESULT QueryInterface(REFIID pIID, void** pBase); 
	///освобождение
	ULONG Release();

protected:
	///коллекция замеров времени выполнения
	TTickMeasures m_TickMeasures;
};

//--------------------------------------------------------------------//


}
}
}

