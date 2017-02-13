/******************************************************************
Реализация простого класса TIMER
	start - Запуск
   stop  - Стоп
   reset - Переинициализировать
   InSec - Показать значения в секундах
   InTCK - В "тиках"
   <<    - Вывод зн-я
******************************************************************/
#ifndef TIMER_H
#define TIMER_H

#include <ostream>
#include <ctime>

namespace SS { namespace Lib {

class TTimer
{
	std::clock_t begin, end;

 public:
   TTimer(){begin = std::clock(); end = 0;}

   //Запустить остановить таймер
   void Start(){ begin = std::clock(); }
   void Stop(){ end = std::clock(); }

   //Продолжить отсчёт
   void Resume(){ begin = std::clock() - (end - begin); }

   //Сбросить таймер (после этого можно пользоваться Resume, вместо Start)
   void Reset(){ begin = end = 0; }

   operator std::clock_t() const{ return end - begin; }

   //Разница в секундах
   double InSec() const{ return (double)(end - begin) / CLK_TCK; }

   //Разница в единицах времени
   double InTCK() const{ return (double)(end - begin); }

   //Минимальный интервад времени который может опредилить таймер
   double MinInterv() const{ return (double) 1.0 / CLK_TCK; }
};


inline std::ostream &operator<<( std::ostream &stream, const TTimer &Ob )
{
	stream <<  Ob.InSec();
   return stream;
}

}} //namespace SS::Lib 
#endif

