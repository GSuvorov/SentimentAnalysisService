#pragma once
#include <stdio.h>

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxBaseManager
{

//--------------------------------------------------------------------//

///коды возможных ошибок модуля
enum ErrorCode
{
	///нулевое значение 
	erIsNull,
	///значение не найдено
	erNotFind,
	///значение не эквивалентно
	erNotEqual,
	///ошибка DBMS
	erDBMS,
	///ошибка выполнения вылетели в кетч
	erTryCatch,
	///невалидный хендл
	erBadHandle,
	///файловая ошибка
	erFileIO
};

///освобождает память по указателю
int st_free (void *mem);
///выделяет память по указателю
void* st_malloc		 (unsigned long n_bytes);

///выводит на консоль
void ToConsole(const wchar_t* pszString);
///выводит на консоль
void ToConsoleEx(const wchar_t* pszString, const wchar_t* pszDest, const wchar_t* pszFile, long Line);
///выводит на консоль
void ToConsole(const wchar_t* pszString, double dbValue);
///выводит на консоль
void ToConsole(const wchar_t* pszString, unsigned int uiValue);
///выводит на консоль
void ToConsole(unsigned char* pBuffer, unsigned int uiSize);
///выводит на консоль состояние памяти
void ViewMemoryStatus(void);
///возвращает аллокированную память
unsigned int GetMemoryStatus(void);
///выводит ошибку с кодом на консоль
void ErrorToConsole(ErrorCode EC, const wchar_t* pszString, const wchar_t* pszDest, const wchar_t* pszFile, long Line);

extern bool g_bStopLogging;

#define UT_STOP_LOGING \
		g_bStopLogging=true;
#define UT_START_LOGING \
		g_bStopLogging=false;

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)

#define __WFILE__ WIDEN(__FILE__)
#define __WFUNCTION__ WIDEN(__FUNCTION__)

///проверяет на нулевое значение
#define ISNULL(x) \
	x==NULL?ErrorToConsole(erIsNull, WIDEN(#x), __WFUNCTION__,  __WFILE__, __LINE__), true:false
///выводит ошибку на консоль
#define ERR_TO_CONSOLE(code, s) \
{ErrorToConsole(code, s, __WFUNCTION__, __WFILE__, __LINE__);}
///выводит сообщение на консоль
#define TO_CONSOLE(s) \
	ToConsoleEx(s, __WFUNCTION__,  __WFILE__, __LINE__);

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
class CTickManager  
{
public:
	
	#pragma warning( disable : 4996 )
	typedef stdext::hash_map<wstring, CTickMeasure> TTickMeasures;
	#pragma warning( default : 4996 )

	CTickManager();
	virtual ~CTickManager();

	///запуск подсчета времени выполнения
	void	StartTick(const wchar_t* szID);
	///останов подсчета времени выполнения
	void	StopTick(const wchar_t* szID);
	///возврат времени выполнения
	double	GetTicks(const wchar_t* szID);
	///отображение времени выполнения
	void	ViewTicks(const wchar_t* szID);
	///отображение всех замеров времени выполнения
	void	ViewTicks(void);

protected:
	///коллекция замеров времени выполнения
	TTickMeasures m_TickMeasures;
};

//--------------------------------------------------------------------//

///объект управляющий подсчетом времени выполнения
extern CTickManager TickManager;

///запускает подсчет времени по идентификатору
#define START_TICK(sID) \
	TickManager.StartTick(sID);

///останавливает подсчет времени по идентификатору
#define STOP_TICK(sID) \
	TickManager.StopTick(sID);

///отображение времени выполнения по идентификатору
#define VIEW_TICK(sID) \
	TickManager.ViewTicks(sID);

///отображение всех замеров времени выполнения
#define VIEW_TICKS() \
	TickManager.ViewTicks();

//--------------------------------------------------------------------//

}
}
}
}
