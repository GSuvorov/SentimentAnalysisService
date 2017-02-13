#include <shlwapi.h>
#include <ctype.h>
#include "stdafx.h"
#include "Entity.h"

using namespace SS::Interface::Core;
using namespace SS::Interface::Core::MainAnalyse;
using namespace SS::Interface::Core::Dictionary;
using namespace SS::Interface::Core::BlackBox;

wstring ClearBadCharacters(wstring str)
{
	wstring res = str;
	/*for(int i = 0; i < str.size(); ++i)
	{
		if ((str[i] != L'<') && (str[i] != L'>') && (str[i] != L'&'))
			res += str[i];
	}*/

	int nLastSpaceIndex;
	for(nLastSpaceIndex = res.size() - 1; nLastSpaceIndex >= 0; --nLastSpaceIndex)
		if (res[nLastSpaceIndex] != L' ')
			break;
	++nLastSpaceIndex;

	if (nLastSpaceIndex < res.size())
		res.erase(nLastSpaceIndex, res.size() - nLastSpaceIndex);
		
	return res;
}

// преобразует число прописью в число
__int64 GetNumberByString(wstring strNum)
{	
	// предполагается что все слова прописью приходят в нормализованном виде (т.е. именительный падеж ед. число)	
	map<wstring, __int64> numbers;
	if (strNum[0]==' ') strNum.erase(strNum.begin());
	if (strNum[strNum.size()-1]==' ') strNum.erase(--(strNum.end()));
	numbers.insert(map<wstring, __int64>::value_type(L"один", 1));
	numbers.insert(map<wstring, __int64>::value_type(L"одна", 1));
	numbers.insert(map<wstring, __int64>::value_type(L"первый", 1));
	numbers.insert(map<wstring, __int64>::value_type(L"два", 2));
	numbers.insert(map<wstring, __int64>::value_type(L"две",2));
	numbers.insert(map<wstring, __int64>::value_type(L"второй", 2));
	numbers.insert(map<wstring, __int64>::value_type(L"три",3));
	numbers.insert(map<wstring, __int64>::value_type(L"третий", 3));
	numbers.insert(map<wstring, __int64>::value_type(L"четыре",4));
	numbers.insert(map<wstring, __int64>::value_type(L"четвертый", 1));	
	numbers.insert(map<wstring, __int64>::value_type(L"пять",5));
	numbers.insert(map<wstring, __int64>::value_type(L"пятый", 5));
	numbers.insert(map<wstring, __int64>::value_type(L"шесть",6));
	numbers.insert(map<wstring, __int64>::value_type(L"шестой", 6));
	numbers.insert(map<wstring, __int64>::value_type(L"семь",7));
	numbers.insert(map<wstring, __int64>::value_type(L"седьмой", 7));
	numbers.insert(map<wstring, __int64>::value_type(L"восемь",8));
	numbers.insert(map<wstring, __int64>::value_type(L"восьмой", 8));
	numbers.insert(map<wstring, __int64>::value_type(L"девять",9));
	numbers.insert(map<wstring, __int64>::value_type(L"девятый", 9));
	numbers.insert(map<wstring, __int64>::value_type(L"ноль",0));
	numbers.insert(map<wstring, __int64>::value_type(L"нулеыой", 0));
	numbers.insert(map<wstring, __int64>::value_type(L"десять",10));
	numbers.insert(map<wstring, __int64>::value_type(L"десятый", 10));
	numbers.insert(map<wstring, __int64>::value_type(L"одинадцать",11));
	numbers.insert(map<wstring, __int64>::value_type(L"одинадцатый", 11));
	numbers.insert(map<wstring, __int64>::value_type(L"двенадцать",12));
	numbers.insert(map<wstring, __int64>::value_type(L"двенадцатый", 12));		
	numbers.insert(map<wstring, __int64>::value_type(L"тринадцать",13));
	numbers.insert(map<wstring, __int64>::value_type(L"тринадцатый", 13));
	numbers.insert(map<wstring, __int64>::value_type(L"четырнадцать",14));
	numbers.insert(map<wstring, __int64>::value_type(L"четырнадцатый", 14));
	numbers.insert(map<wstring, __int64>::value_type(L"пятнадцать",15));
	numbers.insert(map<wstring, __int64>::value_type(L"пятнадцатый", 15));
	numbers.insert(map<wstring, __int64>::value_type(L"шестнадцать",16));
	numbers.insert(map<wstring, __int64>::value_type(L"шестнадцатый", 16));
	numbers.insert(map<wstring, __int64>::value_type(L"семнадцать",17));
	numbers.insert(map<wstring, __int64>::value_type(L"семнадцатый", 17));
	numbers.insert(map<wstring, __int64>::value_type(L"восемнадцать",18));
	numbers.insert(map<wstring, __int64>::value_type(L"восемнадцатый",18));
	numbers.insert(map<wstring, __int64>::value_type(L"девятнадцать",19));
	numbers.insert(map<wstring, __int64>::value_type(L"девятнадцатый",19));
	numbers.insert(map<wstring, __int64>::value_type(L"двадцать",20));
	numbers.insert(map<wstring, __int64>::value_type(L"двадцатый",20));
	numbers.insert(map<wstring, __int64>::value_type(L"тридцать",30));
	numbers.insert(map<wstring, __int64>::value_type(L"тридцатый",30));
	numbers.insert(map<wstring, __int64>::value_type(L"сорок",40));
	numbers.insert(map<wstring, __int64>::value_type(L"сороковой",40));
	numbers.insert(map<wstring, __int64>::value_type(L"пятьдесят",50));
	numbers.insert(map<wstring, __int64>::value_type(L"пятидесятый",50));
	numbers.insert(map<wstring, __int64>::value_type(L"шестьдесят",60));
	numbers.insert(map<wstring, __int64>::value_type(L"шестидесятый",60));
	numbers.insert(map<wstring, __int64>::value_type(L"семьдесят",70));
	numbers.insert(map<wstring, __int64>::value_type(L"семидесятый",70));
	numbers.insert(map<wstring, __int64>::value_type(L"восемьдесят",80));
	numbers.insert(map<wstring, __int64>::value_type(L"восьмидесятый",80));
	numbers.insert(map<wstring, __int64>::value_type(L"девяносто",90));
	numbers.insert(map<wstring, __int64>::value_type(L"девяностый",90));
	numbers.insert(map<wstring, __int64>::value_type(L"сто",100));
	numbers.insert(map<wstring, __int64>::value_type(L"сотый",100));
	numbers.insert(map<wstring, __int64>::value_type(L"двести",200));
	numbers.insert(map<wstring, __int64>::value_type(L"двухсотый",200));
	numbers.insert(map<wstring, __int64>::value_type(L"триста",300));
	numbers.insert(map<wstring, __int64>::value_type(L"трехсотый",300));
	numbers.insert(map<wstring, __int64>::value_type(L"четыреста",400));
	numbers.insert(map<wstring, __int64>::value_type(L"четырехсотый",400));
	numbers.insert(map<wstring, __int64>::value_type(L"пятьтот",500));
	numbers.insert(map<wstring, __int64>::value_type(L"пятисотый",500));
	numbers.insert(map<wstring, __int64>::value_type(L"шестьсот",600));
	numbers.insert(map<wstring, __int64>::value_type(L"шестисотый",600));
	numbers.insert(map<wstring, __int64>::value_type(L"семьсот",700));
	numbers.insert(map<wstring, __int64>::value_type(L"семисотый",700));
	numbers.insert(map<wstring, __int64>::value_type(L"восемьсот",800));
	numbers.insert(map<wstring, __int64>::value_type(L"восьмисотый",800));
	numbers.insert(map<wstring, __int64>::value_type(L"девятьсот",900));
	numbers.insert(map<wstring, __int64>::value_type(L"девятисотый",900));

	map<wstring, __int64> modifiers;
	modifiers.insert(map<wstring, __int64>::value_type(L"тысяча",1000));
	modifiers.insert(map<wstring, __int64>::value_type(L"тысячный",1000));
	modifiers.insert(map<wstring, __int64>::value_type(L"миллион",1000000));
	modifiers.insert(map<wstring, __int64>::value_type(L"миллионнный",1000000));
	modifiers.insert(map<wstring, __int64>::value_type(L"миллиард",1000000000));
	modifiers.insert(map<wstring, __int64>::value_type(L"миллиардный",1000000000));
	modifiers.insert(map<wstring, __int64>::value_type(L"биллион",1000000000));
	modifiers.insert(map<wstring, __int64>::value_type(L"биллионный",1000000000));
	modifiers.insert(map<wstring, __int64>::value_type(L"триллион",1000000000000));
	modifiers.insert(map<wstring, __int64>::value_type(L"триллионный",1000000000000));

	wstring str = L"";
	__int64 resNum = 0;
	__int64 modifierNumber = 0;
	for(int i=strNum.size()-1; i>=0; i--)
	{
		if (strNum[i]==' ' || i==0)
		{
			if (i==0) str.insert(str.begin(), strNum[i]);
			map<wstring,__int64>::iterator it = numbers.find(str);
			if (it!=numbers.end()) // нашли среди чисел
			{
				__int64 num = it->second;
				if (modifierNumber==0) { resNum += num; }
				else { resNum += num*modifierNumber; }	
				if (i==0) modifierNumber = 0;
			}else // не нашли среди чисел и пробуем найти среди модификаторов				
			{
				modifierNumber = 0;
				it = modifiers.find(str);
				if (it!=modifiers.end())
				{
					modifierNumber = it->second;
				}else break; // неизвестно что
			}
			str.clear();
		}else
			str.insert(str.begin(), strNum[i]);
	}
	if (modifierNumber!=0)
	{
		// это значит перед самым первым модфикаторов в твроке не было числа пропись.
		// Например "записано не одна тысяча двести", а просто "тысяча двести"
		resNum += modifierNumber;

	}

	return  resNum;
}


int GetDayOfWeek(CDate basedate)
{
	CDate baseMonday; //  второй понедельник 2000 года  - 10 января. На него можно ориетнировтаься в расчетах
	baseMonday.nDay = 10;
	baseMonday.nMonth = 1;
	baseMonday.nYear = 2000;
	if (baseMonday<basedate)
	{
		while(baseMonday<basedate)
		{
			basedate.Decrement(7);
		}
		int razn = baseMonday.nDay-basedate.nDay;
		if (razn==0) return 1; // день понедельник
		return 8-(baseMonday.nDay-basedate.nDay);
	}else
	{
		while(baseMonday>basedate)
		{
			basedate.Increment(7);
		}
		int razn = basedate.nDay - baseMonday.nDay;
		if (razn==0) return 1; // день понедельник
		return (basedate.nDay - baseMonday.nDay);
	}


}

bool IsLeapYear(int year)
{
	if (year%4==0 && year%100!=0) return true;
	if (year%400==0) return true;
	return false;
}

int GetLastDayForMonth(CDate& date)
{
	switch(date.nMonth)
	{
		case 1: return 31;
		case 2: if (IsLeapYear(date.nYear))
					return 29;
				else
					return 28;
		case 3: return 31;
		case 4: return 30;
		case 5: return 31;
		case 6: return 30;
		case 7: return 31;
		case 8: return 31;
		case 9: return 30;
		case 10: return 31;
		case 11: return 30;
		case 12: return 31;
	}
	return 31;
}

bool Season(wstring strMod, CDateInterval& cdi, int year)
{
	if (strMod==L"зимой" || strMod==L"зима" || strMod==L"зимою")
	{
		cdi.begin.nYear = cdi.end.nYear = year;
		cdi.begin.nMonth = 1;
		cdi.begin.nDay = 1;
		cdi.end.nMonth = 2;
		cdi.end.nDay = GetLastDayForMonth(cdi.end);
	}
	else if (strMod==L"весной" || strMod==L"весною" || strMod==L"весна")
	{
		cdi.begin.nYear = cdi.end.nYear =year;
		cdi.begin.nMonth = 3;
		cdi.begin.nDay = 1;
		cdi.end.nMonth = 5;
		cdi.end.nDay = GetLastDayForMonth(cdi.end);
	}
	else if (strMod==L"летом" || strMod==L"лето")
	{
		cdi.begin.nYear = cdi.end.nYear = year;
		cdi.begin.nMonth = 6;
		cdi.begin.nDay = 1;
		cdi.end.nMonth = 8;
		cdi.end.nDay = GetLastDayForMonth(cdi.end);
		cdi.end.nDay = GetLastDayForMonth(cdi.end);
	}
	else if (strMod==L"осенью" || strMod==L"осень")
	{
		cdi.begin.nYear = cdi.end.nYear = year;
		cdi.begin.nMonth = 9;
		cdi.begin.nDay = 1;
		cdi.end.nMonth = 11;
		cdi.end.nDay = GetLastDayForMonth(cdi.end);
	}
	else
		return false;

	return true;
}

int GetNumberForMonth(wstring month)
{  
	if (month == L"январь" || month == L"january" || month == L"jan")
		return 1;
	if (month == L"февраль" || month == L"february" || month ==  L"feb")
		return 2;
	if (month == L"март" || month == L"march" ||month ==  L"mar")
		return 3;
	if (month == L"апрель" || month == L"april" || month == L"apr")
		return 4;
	if (month == L"май" || month == L"may")
		return 5;
	if (month == L"июнь" || month == L"june" || month == L"jun")
		return 6;
	if (month == L"июль" || month == L"july" || month == L"jul")
		return 7;
	if (month == L"август" || month == L"august" || month == L"aug")
		return 8;
	if (month == L"сентябрь" || month == L"september" || month == L"sept" || month == L"sep")
		return 9;
	if (month == L"октябрь" || month == L"october" || month == L"oct")
		return 10;
	if (month == L"ноябрь" || month == L"november" || month == L"nov")
		return 11;
	if (month == L"декабрь" || month == L"december" || month == L"dec")
		return 12;
	return -1;
}

int GetDayOfWeekByName(wstring str)
{
	if (str==L"понедельник" || str==L"monday") return 1;
	if (str==L"вторник" || str==L"tuesday") return 2;
	if (str==L"среда" || str==L"wednesday") return 3;
	if (str==L"четверг" || str==L"thursday") return 4;
	if (str==L"пятница" || str==L"friday") return 5;
	if (str==L"суббота" || str==L"saturday") return 6;
	if (str==L"воскресенье" || str==L"sunday") return 7;
	return -1;
}

bool IsPreviousWord(wstring strMod)
{
	if (strMod==L"прошлый" || strMod==L"последний" ||
		strMod==L"предыдущий" || strMod==L"прошедший" || 
		strMod==L"минувший" || strMod==L"ушедший" || strMod==L"вчерашний" ||
		strMod==L"last") 
	{
		return true;
	}
	return false;
}

bool IsNextWord(wstring strMod)
{
	if (strMod==L"будущий" || strMod==L"грядущий" ||
		strMod==L"наступающий" || strMod==L"следующий" || strMod==L"завтрашний" || strMod==L"завтра" ||
		strMod==L"next" ) 
	{
		return true;
	}
	return false;
}

bool IsCurrentWord(wstring strMod, bool isVoid)
{
	if ((strMod.size()==0 && isVoid) || strMod==L"этот" || strMod==L"сегодняший"  ||
		strMod==L"текущий"  || strMod==L"нынешний"  ||
		strMod==L"сей" ||  strMod==L"уходящий" ||
		strMod==L"this" || strMod==L"present"  || strMod==L"current")
	{
		return true;
	}
	return false;
}

bool IsLastWord(wstring strMod)
{
	if (strMod == L"последний")
		return true;
	return false;
}



// начало конец середина
void GetByBeginEndModificator(CDateInterval& dateInt, wstring strMod, wstring strDopMod, __int64 intMod, CDate baseDate, CDateInterval& inputDI)
{
	if (inputDI.begin.nYear!=0)
	{
		int length = inputDI.GetLength();
		if (strMod==L"начало")
		{
			dateInt.begin = inputDI.begin;
			dateInt.end = dateInt.begin;
			dateInt.end.Increment(length/3);
		}
		else if (strMod==L"середина")
		{
			dateInt.begin = inputDI.begin;
			dateInt.begin.Increment(length/3);
			dateInt.end = dateInt.begin;
			dateInt.end.Increment(length/3);
		}
		else if (strMod==L"конец" || strMod==L"окончание")
		{
			dateInt.begin = inputDI.end;
			dateInt.begin.Decrement(length/3);
			dateInt.end = inputDI.end;
		}
		else if (strMod==L"половина" || strMod==L"половин" && (strDopMod.size()>0 || intMod>0))
		{						
			if (strDopMod==L"первый" || intMod==1 || strDopMod.size()==0)
			{
				if (inputDI.begin.nMonth==1 && inputDI.end.nMonth==12)
				{
					dateInt.begin = inputDI.begin;
					dateInt.end.nMonth = 6;
					dateInt.end.nYear = inputDI.end.nYear;
					dateInt.end.nDay = GetLastDayForMonth(dateInt.end);								
				}
				else
				{
					dateInt.begin = inputDI.begin;
					dateInt.end = dateInt.begin;
					dateInt.end.Increment(length/2);
				}
			}
			else if (strDopMod==L"второй" || intMod==2)
			{
				if (inputDI.begin.nMonth==1 && inputDI.end.nMonth==12)
				{
					dateInt.begin.nDay = 1;
					dateInt.begin.nMonth = 7;
					dateInt.begin.nYear = inputDI.begin.nYear;
					dateInt.end = inputDI.end;													
				}
				else
				{
					dateInt.begin = inputDI.end;
					dateInt.begin.Decrement(length/2);
					dateInt.end = inputDI.end;
				}
			}
		}
	}
	else
	{
		if (strMod==L"вчера" || strMod==L"yesterday")
		{
			dateInt.begin = baseDate;
			dateInt.begin.Decrement(1);
			dateInt.end = dateInt.begin;
		}
		else if (strMod==L"позавчера")
		{
			dateInt.begin = baseDate;
			dateInt.begin.Decrement(2);
			dateInt.end = dateInt.begin;
		}
		else if (strMod==L"намедни" || strMod==L"накануне")
		{
			dateInt.begin = baseDate;
			dateInt.begin.Decrement(3);
			dateInt.end = baseDate;
			dateInt.begin.Decrement(1);
		}
		else if (strMod==L"сегодня" || strMod==L"today")
		{
			dateInt.begin =  baseDate;
			dateInt.end = baseDate;
		}
		else if (strMod==L"завтра" || strMod==L"tomorrow")
		{
			dateInt.begin = baseDate;
			dateInt.begin.Increment(1);
			dateInt.end = dateInt.begin;
		}
		else if (strMod==L"послезавтра")
		{
			dateInt.begin = baseDate;
			dateInt.begin.Increment(2);
			dateInt.end = dateInt.begin;
		}
	}
}


void GetByYearModificator(CDateInterval& dateInt, wstring strMod, CDate baseDate, int mode = 0)
{
	if (IsPreviousWord(strMod))
	{
		dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear-1;
		dateInt.begin.nMonth = 1;
		dateInt.begin.nDay = 1;
		dateInt.end.nMonth = 12;
		dateInt.end.nDay = 31;
	}
	else if (IsNextWord(strMod))
	{
		dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear+1;
		dateInt.begin.nMonth = 1;
		dateInt.begin.nDay = 1;
		dateInt.end.nMonth = 12;
		dateInt.end.nDay = 31;
	}
	else if (IsCurrentWord(strMod, true))
	{
		dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear;
		dateInt.begin.nMonth = 1;
		dateInt.begin.nDay = 1;
		if (mode == 0)
		{
			dateInt.end.nMonth = 12;
			dateInt.end.nDay = 31;

		}
		else if (mode==1) //  до базовой даты
		{
			dateInt.end.nMonth = baseDate.nMonth;
			dateInt.end.nDay = baseDate.nDay; 

		}
		else if (mode==2) // после базовой
		{
			dateInt.begin.nMonth = baseDate.nMonth;
			dateInt.begin.nDay = baseDate.nDay; 
			dateInt.end.nMonth = baseDate.nMonth;
			dateInt.end.nDay = baseDate.nDay; 
		}
	}
}

bool IsYear(wstring str)
{
	if (str==L"год" || str==L"year")
		return true;
	return false;
}

bool IsMonth(wstring str)
{
	if (str==L"месяц" || str==L"month")
		return true;
	return false;
}

int GetQuarterForDate(CDate& date)
{
	return date.nMonth/3;
}

int GetHalfYearForDate(CDate& date)
{
	return date.nMonth/6;
}


void GetByQuarterModificator(CDateInterval& dateInt, wstring strMod, __int64 intMod, CDate baseDate, CDateInterval& inputDI, int mode = 0)
{
	if (inputDI.begin.nYear!=0)
	{
		if (IsLastWord(strMod))
		{
			dateInt.begin.nYear = dateInt.end.nYear = inputDI.begin.nYear;
			dateInt.begin.nMonth = 10;	
			dateInt.begin.nDay = 1;	
			dateInt.end.nMonth = 12;
			dateInt.end.nDay = 31;
		}
		else if (intMod>0)
		{
			dateInt.begin.nYear = dateInt.end.nYear = inputDI.begin.nYear;
			dateInt.begin.nMonth = 3*(intMod-1)+1;
			dateInt.begin.nDay = 1;		
			dateInt.end.nMonth = dateInt.begin.nMonth+2;
			dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
		}		
	}
	else if (IsPreviousWord(strMod)) 
	{
		int quarter = GetQuarterForDate(baseDate)-1;
		if (quarter < 0) // текущий первый, значит прошлы последний для прошлого года
		{
			dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear-1;
			dateInt.begin.nMonth = 10;	
			dateInt.begin.nDay = 1;	
			dateInt.end.nMonth = 12;
			dateInt.end.nDay = 31;
		}
		else
		{
			dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear;
			dateInt.begin.nMonth = 3 * (quarter - 1) + 1;	
			dateInt.begin.nDay = 1;	
			dateInt.end.nMonth = dateInt.begin.nMonth + 2;
			dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
		}
	}
	else if (IsNextWord(strMod)) 
	{
		int quarter = GetQuarterForDate(baseDate) + 1;
		if (quarter > 4) 
		{
			dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear + 1;
			dateInt.begin.nMonth = 1;	
			dateInt.begin.nDay = 1;	
			dateInt.end.nMonth = 3;
			dateInt.end.nDay = 31;
		}else
		{
			dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear;
			dateInt.begin.nMonth = 3 * (quarter - 1) + 1;	
			dateInt.begin.nDay = 1;	
			dateInt.end.nMonth = dateInt.begin.nMonth + 2;
			dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
		}
	}
	else if (IsCurrentWord(strMod, true))
	{
		int quarter = GetQuarterForDate(baseDate);
		dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear;
		dateInt.begin.nMonth = 3*(quarter-1)+1;	
		dateInt.begin.nDay = 1;	
		dateInt.end.nMonth = dateInt.begin.nMonth+2;
		dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
	}
	else
	{		
		if (intMod > 0)
		{
			if (inputDI.begin.nYear != 0)
				dateInt.begin.nYear = dateInt.end.nYear = inputDI.begin.nYear;
			else
				dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear;
			dateInt.begin.nMonth = 3*(intMod-1)+1;
			dateInt.begin.nDay = 1;		
			dateInt.end.nMonth = dateInt.begin.nMonth+2;
			dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
		}
		else
		{
			int quarter = GetQuarterForDate(baseDate);
			dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear;
			dateInt.begin.nMonth = 3*(quarter-1)+1;	
			dateInt.begin.nDay = 1;	
			dateInt.end.nMonth = dateInt.begin.nMonth+2;
			dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
		}
	}
}

void GetByHalfYearModificator(CDateInterval& dateInt, wstring strMod, __int64 intMod, CDate baseDate, CDateInterval& inputDI, int mode = 0)
{
	if (inputDI.begin.nYear!=0)
	{
		if (IsLastWord(strMod))
		{
			dateInt.begin.nYear = dateInt.end.nYear = inputDI.begin.nYear;
			dateInt.begin.nMonth = 7;	
			dateInt.begin.nDay = 1;	
			dateInt.end.nMonth = 12;
			dateInt.end.nDay = 31;
		}
		else if (intMod>0)
		{
			dateInt.begin.nYear = dateInt.end.nYear = inputDI.begin.nYear;
			dateInt.begin.nMonth = 6*(intMod-1)+1;
			dateInt.begin.nDay = 1;		
			dateInt.end.nMonth = dateInt.begin.nMonth+5;
			dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
		}		
	}
	else if (IsPreviousWord(strMod)) 
	{
		int half = GetHalfYearForDate(baseDate)-1;
		if (half<0) 
		{
			dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear-1;
			dateInt.begin.nMonth = 7;	
			dateInt.begin.nDay = 1;	
			dateInt.end.nMonth = 12;
			dateInt.end.nDay = 31;
		}
		else
		{
			dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear;
			dateInt.begin.nMonth = 6*(half-1)+1;	
			dateInt.begin.nDay = 1;	
			dateInt.end.nMonth = dateInt.begin.nMonth+5;
			dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
		}
	}
	else if (IsNextWord(strMod)) 
	{
		int half = GetHalfYearForDate(baseDate)+1;
		if (half>2) // прошлый год
		{
			dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear+1;
			dateInt.begin.nMonth = 1;	
			dateInt.begin.nDay = 1;	
			dateInt.end.nMonth = 6;
			dateInt.end.nDay = 30;
		}
		else
		{
			dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear;
			dateInt.begin.nMonth = 6*(half-1)+1;	
			dateInt.begin.nDay = 1;	
			dateInt.end.nMonth = dateInt.begin.nMonth+5;
			dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
		}
	}
	else if (IsCurrentWord(strMod,true))
	{
		int half = GetHalfYearForDate(baseDate);
		dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear;
		dateInt.begin.nMonth = 6*(half-1)+1;	
		dateInt.begin.nDay = 1;	
		dateInt.end.nMonth = dateInt.begin.nMonth+5;
		dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
	}
	else
	{		
		if (intMod>0)
		{
			if (inputDI.begin.nYear!=0)
				dateInt.begin.nYear = dateInt.end.nYear = inputDI.begin.nYear;
			else
				dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear;
			dateInt.begin.nMonth = 6*(intMod-1)+1;
			dateInt.begin.nDay = 1;		
			dateInt.end.nMonth = dateInt.begin.nMonth+5;
			dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
		}
		else
		{
			int quarter = GetHalfYearForDate(baseDate);
			dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear;
			dateInt.begin.nMonth = 6*(quarter-1)+1;	
			dateInt.begin.nDay = 1;	
			dateInt.end.nMonth = dateInt.begin.nMonth+5;
			dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
		}
	}
}

void GetByDecadeModificator(CDateInterval& dateInt, wstring strMod, __int64 intMod, CDate baseDate, CDateInterval& inputDI, int mode = 0)
{
	if (inputDI.begin.nYear!=0)
	{
		if (IsLastWord(strMod))
		{
			dateInt.end = inputDI.end;
			dateInt.begin = dateInt.end;
			dateInt.begin.Decrement(9);
		}
		else if (intMod>0)
		{
			dateInt.begin = inputDI.begin;
			dateInt.end = dateInt.begin;
			dateInt.end.Increment(intMod*10-1);			
		}		
	}
	if (IsPreviousWord(strMod)) 
	{
		// получаем номер текущей декады в месяце. Если 
		// это первая - то преиод = последней декаде прошлого месяца
		// иначе - декада данного месяца но на одну меньше
		int dec_nom = baseDate.nDay/10;
		dateInt.begin =  baseDate;
		dateInt.begin.nDay = dec_nom*10+1;
		dateInt.begin.Decrement(10);
		dateInt.end = dateInt.begin;
		dateInt.end.Increment(9);
	}
	if (IsNextWord(strMod)) 
	{
		int dec_nom = baseDate.nDay/10;
		dateInt.begin =  baseDate;
		dateInt.begin.nDay = dec_nom*10+1;
		dateInt.begin.Increment(10);
		dateInt.end = dateInt.begin;
		dateInt.end.Increment(9);
	}
	else if (IsCurrentWord(strMod, true))
	{
		int dec_nom = baseDate.nDay/10;
		dateInt.begin =  baseDate;
		dateInt.begin.nDay = dec_nom*10+1;		
		dateInt.end = dateInt.begin;
		dateInt.end.Increment(9);
	}		
}

void GetByMonthModificator(CDateInterval& dateInt, wstring strMod, __int64 intMod, CDate baseDate, CDateInterval& inputDI, int mode = 0)
{
	int nYear = baseDate.nYear;	
	if (inputDI.begin.nYear!=0)
	{				
		if (IsLastWord(strMod))
		{
			dateInt.begin.nYear = dateInt.end.nYear = inputDI.begin.nYear;
			dateInt.begin.nMonth = dateInt.end.nMonth = 12;		  
			dateInt.begin.nDay = 1;		  
			dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
		}
		else if (intMod>0)
		{
			dateInt.begin.nYear = dateInt.end.nYear = inputDI.begin.nYear;
			dateInt.begin.nMonth = dateInt.end.nMonth = intMod;		  
			dateInt.begin.nDay = 1;		  
			dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
		}
	}
	else if (IsPreviousWord(strMod)) 
	{
		if (baseDate.nMonth>1)
		{
			dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear;
			dateInt.begin.nMonth = dateInt.end.nMonth = baseDate.nMonth-1;		  
			dateInt.begin.nDay = 1;		  
			dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
		}
		else
		{
			dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear-1;
			dateInt.begin.nMonth = dateInt.end.nMonth = 12;		  
			dateInt.begin.nDay = 1;		  
			dateInt.end.nDay = 31;
		}		 
	}
	else if (IsNextWord(strMod)) 
	{
		if (baseDate.nMonth<12)
		{
			dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear;
			dateInt.begin.nMonth = dateInt.end.nMonth = baseDate.nMonth+1;		  
			dateInt.begin.nDay = 1;		  
			dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
		}
		else
		{
			dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear+1;
			dateInt.begin.nMonth = dateInt.end.nMonth = 1;		  
			dateInt.begin.nDay = 1;		  
			dateInt.end.nDay = 31;
		}		 		
	}
	else if (IsCurrentWord(strMod, true))
	{
		dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear;
		dateInt.begin.nMonth = dateInt.end.nMonth = baseDate.nMonth;		  
		dateInt.begin.nDay = 1;	
		if (mode == 0)
		{
			dateInt.end.nDay = GetLastDayForMonth(dateInt.end);

		}
		else if (mode==1) //  до базовой даты
			dateInt.end.nDay = baseDate.nDay;
		else if (mode==2) // после базовой
		{			
			dateInt.begin.nDay = baseDate.nDay; 			
			dateInt.end.nDay =GetLastDayForMonth(dateInt.end);
		}
	}
	else // просто запись - первый месяц и т.д. имеем вывиду что текущего года
	{ 
		dateInt.begin.nYear = dateInt.end.nYear = baseDate.nYear;
		if (intMod!=0)
			dateInt.begin.nMonth = dateInt.end.nMonth =  intMod;
		else
			dateInt.begin.nMonth = dateInt.end.nMonth =  baseDate.nMonth;
		dateInt.begin.nDay = 1;	
		dateInt.end.nDay =GetLastDayForMonth(dateInt.end);
	}
}

void GetByMonthNameModificator(CDateInterval& dateInt, wstring strName, wstring strMod, CDate baseDate,CDateInterval& inputDI)
{	
	int nMonth = GetNumberForMonth(strName);
	int nYear = baseDate.nYear;	
	if (inputDI.begin.nYear != 0)
	{
		dateInt.begin.nYear = dateInt.end.nYear =  inputDI.begin.nYear;
		dateInt.begin.nMonth = dateInt.end.nMonth = nMonth;
		dateInt.begin.nDay = 1;
		dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
	}
	else if (IsPreviousWord(strMod)) 
	{		
		if (nMonth>baseDate.nMonth){ // тогда надо звять этот месяц из прошлого года, а иначе говорится об этом годе и уже прошедшем месяце
			dateInt.begin.nYear = dateInt.end.nYear =  baseDate.nYear-1;
			dateInt.begin.nMonth = dateInt.end.nMonth = nMonth;
			dateInt.begin.nDay = 1;
			dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
		}
		else
		{
			dateInt.begin.nYear = dateInt.end.nYear =  baseDate.nYear;
			dateInt.begin.nMonth = dateInt.end.nMonth = nMonth;
			dateInt.begin.nDay = 1;
			dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
		}
	}
	else if (IsNextWord(strMod)) 
	{
		dateInt.begin.nYear = dateInt.end.nYear =  baseDate.nYear;
		dateInt.begin.nMonth = dateInt.end.nMonth = nMonth;
		dateInt.begin.nDay = 1;
		dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
	}
	else
		//if (strMod.size()==0 || strMod==L"этот"  || strMod==L"сей" || 
		//	strMod==L"текущий" || strMod==L"нынешний" || strMod==L"уходящий" ||
		//	strMod==L"this" || strMod==L"present" || strMod==L"current")
	{
		dateInt.begin.nYear = dateInt.end.nYear =  baseDate.nYear;
		dateInt.begin.nMonth = dateInt.end.nMonth = nMonth;
		dateInt.begin.nDay = 1;
		dateInt.end.nDay = GetLastDayForMonth(dateInt.end);
	}
}


void GetByWeekModificator(CDateInterval& dateInt, wstring strMod, wstring dayMod, __int64 intMod,
						  CDate baseDate, CDateInterval& inputDI)
{	
	if (inputDI.begin.nYear!=0)
	{
		// известен интервал. Надо понять какой номер недели в нем. Далее отсчитать столько отрезков по 7 дней в этом интервале
		if (IsLastWord(strMod))
		{
			// 1) берем последний день интервала и определяем его день недели
			if (dayMod.size()==0)
			{
				int dow = GetDayOfWeek(inputDI.end);
				dateInt.end = inputDI.end;
				dateInt.begin = inputDI.end;
				dateInt.begin.Decrement(dow-1);
			}else
			{
				if (inputDI.end>=inputDI.begin)
				{
					int findinDOW = GetDayOfWeekByName(dayMod);					
					dateInt.begin = inputDI.end;
					while(dateInt.begin>=inputDI.begin)
					{
						int dow = GetDayOfWeek(dateInt.begin);
						if (dow==findinDOW) break;					
						dateInt.begin.Decrement(1);
					}
				}
				dateInt.end = dateInt.begin;
			}
		}else			
		{
			if (dayMod.size()==0)
			{
				if (intMod!=0)
				{
					dateInt.begin = inputDI.begin;
					dateInt.begin.Increment(7*(intMod-1));		
					int dow = GetDayOfWeek(inputDI.begin);
					dateInt.begin.Decrement(dow-1);
					dateInt.end = dateInt.begin;
					dateInt.end.Increment(6);
					if (dateInt.end>inputDI.end) dateInt.end = inputDI.end;
				}
			}else
			{
				if (intMod==0) intMod=1;
				int count =0;
				int findinDOW = GetDayOfWeekByName(dayMod);
				// надо найти данном интервале соотв. по счету день недели (например второй четверг прошлого месяца)
				dateInt.begin = inputDI.begin;
				if (inputDI.end>=inputDI.begin)
				{
					while(dateInt.begin<=inputDI.end)
					{
						int dow = GetDayOfWeek(dateInt.begin);
						if (dow==findinDOW) count++;
						if (count==intMod) break; 
						dateInt.begin.Increment(1);
					}
				}
				dateInt.end = dateInt.begin;
			}

		}		

	}else
		if (IsPreviousWord(strMod)) 
		{
			int dow = GetDayOfWeek(baseDate); // получили базовый день.
			int sub = dow+6;
			dateInt.begin  = baseDate;
			dateInt.begin.Decrement(sub);	
			dateInt.end = dateInt.begin;
			dateInt.end.Increment(6);

			if (dayMod.size()!=0)
			{ 
				// интервал конкретный день на неделе
				dateInt.begin.Increment(GetDayOfWeekByName(dayMod)-1);
				dateInt.end = dateInt.begin;
			}		
		}else
			if (IsNextWord(strMod)) 
			{
				int dow = GetDayOfWeek(baseDate); // получили базовый день.
				int add = 8-dow;
				dateInt.begin  = baseDate;
				dateInt.begin.Increment(add);	
				dateInt.end = dateInt.begin;
				dateInt.end.Increment(6);


				if (dayMod.size()!=0)
				{ 
					// интервал конкретный день на неделе
					dateInt.begin.Increment(GetDayOfWeekByName(dayMod)-1);
					dateInt.end = dateInt.begin;
				}		
			}else
				if (IsCurrentWord(strMod, true))
				{
					int dow = GetDayOfWeek(baseDate);
					dateInt.begin  = baseDate;
					dateInt.begin.Decrement(dow-1);
					dateInt.end = dateInt.begin;
					dateInt.end.Increment(6);
					if (dayMod.size()!=0)
					{ 
						// интервал конкретный день на неделе
						dateInt.begin.Increment(GetDayOfWeekByName(dayMod)-1);
						dateInt.end = dateInt.begin;
					}		
				}		
}

void GetByDayModificator(CDateInterval& dateInt, wstring strMod, __int64 intMod, CDate baseDate, CDateInterval& inputDI)
{
	if (inputDI.begin.nYear!=0)
	{		
		if (IsLastWord(strMod))
		{
			dateInt.begin = inputDI.end;
			dateInt.end = inputDI.end;
		}else if (intMod!=0)
		{
			dateInt.begin = inputDI.begin;
			dateInt.begin.Increment(intMod-1);
			dateInt.end = dateInt.begin;
		}
	}
	else
		if (IsPreviousWord(strMod)) 
		{
			dateInt.begin = baseDate;
			dateInt.end = baseDate;
			dateInt.begin.Decrement(1);
			dateInt.end.Decrement(1);
		}
		else
			if (IsNextWord(strMod)) 
			{
				dateInt.begin = baseDate;
				dateInt.end = baseDate;
				dateInt.begin.Increment(1);
				dateInt.end.Increment(1);
			}else
				if (IsCurrentWord(strMod, false))
				{
					dateInt.begin = baseDate;
					dateInt.end = baseDate;	
				}
}

CEntity::CEntity(UnitPropertiesOperator* unitOperator, SS::Core::Types::EAnalyseLanguage lang)
{
	m_pUnitOperator = unitOperator;
    nType = eEntityBaseType::Default;
	entType = entDefault;
	isUsed = true;
	extDateRootUnit = NULL;
	m_lang = lang;
	m_strHash = L"";
	isProcessed = false;
	isPseudoPlace = false;
	isAnaphora = true;
	m_pBaseDate = NULL;
	m_strValue = L"";	
	ID = 0;
	m_strProfession = L"";
	m_strLastName = L"";
	m_strFirstName = L"";
	m_strMiddleName = L"";
	m_strPrefix = L"";
	m_strPost = L"";
	m_Name = L"";
	isHaveHomogenousAdj = false;
	rootType = eEntityBaseType::Default;
	begin = 0;
	end = 0;
	rootUnit = NULL;
	nounUnit = NULL;
	m_pSent = NULL;
}

wstring CEntity::GetTypeStr()
{
	switch(entType)
	{
		case entPersonName: return L"PersonName";
		case entPersonInfo: return L"PersonInfo";
		case entPersonNoInfo: return L"PersonNoInfo";
		case entOrgName: return L"OrgName";
		case entOrgNoInfo: return L"OrgNoInfo";
		case entPlaceName: return L"PlaceName";
		case entPlaceNoInfo: return L"PlaceNoInfo";
		case entDate: return L"Date";
		case entUnknownProper: return L"UnknownProper";
		case entNounVerb: return L"NounVerb";
		case entVerb: return GetVerbType();
		case entPronoun: return L"Pronoun";
		case entOrgAbbr: return L"OrgAbbr";
		case entOrgInfo: return L"OrgInfo";
        case entAdverb: return L"Adverb";
        case entPretext: return L"Pretext";
        case entPredicative: return L"Predicative";
        case entNumeric: return L"Numeric";
		case entQuote: return L"Quote";
		case entAdjective: return L"Adjective";
		case entShortAdjective: return L"ShortAdjective";
		case entComparative: return L"Comparative";
		case entSuperlative: return L"Superlative";
		case entPhone: return L"Phone";
		case entEvent: return L"Event";
		case entProduct: return L"Product";
		default: return L"Unknown";
	}
}

void CEntity::SetDate(wstring strBaseDate)
{
	m_pBaseDate = new CDate(strBaseDate);
}

wstring CEntity::GetValue()
{
    return m_strValue;
}

wstring CEntity::GetNormalString()
{
    //if(m_strValue == L"")
        NormalizeValue();
    return m_strValue;
}

wstring CEntity::NormalizeValue()
{
	m_strValue = L"";
	propList.clear();
	if (entType==entDate && m_DateInterval.IsDateClear())
        ProcessValueForDate();
	m_strValue = L"";
	m_strPrefix = L"";
	m_Adjectives.clear();

	TUnitList oldList;
	if (rootUnit->GetReferenceUnit())
	{
		for(TUnitList::iterator it = unitList.begin(); it != unitList.end(); ++it)
			oldList.push_back(*it);
		unitList.clear();
		for(TUnitList::iterator it = unitListRef.begin(); it != unitListRef.end(); ++it)
			unitList.push_back(*it);
	}
	switch(entType)
	{
	    case entDate:
		    m_strValue = GetDateNormalString();
            break;
	    case entOrgName:
	    case entOrgInfo:
	    case entOrgAbbr:
		    if (isPseudoPlace)
		    {
			    m_strValue  = GetRegionNormalString();
			    for(list<CEntity*>::iterator it = lstRootEntities.begin(); it != lstRootEntities.end(); ++it)
				    m_strValue += L"," + (*it)->GetRegionNormalString();
			    m_strHash = m_strValue;
		    }
		    else
		    {
			    m_strValue = GetOrgNormalString();
			    m_strHash = GetHash();
			}
		    break;
	    case entPersonName:
	    case entPersonInfo:
		    m_strValue = GetPersonNormalString();
		    m_strHash = GetHash();
			m_strPost = ClearBadCharacters(m_strPost);
		    break;
	    case entPlaceNoInfo:
		    m_strValue = GetDefaultNormalString();
		    break;
	    case entPlaceName:
			if (rootType == eEntityBaseType::Place)
			{
				m_strValue = GetRegionNormalString();
				for(list<CEntity*>::iterator it = lstRootEntities.begin(); it != lstRootEntities.end(); ++it)
					m_strValue += L"," + (*it)->GetRegionNormalString();
			}
			else
				m_strValue = GetDefaultNormalString();
		    break;
	    case entVerb:
		    m_strValue = GetVerbNormalString();
		    break;
	    case entPronoun:
            m_strValue = GetNormalStringByPartOfSpeech(SS::Core::Features::Values::PartOfSpeechTypes.postPronoun);
		    break;
		case entAdjective:
		case entShortAdjective:
		case entComparative:
		case entSuperlative:
			m_strValue = GetNormalStringByPartOfSpeech(SS::Core::Features::Values::PartOfSpeechTypes.postAdjective);
			break;
		case entNumeric:
			m_strValue = GetNormalStringByPartOfSpeech(SS::Core::Features::Values::PartOfSpeechTypes.postNumeric);
			break;
        case entAdverb:
        case entPretext:
        case entPredicative:
		case entQuote:
            m_strValue = GetUnchangeableNormalString();
            break;
		case entPhone:
			m_strValue = GetPhoneNormalString();
			break;
	    default:
            m_strValue = GetDefaultNormalString();
	}

	if (rootUnit->GetReferenceUnit())
	{
		unitList.clear();
		for(TUnitList::iterator it = oldList.begin(); it!= oldList.end(); ++it)
			unitList.push_back(*it);
		oldList.clear();
	}

	m_strValue = ClearBadCharacters(m_strValue);
	m_strHash = ClearBadCharacters(m_strHash);
	m_strPrefix = ClearBadCharacters(m_strPrefix);

    m_strValue = m_pUnitOperator->QuoteNormalize(m_strValue);
	m_strHash = m_pUnitOperator->QuoteNormalize(m_strHash);
	m_strPrefix = m_pUnitOperator->QuoteNormalize(m_strPrefix);

	if ((entType != entOrgName) && (entType != entEvent) && (entType != entProduct))
	{
		m_pUnitOperator->RemovePossessiveSign(m_strValue);
		m_pUnitOperator->RemovePossessiveSign(m_strPrefix);
	}
	if ((entType == entNumeric) && (m_pUnitOperator->IsRomanNumber(m_strValue) || m_pUnitOperator->IsArabicNumber(m_strValue)))
		propList.clear();
	return  m_strValue;
}

wstring CEntity::GetPhoneNormalString()
{
	wstring result = L"";
	
	TUnitList phoneNumberUnits;
	TUnitList additionalNumberUnits;
	
	TUnitList *pSourceCollection = &phoneNumberUnits;
	bool isNumber = false;
	SS::Core::Features::CMorphoFeature oMorfoSymbol;
    oMorfoSymbol.Undefine();
    oMorfoSymbol.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;
    oMorfoSymbol.m_OfficialType = SS::Core::Features::Values::OfficialType.otSymbol;
	SS::Core::Features::CMorphoFeature oMorfoUnit;
	for(TUnitList::iterator it = unitList.begin(); it != unitList.end(); ++it)
	{
		bool isNumeric = false;
		bool isSymbol = false;
		IUnit *pUnit = *it;
		wstring word = pUnit->GetWord();

		m_pUnitOperator->GetUnitMorfoFeatures(pUnit, oMorfoUnit);
		if (m_pUnitOperator->IsArabicNumber(word))
			isNumeric = true;
		else if(isNumber && !m_pUnitOperator->IsMorpho(pUnit, oMorfoSymbol))
		{
			pSourceCollection = &additionalNumberUnits;
			isNumber = false;
		}

		isNumber = isNumber || isNumeric;
		if (isNumeric)
			pSourceCollection->push_back(pUnit);
	}

	int phoneNumberLength = GetPhoneNumberLength(unitList.front()->GetPatternName());
	bool isUnclosedCityCode = false;
	for (TUnitList::reverse_iterator it = phoneNumberUnits.rbegin(); it != phoneNumberUnits.rend(); ++it)
	{
		IUnit *pUnit = *it;
		wstring word = pUnit->GetWord();
		
		if (phoneNumberLength > 0)
			phoneNumberLength -= word.size();
		else if (isUnclosedCityCode)
		{
			result.insert(0, L" (");
			isUnclosedCityCode = false;
		}
		else
		{
			result.insert(0, L") ");
			isUnclosedCityCode = true;
		}
		result.insert(0, word);
	}
	if (isUnclosedCityCode)
		result.insert(0, L"(");

	if (!additionalNumberUnits.empty())
	{
		result += L" / ";
		for(TUnitList::iterator it = additionalNumberUnits.begin(); it != additionalNumberUnits.end(); ++it)
			result += (*it)->GetWord();
	}
		
	return result;
}

int CEntity::GetPhoneNumberLength(wstring phoneType)
{
	int result = -1;
	if (phoneType == L"FiveDigitsPhone")
		return 5;
	else if (phoneType == L"SixDigitsPhone")
		return 6;
	else if (phoneType == L"SevenDigitsPhone")
		return 7;
	else if (phoneType == L"TenDigitsPhone")
		return 10;
	else if (phoneType == L"ElevenDigitsPhone")
		return 11;
	else
		SS_THROW(L"Неизвестный тип телефона");
	return result;
}

wstring NormalizeEntString(wstring str)
{
	wstring curWord = L"";
	wstring lastWord = L"";
	wstring res = L"";
	for(unsigned int i = 0; i < str.size(); ++i)
	{
		if (str[i] == L' ')
		{
			if (lastWord.size() != 0)
				res += L" ";
			if (lastWord != curWord)
				res += curWord;
			lastWord = curWord;
			curWord.clear();
		}
		else
			curWord += str[i];
	}
	return res;
}

wstring CEntity::GetHash()
{
	// нормализуем хэш. Выкидываем знаки препинания. Кавычки. и слова из сто словаря.
	wstring resStr = L"";
	int num_of_space = 0;
	for(unsigned int i = 0; i < m_strHash.size(); ++i)
	{
		if (iswalnum(m_strHash[i]) || m_strHash[i] == L' ')
		{
			if (m_strHash[i] == L' ')
			{
				if (num_of_space == 0)
					resStr += m_strHash[i];
				++num_of_space;
			}
			else 
			{
				num_of_space = 0;
				resStr += m_strHash[i];
			}
		}
		else num_of_space = 0;
	}
	return resStr;
}

wstring CEntity::GetPersonNormalStringEng()
{
	// для английского - просто суммируем все слова.
	// при построении хэша - все, что после of переносим вперед. of выкидываем. 
	// например, president of America = America president
	wstring strRes = L"";
	m_strHash = L"";
	list<wstring> lstPartsOfHash;
	wstring curStr = L"";
	set<IUnit*> addedUnits;
	SS::Core::Features::CMorphoFeature oMorfoPattern;
	oMorfoPattern.Undefine();
	oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;	
	m_strFirstName = L"";
	m_strLastName = L"";
	m_strMiddleName = L"";

	for(TUnitList::reverse_iterator it = unitList.rbegin(); it!= unitList.rend(); it++)
	{
		IUnit* pUnit = *it;
		wstring str = pUnit->GetWord();
		if (str.empty())
			continue;
		if ((m_pUnitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
			SS::Core::Features::Values::TSPersonType.semprstFirstName) || pUnit->IsInitial()))
		{
			if (!m_strFirstName.empty()) 
			{
				if (m_strLastName.empty())
					m_strLastName = m_strFirstName;
				else 
					m_strMiddleName = m_strFirstName;
			}
			m_strFirstName = str;
		}
		else if (m_strMiddleName.empty() &&
			(m_pUnitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
			SS::Core::Features::Values::TSPersonType.semprstMiddleName) || pUnit->IsInitial()))
		{
			m_strMiddleName = str;
		}
		else if (m_strLastName.empty() &&
			m_pUnitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
			SS::Core::Features::Values::TSPersonType.semprstLastName))
		{
			m_strLastName = str;
		}
		else if (m_pUnitOperator->IsProperUnknown(pUnit))
		{
			if (m_strLastName.empty())
				m_strLastName = str;
			else if (m_strFirstName.empty())
				m_strFirstName = str;
		}
	}

	bool isBegin = true;
	for(TUnitList::iterator it = unitList.begin(); it != unitList.end(); ++it)
	{
		IUnit* pUnit = *it;
		wstring str = pUnit->GetWord();
		if (str.empty() || !m_pUnitOperator->IsVisualChild(pUnit))
			continue;
		if (str==L"s" || str==L"S" || str==L"'")
			continue;
		wstring normalForm = m_pUnitOperator->GetMainFormText1(pUnit, SS::Core::Features::Values::GenderType.Undefined).form;
		if (!m_pUnitOperator->IsMorpho(pUnit, oMorfoPattern) || !isBegin)
		{
			isBegin = false;
			strRes += normalForm + L" ";
		}	
		if (m_strProfession.empty() &&
			(m_pUnitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
			SS::Core::Features::Values::TSPersonType.semprstAgent) ||
			m_pUnitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
			SS::Core::Features::Values::TSPersonType.semprstProfession)))
		{
			m_strProfession = str;
		}
	}

	for(TUnitList::iterator it = unitList.begin(); it != unitList.end(); ++it)
	{
		IUnit* pUnit = *it;
		wstring str = pUnit->GetWord();
		if (str.empty() || !m_pUnitOperator->IsVisualChild(pUnit))
			continue;
		else if (addedUnits.find(pUnit) == addedUnits.end())
		{			
			if (str==L"of" || str==L"Of" || str==L"OF")
			{
				TUnitList allUnits;
				m_pUnitOperator->GetLinearUnits(pUnit, allUnits);
				wstring resStr1 = L"";
				for(TUnitList::iterator it1 = allUnits.begin(); it1 != allUnits.end(); ++it1)
				{
					IUnit* pUnit1 = *it1;
					wstring str1 = pUnit1->GetWord();
					addedUnits.insert(pUnit1);
					if (str1!=L"of" && str1!=L"Of" && str1!=L"OF")
						resStr1 += str1 + L" ";
				}
				m_strHash = resStr1 + m_strHash;
			} 
			else 
			{
				if (m_pUnitOperator->IsMorpho(pUnit, oMorfoPattern))
					continue;
				if (str!=L"s" && str!=L"'" && str!=L"S")
				{
					//strRes += str + L" ";
					m_strHash += str + L" ";
				}
			}
		}
	}
	return strRes;
}


bool CEntity::IsDelOrgPrefix(IUnit* pUnit)
{		
	wstring str = m_pUnitOperator->GetMainFormText1(pUnit,SS::Core::Features::Values::GenderType.Undefined).form;
	wstring strLo = m_pUnitOperator->GetLowerWord(str);
	return CDictionaryPool::IsFindOrgPrefixes(strLo);
}

wstring CEntity::GetOrgNormalString()
{
	list<IUnit*> unitsInQuote;
	wstring resStr = L"";
	wstring resStrHash = L"";
	m_Name = L"";

	SS::Core::Features::CMorphoFeature oMorphoOfficial;
	oMorphoOfficial.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;	
	SS::Core::Features::CMorphoFeature oMorphoNoun;
	oMorphoNoun.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;
	SS::Core::Features::CMorphoFeature oMorphoGenetive;
	oMorphoGenetive.m_CaseType = SS::Core::Features::Values::CaseType.ctGenitive;

	if (m_lang != SS::Core::Types::ealRussian)
	{
		IUnit* pUnit = (*unitList.begin());
		wstring str1 = pUnit->GetWord();
		wstring str2 = rootUnit->GetWord();
		if (m_pUnitOperator->IsProper(pUnit) || m_pUnitOperator->IsProper(rootUnit) || str1.empty() || !str2.empty())
		{
			for(TUnitList::iterator it = unitList.begin(); it != unitList.end(); ++it)
			{
				IUnit* pUnit = *it;
				wstring strBase = pUnit->GetWord();
				wstring strLo = m_pUnitOperator->GetLowerWord(strBase);
				if (strLo!=L"of" && (strBase.empty() || !m_pUnitOperator->IsVisualChild(pUnit) ||
					m_pUnitOperator->IsMorpho(pUnit, oMorphoOfficial)))
				{
					continue;
				}
				resStrHash += strBase + L" ";
				resStr += strBase + L" ";
			}
			m_strHash = resStrHash;
			return resStr;
		}
	}

	TUnitList prefixUnits;
	TUnitList suffixUnits;
	TUnitList middleUnits;
	set<IUnit*> addedUnits;
	bool isQuote = false;

	// ищем суффикс - непосредственно название (в кавычках или без)
	IUnit* pLastUnit = *unitList.rbegin();
	wstring str = pLastUnit->GetWord();
	bool isStrongSuffix = false;
	if (m_pUnitOperator->IsQuote(str))
	{
		isQuote = true;
		int qCount = 0;
		for(TUnitList::const_reverse_iterator it = unitList.rbegin(); it != unitList.rend(); ++it)
		{
			IUnit* pUnit = *it;
			wstring str = pUnit->GetWord();	
			if (str.empty())
				continue;
			if (m_pUnitOperator->IsQuote(str))
				++qCount;
			else
			{ 
				suffixUnits.push_front(pUnit);
				addedUnits.insert(pUnit);
			}
			isStrongSuffix = true;
			if (qCount == 2)
				break;
		}
	}
	else
	{
		for(TUnitList::const_reverse_iterator it = unitList.rbegin(); it != unitList.rend(); ++it)
		{
			IUnit* pUnit = *it;
			wstring word = pUnit->GetWord();
			if (m_pUnitOperator->IsMorpho(pUnit, oMorphoGenetive))
				break;

			bool isEnglish = m_pUnitOperator->IsEnglishWord(pUnit);
			if ((isEnglish && !word.empty() && m_pUnitOperator->IsUnitChild(pUnit->GetParentUnit(), pUnit)) ||
				(!isEnglish && word.empty()))
			{
				continue;
			}

			if (m_pUnitOperator->IsProperUnknown(pUnit) || (m_pUnitOperator->IsProper(pUnit) &&
				(m_pUnitOperator->IsAbbr(pUnit) ||
				m_pUnitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtOrganization) ||
				m_pUnitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPlace))))
			{
				suffixUnits.push_front(pUnit);
				addedUnits.insert(pUnit);
			}
			else
				break;
		}
	}

	bool isInQuote = false;
	SMIList lstMIFin;
	SS::Core::Features::CMorphoFeature oMorpho;
	// ищем префикс - все до первого существительного
	for(TUnitList::iterator it = unitList.begin(); it != unitList.end(); ++it)
	{
		IUnit* pUnit = *it;
		wstring str = pUnit->GetWord();
		if (m_pUnitOperator->IsQuote(str))
		{
			isInQuote = true;
			continue;
		}
		if (m_pUnitOperator->IsMorpho(pUnit, oMorphoOfficial))
		{
			if (addedUnits.find(pUnit) == addedUnits.end())
				addedUnits.insert(pUnit);
			continue;
		}
		if (m_pUnitOperator->IsMorpho(pUnit, oMorphoNoun))
		{
			m_pUnitOperator->GetUnitMorfoFeaturesFull(pUnit, oMorpho, &lstMIFin);
			if (!prefixUnits.empty())
				m_strProfession = m_pUnitOperator->GetMainFormText1(pUnit, oMorpho.m_GenderType).form;
			if (!m_pUnitOperator->IsAbbr(pUnit))
			{
				for(SMIList::iterator smiIt = lstMIFin.begin(); smiIt != lstMIFin.end(); ++smiIt)
				{
					if (m_pUnitOperator->IsAbbr(pUnit) && (*smiIt).numberType.Equal(SS::Core::Features::Values::NumberType.ntPlural))
						continue;
					propList.push_back(*smiIt);
				}
			}
			break;
		}
		if (!isInQuote && (addedUnits.find(pUnit) == addedUnits.end()))
		{
			prefixUnits.push_back(pUnit);
			addedUnits.insert(pUnit);
		}
	}

	for(TUnitList::iterator it = unitList.begin(); it != unitList.end(); ++it)
	{
		IUnit* pUnit = *it;
		wstring strBase = pUnit->GetWord();
		bool isEnglish = m_pUnitOperator->IsEnglishWord(pUnit);
		if ((isEnglish && !strBase.empty() && m_pUnitOperator->IsUnitChild(pUnit->GetParentUnit(), pUnit)) ||
			(!isEnglish && (strBase.empty() || m_pUnitOperator->IsQuote(strBase))))
		{
			continue;
		}
		if (addedUnits.find(pUnit) == addedUnits.end())
		{
			middleUnits.push_back(pUnit);
			addedUnits.insert(pUnit);
		}
	}

	SS::Core::Features::CMorphoFeature oPatternAdjective;
	oPatternAdjective.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;
	oPatternAdjective.m_PartOfSpeechTypes.AssignOR(&SS::Core::Features::Values::PartOfSpeechTypes.postNumeric);
	oPatternAdjective.m_PartOfSpeechTypes.AssignOR(&SS::Core::Features::Values::PartOfSpeechTypes.postVerb);

	SS::Core::Features::CMorphoFeature oAdjective;
	oAdjective.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;

	/// нормализация префикса - по первому существительному цепочки
	for(TUnitList::iterator it = prefixUnits.begin(); it != prefixUnits.end(); ++it)
	{
		IUnit* pUnit = *it;
		wstring strBase = pUnit->GetWord();
		if (!m_pUnitOperator->IsVisualChild(pUnit) || (resStr.empty() && IsDelOrgPrefix(pUnit)))
			continue;
		wstring strNorm = m_pUnitOperator->GetMainFormText1(pUnit, oMorpho.m_GenderType, oMorpho.m_NumberType).form;
		if (m_pUnitOperator->IsMorpho(pUnit, oAdjective))
			m_Adjectives.insert(strNorm);
		if (m_pUnitOperator->IsMorpho(pUnit, oMorphoNoun) || m_pUnitOperator->IsMorpho(pUnit, oPatternAdjective) ||
			m_pUnitOperator->IsProperByUpper(pUnit))
		{
			if (!m_pUnitOperator->IsAbbr(pUnit) || pUnit->IsEvristicAbbr())
				resStrHash += strNorm + L" ";
			else
				resStrHash += strBase + L" ";
		}
		if (!m_pUnitOperator->IsAbbr(pUnit))
			resStr += strNorm + L" ";
		else
			resStr += strBase + L" ";

		if (!m_pUnitOperator->IsQuote(strNorm))
			m_strPrefix += strNorm + L" ";
	}

	/// здесь генетив
	bool isNoun = false;
	for(TUnitList::iterator it = middleUnits.begin(); it != middleUnits.end(); ++it)
	{
		IUnit* pUnit = *it;
		if (!m_pUnitOperator->IsVisualChild(pUnit))
			continue;
		wstring strNorm = pUnit->GetWord();
		if (!isNoun)
		{
			if (m_pUnitOperator->IsAbbr(pUnit) || pUnit->IsEvristicAbbr())
			{
				strNorm = AbbrNorm(pUnit);
				isNoun = true;
			}
			else
			{
				if (strNorm.empty()) /// английское слово
					strNorm = m_pUnitOperator->GetFullWord(pUnit);
				else
					strNorm = m_pUnitOperator->GetMainFormText1(pUnit, oMorpho.m_GenderType, oMorpho.m_NumberType).form;
				isNoun = m_pUnitOperator->IsMorpho(pUnit, oMorphoNoun);
			}
		}
		resStrHash += strNorm + L" ";
		resStr += strNorm + L" ";
	}

	if (isQuote)
		resStr += L"\"";

	for(TUnitList::iterator it = suffixUnits.begin(); it != suffixUnits.end(); ++it)
	{
		IUnit* pUnit = *it;
		if (!m_pUnitOperator->IsVisualChild(pUnit) || ((unitList.size() > 1) && resStr.empty() && IsDelOrgPrefix(pUnit)))
			continue;
		wstring strNorm = pUnit->GetWord();
		bool isAbbr = m_pUnitOperator->IsAbbr(pUnit);
		if (isAbbr || pUnit->IsEvristicAbbr())
			strNorm = AbbrNorm(pUnit);
		else if (strNorm.empty()) /// английское слово
			strNorm = m_pUnitOperator->GetFullWord(pUnit);
		else
			strNorm = m_pUnitOperator->GetMainFormText1(pUnit, oMorpho.m_GenderType, oMorpho.m_NumberType).form;
		resStrHash += strNorm + L" ";
		resStr += strNorm + L" ";
		m_Name += strNorm + L" ";
	}
	if (m_Name.empty())
		m_Name = resStr;

	if (isQuote)
		resStr += L"\"";
	m_strHash = resStrHash;
	return resStr;
}


bool CEntity::isMiddleNameEvristic(wstring str)
{
	wstring strLo = m_pUnitOperator->GetLowerWord(str);
	if (strLo.size() < 6)
		return false;
	strLo = strLo.substr(strLo.size() - 2, 2);
	if (strLo==L"ич" || strLo==L"ыч")
		return true;
	return false;
}

void CEntity::AddUnitToNameGroup(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if (m_setNameGroupUnits.find(pUnit) == m_setNameGroupUnits.end())
	{
		IUnit* pParent = pUnit->GetParentUnit();
		if (pParent && m_pUnitOperator->IsUnitChild(pParent, pUnit))
		{
			m_setNameGroupUnits.insert(pParent);
			TUnitList list = pParent->GetChildList();
			for(TUnitList::iterator it = list.begin(); it != list.end(); ++it)
				m_setNameGroupUnits.insert(*it);
		}
	}
}

wstring CEntity::GetPersonNormalString()
{	
	if (m_lang != SS::Core::Types::ealRussian)
        return GetPersonNormalStringEng();

	wstring resStr = L"";
	TUnitList prefixUnits;
	TUnitList middleUnits;
	SS::Core::Features::CMorphoFeature oMorfoNoun;
	oMorfoNoun.Undefine();
	SMIList lstMI1;
	SMIList lstMITemp;
	SMIList lstMIMiddleName;
	SMIList lstMILastName;
	SMIList lstMIFirstName;
	SMIList lstMI3;
	set<IUnit*> addedUnits;

	SS::Core::Features::CMorphoFeature oMorfoPattern;
	oMorfoPattern.Undefine();
	oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;

	for(TUnitList::iterator it = unitList.begin(); it!= unitList.end(); ++it)
	{
        IUnit *pUnit = *it;
		wstring str = pUnit->GetWord();
        if (!str.empty())
		{			
			if (pUnit->GetEvristicLN())
                break;

			if (pUnit->IsInitial() || (m_pUnitOperator->IsPersonName(pUnit) && m_pUnitOperator->IsMorpho(pUnit, oMorfoPattern)))
                break;
			if (addedUnits.find(pUnit) == addedUnits.end())
			{
				prefixUnits.push_back(pUnit);
				addedUnits.insert(pUnit);
			}

			if (m_pUnitOperator->IsMorpho(pUnit, oMorfoPattern))
			{
				m_pUnitOperator->GetUnitMorfoFeaturesFull(pUnit, oMorfoNoun, &lstMI1);
				break;
			}
		}
	}

    if (prefixUnits.empty())
	{
		int countNames = 0;
		int countOrgPlace = 0;
		for(TUnitList::iterator it = (++unitList.begin()); it != unitList.end(); ++it)
		{
			if ((m_pUnitOperator->IsSemantic(*it, SS::Core::Features::Values::TSemanticType.smtPlace) ||
				m_pUnitOperator->IsSemantic(*it, SS::Core::Features::Values::TSemanticType.smtOrganization))
				&& !m_pUnitOperator->IsSemantic(*it, SS::Core::Features::Values::TSemanticType.smtPerson) &&
                !(*it)->GetEvristicLN() && !(*it)->IsInitial() && !m_pUnitOperator->IsParentInitial(*it))
			{
				for(TUnitList::iterator it1 = (--it); it1 != unitList.begin(); --it1)
				{
					wstring str = (*it1)->GetWord();
					if (str.size() > 0)
                    {
						if (addedUnits.find(*it1) == addedUnits.end())
						{
							prefixUnits.push_front(*it1);
							addedUnits.insert(*it1);
						}
					}
				}
				wstring str = (*unitList.begin())->GetWord();
				if (str.size() > 0)
				{
					if (addedUnits.find(*unitList.begin()) == addedUnits.end())
					{
						prefixUnits.push_front(*unitList.begin());
						addedUnits.insert(*unitList.begin());
					}
					m_pUnitOperator->GetUnitMorfoFeaturesFull(*it, oMorfoNoun, &lstMI1);
				}
				break;
			}
            else if (m_pUnitOperator->IsPersonName(*it))
                ++countNames;
			if (countNames > 1)
                break;
		}
	}

	if (entType == entPersonName)
	{
		for(SMIList::iterator it = lstMI1.begin(); it != lstMI1.end(); ++it)
		{
			lstMITemp.push_back(*it);
			(*it).genType.Undefine();
		}
	}

	// теперь пытаемся определить род цепочки. Для этого идем с конца по всем именам и заносим в список
	// до тех пор, пока идут инициал, имя, фамилия, отчество или неизвестный пропер.
	// При этом если нашли семантическое имя, то берем по нему, далее по отчеству.
    // если их нет, то берем по уже найденному существительному

	bool isNameEnd = false;
	bool isEvriscticLN = false;
	bool isAbbr = false;
	bool isPo = false;
	bool isQuote = false;
	for(TUnitList::const_reverse_iterator it = unitList.rbegin(); it != unitList.rend(); ++it)
	{
		IUnit* pUnit = *it;
		wstring str = pUnit->GetWord();		
		if (str.empty() || m_pUnitOperator->IsParentInitial(pUnit))
            continue;	
		SS::Core::Types::EAnalyseLanguage eal = pUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst().GetLanguage();
		isAbbr = (m_pUnitOperator->IsAbbr(pUnit) || eal==SS::Core::Types::ealEnglish) && !pUnit->IsRusInitial();
		if (m_pUnitOperator->IsQuote(str))
            isQuote = true;
		TUnitList::const_reverse_iterator itPrev = it;
		++itPrev;
		if (entType==entPersonName && !isQuote && !isAbbr && !isNameEnd &&
			(m_pUnitOperator->IsPersonName(pUnit) || it==unitList.rbegin()))
		{
			if (!isEvriscticLN && !m_pUnitOperator->IsParentGeoCombi(pUnit) && entType==entPersonName)
                m_pUnitOperator->GetRusLNForms(pUnit, lstMI3);

			if (m_pUnitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
				SS::Core::Features::Values::TSPersonType.semprstMiddleName))
			{
				// наибольший приоритет так как почти нет омонимии
				m_pUnitOperator->GetUnitMorfoFeaturesFull(pUnit, oMorfoNoun, &lstMIMiddleName);
			}
            else if (m_pUnitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
					SS::Core::Features::Values::TSPersonType.semprstFirstName))
			{
				m_pUnitOperator->GetUnitMorfoFeaturesFull(pUnit, oMorfoNoun, &lstMIFirstName);
			}
			else if (!isEvriscticLN && !pUnit->IsInitial() &&
				(m_pUnitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
				SS::Core::Features::Values::TSPersonType.semprstLastName) || !lstMI3.empty()))
			{
				m_pUnitOperator->GetUnitMorfoFeaturesFull(pUnit, oMorfoNoun, &lstMILastName);
			}
            else 
            {						
			    if (addedUnits.find(pUnit)==addedUnits.end() && !pUnit->IsRusInitial() &&
				    (!m_pUnitOperator->IsProperUnknown(pUnit) || entType==entPersonInfo || 
                    m_pUnitOperator->IsParentGeoCombi(pUnit)) && (it!=unitList.rbegin()||
					entType==entPersonInfo))
				{
					middleUnits.push_back(pUnit);
					addedUnits.insert(pUnit);
				}
			}					
		}
		else if (!isQuote && !prefixUnits.empty() && pUnit==*(--prefixUnits.end()))
			break;
		else if (!str.empty() && !pUnit->IsRusInitial())
		{
			isNameEnd = true;
			if (addedUnits.find(pUnit) == addedUnits.end())
			{
				wstring strLo = m_pUnitOperator->GetLowerWord(str);
				if (strLo == L"по")
                    isPo = true;				
				middleUnits.push_back(pUnit);
				addedUnits.insert(pUnit);
			}
		}
	}

	int qCount = 0;
	wstring resStr2 = L"";
	wstring resStrForHash = L"";	
	oMorfoPattern.Undefine();
	oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;
	bool isProperInMiddle = false;
	for(TUnitList::iterator it = middleUnits.begin(); it != middleUnits.end(); ++it)
	{
		IUnit* pUnit = *it;
		if (m_pUnitOperator->IsParentHavePoint(pUnit))
            continue;
		wstring str = pUnit->GetWord();
		wstring str1 = L"";
		if (m_pUnitOperator->IsAbbr(pUnit))
			str1 = AbbrNorm(pUnit);
		else
			str1 = m_pUnitOperator->GetMainFormText1(pUnit,SS::Core::Features::Values::GenderType.Undefined).form;

		if (m_pUnitOperator->IsProper(pUnit))
            isProperInMiddle = true;
		else if (m_pUnitOperator->IsQuote(str))
		{
			isProperInMiddle = true;
			++qCount;
		}
		resStr2 = str + L" " + resStr2;
	}
	// теперь пересекаем списки возможных морфологических свойств цепочки
	SMIList lstMIFin1;
	SMIList lstMIFin2;
	SMIList lstMIFin3;
	SMIList lstMIFin;
	m_pUnitOperator->IntersectMorphoInfoLists(lstMIMiddleName, lstMIFirstName, lstMIFin1);
	if (lstMIFin1.size() == 0) 
		for(SMIList::iterator it = lstMIMiddleName.begin(); it != lstMIMiddleName.end(); ++it)
			lstMIFin1.push_back(*it);

	m_pUnitOperator->IntersectMorphoInfoLists(lstMIFin1, lstMILastName, lstMIFin2);		
	if (lstMIFin2.size() == 0) 
		for(SMIList::iterator it = lstMIFin1.begin(); it != lstMIFin1.end(); ++it)
			lstMIFin2.push_back(*it);

	if ((entType != entPersonInfo) &&
		m_pUnitOperator->IsSemantic(rootUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
		SS::Core::Features::Values::TSPersonType.semprstAgent) && 
		!(m_pUnitOperator->IsSemantic(rootUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
		SS::Core::Features::Values::TSPersonType.semprstProfession) ||
		m_pUnitOperator->IsSemantic(rootUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
		SS::Core::Features::Values::TSPersonType.semprstNationality)))
	{
		for(SMIList::iterator it = lstMIFin2.begin(); it != lstMIFin2.end(); ++it)
		{
			if (!(*it).caseType.IsIntersectsWith(&SS::Core::Features::Values::CaseType.ctCommon) &&
				(*it).caseType.IsIntersectsWith(&SS::Core::Features::Values::CaseType.ctGenitive))
			{
				entType = entPersonInfo;
			}
		}

		if (entType == entPersonInfo)
		{
			lstMIFin2.clear();
			lstMI1.clear();
			for(SMIList::iterator it = lstMITemp.begin(); it != lstMITemp.end(); ++it)
				lstMI1.push_back(*it);
		}
	}

	m_pUnitOperator->IntersectMorphoInfoLists(lstMI1, lstMIFin2, lstMIFin3);
	m_pUnitOperator->IntersectMorphoInfoLists(lstMIFin3, lstMI3, lstMIFin);

	SMorphoInfo smiFin;
	if (lstMIFin.empty())
	{
		smiFin.genType.Assign(SS::Core::Features::Values::GenderType.gtMasculine);
		smiFin.caseType.Assign(SS::Core::Features::Values::CaseType.Undefined);
		propList.push_back(smiFin);
	}
    else
	{
		propList.clear();
		SS::Core::Features::Types::GenderType firstGender;
		firstGender.Undefine();
		for(SMIList::iterator smiIt = lstMIFin.begin(); smiIt != lstMIFin.end(); ++smiIt)
		{
			if(!firstGender.Equal(SS::Core::Features::Values::GenderType.gtMasculine))
            {
				if ((*smiIt).genType.Equal(SS::Core::Features::Values::GenderType.gtMasculine))
				{
					propList.clear();
                    firstGender.Assign((*smiIt).genType);
				}
                else if(!firstGender.Equal(SS::Core::Features::Values::GenderType.gtFemale))
				{
					if ((*smiIt).genType.Equal(SS::Core::Features::Values::GenderType.gtFemale))
						propList.clear();
                    firstGender.Assign((*smiIt).genType);
				}
            }
			propList.push_back(*smiIt);
        }
		smiFin.genType.Assign(firstGender);
		smiFin.caseType.Assign((*propList.begin()).caseType);
	}

	wstring resStrPrefixForHash = L"";
	if (!prefixUnits.empty())
	{
		SS::Core::Features::Types::GenderType firstGender;
		firstGender.Assign(smiFin.genType);
		bool isFind = false;

		for(SMIList::iterator it = lstMITemp.begin(); it != lstMITemp.end(); ++it)
		{
			if ((*it).genType.Equal(smiFin.genType))
			{
				isFind = true;
                break;
			}
		}
		if (!isFind && lstMITemp.size()>0)
            firstGender.Assign((*lstMITemp.begin()).genType);

		wstring rootNormal = m_pUnitOperator->GetMainFormText1(rootUnit, firstGender).form;
		if (m_pUnitOperator->IsSemantic(rootUnit, SS::Core::Features::Values::TSemanticType.smtPerson))
			m_strProfession = rootNormal;
		if (CDictionaryPool::IsPost(m_pUnitOperator->GetLowerWord(rootNormal)))
			m_strPost = rootNormal;

		SS::Core::Features::CMorphoFeature oNoun;
		oNoun.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;
		SS::Core::Features::CMorphoFeature oAdjective;
		oAdjective.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;
		for(TUnitList::iterator it = prefixUnits.begin(); it != prefixUnits.end(); ++it)
		{
			wstring str = m_pUnitOperator->GetMainFormText1(*it, firstGender).form;
			if (it == (--prefixUnits.end()))
                resStrPrefixForHash = str;
			if (!m_pUnitOperator->IsQuote(str))
			{
				resStr += str + L" ";
				if (m_pUnitOperator->IsMorpho(*it, oAdjective))
					m_Adjectives.insert(str);
				if (!m_pUnitOperator->IsMorpho(*it, oNoun))
					m_strPrefix += str + L" ";
			}
		}
	}

	wstring resStr1 = L"";
	wstring resStrSuffixForHash = L""; // инициалы имена и отчества всегда выносим в конец
	// чтобы отлавливать Иванов И.В и И.В. Иванов как одно и тоже
	wstring resStrLastName = L"";
	wstring resStrFirstName = L"";
	wstring resStrMiddleName = L"";

	bool isFirstNameInTheEnd = false;
	bool isStrongLastName = false;
	if (entType == entPersonName)
	{
		for(TUnitList::const_reverse_iterator it = unitList.rbegin(); it != unitList.rend(); ++it)
		{
			IUnit* pUnit = *it;
			wstring str1 = pUnit->GetWord();
			if (str1.empty() || m_pUnitOperator->IsParentInitial(pUnit))
                continue;
			SS::Core::Types::EAnalyseLanguage eal = pUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst().GetLanguage();
			isAbbr = (m_pUnitOperator->IsAbbr(pUnit) || eal==SS::Core::Types::ealEnglish) && !pUnit->IsRusInitial();
			if (isAbbr)
                break;
			//lstMI3.clear();
			//		m_pUnitOperator->GetRusLNForms(pUnit, lstMI3);

			wstring str = m_pUnitOperator->GetRusLNBase(pUnit, smiFin);
			if (m_pUnitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
				SS::Core::Features::Values::TSPersonType.semprstFirstName))
			{
				str.clear();
			}

			if (m_pUnitOperator->IsPersonName(pUnit) || 
                (it==unitList.rbegin() && (!str.empty() || m_pUnitOperator->IsProper(pUnit))))
            /// последний всегда добавляем если есть эвристика персоны
			{
				wstring str2 = str;
				if (str.empty())
					str = m_pUnitOperator->GetMainFormText1(pUnit, smiFin.genType).form;
				if (addedUnits.find(pUnit) != addedUnits.end())
                    continue;
				resStr1 = str + L" " + resStr1;

				if ((resStrLastName.empty() || (!isStrongLastName)) &&
					!pUnit->IsRusInitial() && (m_pUnitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
					SS::Core::Features::Values::TSPersonType.semprstLastName) ||
					(m_pUnitOperator->IsProperUnknown(pUnit) && !isMiddleNameEvristic(str) &&
					!m_pUnitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtOrganization)) || !str2.empty() ||
					(m_pUnitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPlace) &&
					it==unitList.rbegin() && entType==entPersonName)))
				{
					if (!resStrLastName.empty() && resStrFirstName.empty())
						resStrFirstName = resStrLastName;
					resStrLastName = str;
					if (m_pUnitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
						SS::Core::Features::Values::TSPersonType.semprstLastName))
					{
						isStrongLastName = true;
					}
					AddUnitToNameGroup(pUnit);
				}
                else if (resStrMiddleName.empty() && (m_pUnitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
					SS::Core::Features::Values::TSPersonType.semprstMiddleName) || pUnit->IsRusInitial() ||
					(m_pUnitOperator->IsProperUnknown(pUnit) &&  isMiddleNameEvristic(str))))
				{
					if (resStrLastName.empty())
                        isFirstNameInTheEnd = true;
					resStrMiddleName = str;
					AddUnitToNameGroup(pUnit);
				}
                else if (m_pUnitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
					SS::Core::Features::Values::TSPersonType.semprstFirstName))
		        {
			        if (!resStrFirstName.empty() && resStrLastName.empty())
                        resStrLastName = resStrFirstName;
			        resStrFirstName = str;
			        AddUnitToNameGroup(pUnit);
		        }
		        else
		        {
			        if (resStrFirstName.empty() && !m_pUnitOperator->IsParentGeoCombi(pUnit) &&
						(pUnit->IsRusInitial() ||m_pUnitOperator->IsProperUnknown(pUnit)))
			        {
				        if (resStrLastName.empty() == 0)
                            isFirstNameInTheEnd = true;
				        resStrFirstName = str;
				        AddUnitToNameGroup(pUnit);
			        }
		        }
			}
            else
                break;
		}
	}

    IUnit *pLast = *unitList.rbegin();
	if (resStrLastName.empty() && entType==entPersonName && addedUnits.find(pLast) == addedUnits.end())
	{
		wstring str = m_pUnitOperator->GetMainFormText1(pLast, smiFin.genType).form;
		if (str!=resStrMiddleName && str!=resStrFirstName)
			resStrLastName = str;
		AddUnitToNameGroup(pLast);
	}

	if (resStrFirstName.empty() && !resStrMiddleName.empty())
	{
		resStrFirstName = resStrMiddleName;
		resStrMiddleName.clear();
	}
	
	resStrSuffixForHash = resStrLastName + L" " + resStrFirstName + L" " + resStrMiddleName;

	/*if (resStrLastName.empty() && !resStrFirstName.empty() && (entType == entPersonName))
	{
		wstring strLast = pLast->GetWord();
		if (!strLast.empty() && strLast!=resStrFirstName && strLast!=resStrMiddleName)
			resStrLastName = strLast;
	}*/

	m_strLastName = resStrLastName;
	m_strFirstName = resStrFirstName;
	m_strMiddleName = resStrMiddleName;

	if (m_strLastName.empty() && m_strFirstName.empty() && m_strMiddleName.empty())
        entType = entPersonInfo;

	qCount = 0;
	for(TUnitList::iterator it = middleUnits.begin(); it != middleUnits.end(); ++it)
	{
		IUnit *pUnit = *it;
		if (m_pUnitOperator->IsParentHavePoint(pUnit))
            continue;
		wstring str = pUnit->GetWord();

		if (m_pUnitOperator->IsQuote(str))
            ++qCount;

		// все слова с малеьнкой буквы не должны входить в хэш, если только у них нет пропер семантики страны, организации или имени.
		//if (m_pUnitOperator->IsNotHashOrgInsidePerson(pUnit))	
		wstring str1 = L"";
		if (m_pUnitOperator->IsAbbr(pUnit) || pUnit->IsEvristicAbbr())
			str1 = AbbrNorm(pUnit);
		else
			str1 = m_pUnitOperator->GetMainFormText1(pUnit, SS::Core::Features::Values::GenderType.Undefined).form;

		if (isQuote || (!m_pUnitOperator->IsMorpho(pUnit, oMorfoPattern) &&
			(m_pUnitOperator->IsProper(pUnit) || entType==entPersonInfo || (qCount%2==1) || !isProperInMiddle)))
        {
			resStrForHash = str1 + L" " + resStrForHash;
        }
	}
	resStr += resStr2 + resStr1;
	m_strHash = resStrPrefixForHash + L" " + resStrForHash + resStrSuffixForHash;
	return resStr;
}

// определяет может ли сущность быть объединена с предыдущей
// условие объединения - совпадение типов 
bool CEntity::IsHomogeneousUnionPossible(CEntity* pEnt)
{
	bool isPersonName = (entType == entPersonName) || (pEnt->entType == entPersonName);
	bool isPronoun = (entType == entPronoun) || (pEnt->entType == entPronoun);
	return !(isPersonName || isPronoun);
}

wstring CEntity::GetSubthemesXML(const wstring &srcText)
{
	if (m_lstSubEntites.empty())
        return L"";
	ATL::CAtlStringW strResult = L"<THEMES ID=\"";
	strResult.AppendFormat(L"%d", ID);
	strResult += L"\">";

	for(TEntityList::iterator itEntity = m_lstSubEntites.begin(); itEntity != m_lstSubEntites.end(); itEntity++)
	{
		CEntity *pEntity = *itEntity;
		int offsetBegin = pEntity->GetBeginOffset();
		int offsetEnd  = pEntity->GetEndOffsetNew(srcText);	
		if (offsetEnd<offsetBegin)
            offsetBegin=offsetEnd;
		wstring cdatastr = m_pUnitOperator->GetUnmarkingTextXML(srcText, offsetBegin, offsetEnd);
		if (offsetEnd - offsetBegin == 0)
            cdatastr.clear();
		strResult += pEntity->GetXML(cdatastr,false).c_str();
	}

	strResult += L"</THEMES>";
	return strResult;
}

wstring CEntity::GetFirstNameXML()
{
	if (!m_strFirstName.empty())
		return L" FN=\"" + m_strFirstName + L"\"";
	return L"";
}

wstring CEntity::GetMiddleNameXML()
{
	if (!m_strMiddleName.empty())
		return L" MN=\"" + m_strMiddleName + L"\"";
	return L"";
}

wstring CEntity::GetLastNameXML()
{
	if (!m_strLastName.empty())
		return L" LN=\"" + m_strLastName + L"\"";
	return L"";
}

wstring CEntity::GetNameXML()
{
	if (!m_Name.empty())
		return L" NAME=\"" + m_Name + L"\"";
	return L"";
}

wstring CEntity::GetPseudoPlaceXML()
{
	if(this->isPseudoPlace)
		return L" PLACE=\"\"";
	return L"";
}

wstring CEntity::GetLinkedPlaces()
{
	if (entType == entPlaceName)
	{
		wstring result = L"";
		{
			bool isFirst = true;			
			for(TUnitList::iterator it = lstLinkUnits.begin(); it != lstLinkUnits.end(); ++it)
			{
				wstring linkValue = L"";
				if (!isFirst)
					result += L",";
				IUnit* pUnit = *it;
				TUnitList allUnits;
				m_pUnitOperator->GetLinearUnits(pUnit, allUnits);
				for(TUnitList::iterator it1 = allUnits.begin(); it1 != allUnits.end(); ++it1)
				{
					wstring str1 = m_pUnitOperator->GetMainFormText1(*it1, SS::Core::Features::Values::GenderType.Undefined).form;
					if (m_pUnitOperator->IsQuote(str1))
						linkValue += L'\'';
					if (str1.size() > 0)
						linkValue += str1 + L" ";
				}
				linkValue = m_pUnitOperator->SpaceNormalize(linkValue);
				if (linkValue != m_strValue)
				{
					isFirst = false;
					result += linkValue;
				}
				//str += (*it)->GetRegionNormalString();
			}
		}
		result = m_pUnitOperator->QuoteNormalize(result);
		return result;
	}
	return L"";
}

wstring CEntity::GetLinkedPlacesXML()
{
	wstring result = GetLinkedPlaces();
	if (!result.empty())
		return L"\" INFO=\"" + result;
	return L"";
}

wstring CEntity::GetXML(wstring value, int sentOffset, bool isEntityTeg)
{
	wchar_t sPos[10];
	wsprintfW(sPos, L"%d", GetBeginOffset() - sentOffset);

	wstring strPos = sPos;

	wchar_t sID[15];
	wsprintfW(sID, L"%d", ID);
	wstring strID = sID;

	wstring resXML = L"";

	wstring tegName;
	wstring tegNameEnd;

	if (isEntityTeg)
		tegName = L"ENTITY";
    else
		tegName   = L"THEME";

    if (nType == eEntityBaseType::Ceterus)
    {
        tegName = L"CET";
        tegNameEnd = tegName;
	    tegName += L" ID=\"" + strID + L"\"";
        tegName += L" TYPE=\"" + GetTypeStr() + L"\"";
		tegName += L" POS=\"" + strPos + L"\"";
    }
    else if (nType == eEntityBaseType::Verb)
	{
		tegName = L"VERB";
        tegNameEnd = tegName;
		tegName += L" ID=\"" + strID + L"\"";
		tegName += L" TYPE=\"" + GetVerbType() + L"\"";
		tegName += L" MI=\"" + GetMorphoPropStr() + L"\"";
		tegName += L" POS=\"" + strPos + L"\"";
	} 
	else
	{
        tegNameEnd = tegName;
		tegName += L" ID=\"" + strID + L"\"";
		tegName += L" POS=\"" + strPos + L"\"";

		if (!m_strProfession.empty() ||
			!m_strFirstName.empty()||
			!m_strLastName.empty() ||
			!m_strMiddleName.empty()||
			!m_strPrefix.empty())
        {
			tegName +=  L" PROF=\"" + m_strProfession + L"\"";
        }

		if (!m_strPrefix.empty())
			tegName += L" PRE=\"" + m_strPrefix + L"\"";

		tegName += L" MI=\"" + GetMorphoPropStr() + L"\" TYPE=\"" + GetTypeStr() + L"\"";

		tegName += GetLastNameXML();
		tegName += GetFirstNameXML();
		tegName += GetMiddleNameXML();
		tegName += GetNameXML();

		if (m_pUnitOperator->IsSyntax(rootUnit, SS::Core::Features::Values::SyntaxCategories.scSubjectFin))
			tegName += L" ROLE=\"subj\"";
		else
			tegName += L" ROLE=\"obj\"";

        tegName += GetPseudoPlaceXML();
	}
	if (!m_Adjectives.empty())
	{
		wstring value = L"";
		bool isFirst = false;
		for (set<wstring>::iterator it = m_Adjectives.begin(); it != m_Adjectives.end(); ++it)
		{
			if (isFirst)
				value += L",";
			value += *it;
			isFirst = true;
		}
		tegName += L" ADJ=\"" + value + L"\"";
	}

	resXML = L"<" + tegName + L" VALUE=\"";
	//this->GetNormalString();
	
	resXML += m_strValue;
	switch(entType)
	{
		case entPlaceNoInfo:
		case entPlaceName:
			resXML += GetLinkedPlacesXML();
			break;
		
		case entPersonName:
		case entPersonInfo:
			resXML += L"\" HASH=\"" + m_strHash;
			break;

		case entOrgName:
		case entOrgAbbr:
		case entOrgInfo:
		{
			wstring fullname = L"";
			if (!this->unitResolutions.empty())
			{
				TUnitList resUnitList = *unitResolutions.begin();
				if (!resUnitList.empty())
					fullname = GetDefaultNormalString(resUnitList);
			}
            resXML += L"\" HASH=\"" + m_strHash;
			if (!fullname.empty())
				resXML += L"\" FULLNAME=\"" + fullname;
			break;
		}
		default:
			break;
	}
	resXML += L"\">" + value;
	resXML += L"</" + tegNameEnd  + L">";
	return resXML;
}

wstring CEntity::GetDateNormalString()
{	
	return this->m_DateInterval.GetString();
}

wstring CEntity::GetVerbNormalString()
{
	IUnit *pUnit = *unitList.begin();
	wstring res = L"";
	wstring word = pUnit->GetWord();
	if (!word.empty())
		res = m_pUnitOperator->GetVerbNormText(pUnit);
	else
	{
		TUnitList::iterator it = unitList.begin();
		while(true)
		{
			++it;
			if (it == unitList.end())
				break;
			pUnit = *it;
			if (!pUnit->GetParentUnit())
				break;

			if (m_pUnitOperator->IsVerbUnit(pUnit))
				res += m_pUnitOperator->GetVerbNormText(pUnit);
			else
				res += m_pUnitOperator->GetMainFormText1(pUnit, SS::Core::Features::Values::GenderType.Undefined).form;
			res += L" ";
		}
	}

	SMIList lstMI;
	SS::Core::Features::CMorphoFeature oMorfoNoun;
	oMorfoNoun.Undefine();
	m_pUnitOperator->GetUnitMorfoFeaturesFull(rootUnit, oMorfoNoun, &lstMI);
	for(SMIList::iterator smiIt = lstMI.begin(); smiIt != lstMI.end(); ++smiIt)
		propList.push_back(*smiIt);

	return res;
}

wstring CEntity::GetUnchangeableNormalString()
{
    wstring res = L"";
	for(TUnitList::iterator it = unitList.begin(); it != unitList.end(); ++it)
	{
        wstring str = (*it)->GetWord();
        if(!str.empty())
        {
		    res += str;
		    res += L" ";
        }
	}
    return res;
}

wstring CEntity::GetNormalStringByPartOfSpeech(SS::Core::Features::Types::PartOfSpeechTypes partOfSpeech,
    SS::Core::Features::Types::GenderType gender/* = SS::Core::Features::Values::GenderType.Undefined*/)
{
	wstring res = L"";
	for(TUnitList::iterator it = unitList.begin(); it != unitList.end(); ++it)
	{
		res += m_pUnitOperator->GetMainFormText1(*it, gender, SS::Core::Features::Values::NumberType.ntSingle, partOfSpeech).form;
		res += L" ";
	}

	SMIList lstMI;
	SS::Core::Features::CMorphoFeature oMorfo;
	m_pUnitOperator->GetUnitMorfoFeaturesFull(rootUnit, oMorfo, &lstMI);
	bool isPluralNumeric = false;
	if (partOfSpeech.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postNumeric))
	{
		isPluralNumeric = oMorfo.m_NumericType.IsContain(SS::Core::Features::Values::NumericType.ntCollection) ||
			(oMorfo.m_NumericType.IsContain(SS::Core::Features::Values::NumericType.ntCount) && oMorfo.m_NumberType.IsUndefined());
	}
	for(SMIList::iterator smiIt = lstMI.begin(); smiIt != lstMI.end(); ++smiIt)
    {
		SMorphoInfo morphoInfo = *smiIt;
        if (morphoInfo.partOfSpeech.Equal(partOfSpeech))
		{
			if (isPluralNumeric)
				morphoInfo.numberType = SS::Core::Features::Values::NumberType.ntPlural;
		    propList.push_back(morphoInfo);
		}
    }
    if(propList.empty())
    {
        m_pUnitOperator->GetUnitMorfoFeaturesFullByMorphoAnalyzer(rootUnit, oMorfo, &lstMI);
        for(SMIList::iterator smiIt = lstMI.begin(); smiIt != lstMI.end(); ++smiIt)
        {
            if ((*smiIt).partOfSpeech.Equal(partOfSpeech) && 
                (gender.Equal(SS::Core::Features::Values::GenderType.Undefined) || 
                (*smiIt).genType.Equal(gender)))
            {
		        propList.push_back(*smiIt);
            }
        }
    }
	return res;
}

wstring CEntity::GetDefaultNormalString(TUnitList& unitList1)
{
	wstring res = L"";
	SS::Core::Features::CMorphoFeature oMorfoNoun;
	oMorfoNoun.Undefine();

	SS::Core::Features::CMorphoFeature oMorfoPatternOfficial;
	oMorfoPatternOfficial.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;	
    oMorfoPatternOfficial.m_OfficialType = SS::Core::Features::Values::OfficialType.otConjunction;
    oMorfoPatternOfficial.m_OfficialType.AssignOR(&SS::Core::Features::Values::OfficialType.otPretext);
    oMorfoPatternOfficial.m_OfficialType.AssignOR(&SS::Core::Features::Values::OfficialType.otSymbol);

	SS::Core::Features::CMorphoFeature oMorfoPatternNoun;
	oMorfoPatternNoun.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;	

	SS::Core::Features::CMorphoFeature oMorfoPatternAdjective;
	oMorfoPatternAdjective.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;	

	SS::Core::Features::CMorphoFeature oMorfoPatternParticiple;
	oMorfoPatternParticiple.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;		
    oMorfoPatternParticiple.m_VerbForm = SS::Core::Features::Values::VerbForm.vfParticiple;

	SS::Core::Features::CMorphoFeature oMorfoPattern2;
	oMorfoPattern2.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postPronoun;	

	SS::Core::Features::Types::GenderType firstGender;
	firstGender.Undefine();

	bool isNoun = false;
    nounUnit = NULL;

	for(TUnitList::iterator it = unitList1.begin(); it != unitList1.end(); it++)	
	{
        wstring str = (*it)->GetWord();
        if(str == L"of")
            break;

		if (m_pUnitOperator->IsMorpho(*it, oMorfoPatternNoun)/* && !m_pUnitOperator->isEvristicAdj(*it)*/)
		{
            bool analyze = false;
            if(m_lang == SS::Core::Types::EAnalyseLanguage::ealRussian)
            {
                if (!m_pUnitOperator->IsPseudoAdjective(*it) || propList.empty())
                    analyze = true;
            }
            else
                analyze = true;

			if(analyze)
			{
				nounUnit = *it;
				SMIList lstMI;
				m_pUnitOperator->GetUnitMorfoFeaturesFull(*it, oMorfoNoun, &lstMI);
				propList.clear();
				firstGender.Undefine();
                bool isGenderAnim = true;
                /// даем проритет мужскому роду в случае если родов несколько
                /// так же приоритет дается неодушевленным морфоформам
				for(SMIList::iterator smiIt = lstMI.begin(); smiIt != lstMI.end(); ++smiIt)
				{
                    if(!(*smiIt).partOfSpeech.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postNoun))
                        continue;

                    if(m_lang == SS::Core::Types::EAnalyseLanguage::ealRussian)
                    {
                        bool isAnimation = (*smiIt).animationType.Equal(SS::Core::Features::Values::AnimInAnimType.aiaAnimation);
                        if(isAnimation && !isGenderAnim)
                            continue;

                        SS::Core::Features::Types::GenderType gender = (*smiIt).genType;
						if (isGenderAnim && !isAnimation)
                        {
                            firstGender.Assign(gender);
                            propList.clear();
                            isGenderAnim = false;
                        }
                        else if(!firstGender.Equal(SS::Core::Features::Values::GenderType.gtMasculine))
                        /// (isGenderAnim == isAnimation)
                        {
				            if (gender.Equal(SS::Core::Features::Values::GenderType.gtMasculine))
							{
                                firstGender.Assign(gender);
								propList.clear();
							}
                            else if(!firstGender.Equal(SS::Core::Features::Values::GenderType.gtFemale))
							{
								if(!gender.IsUndefined() && !firstGender.Equal(gender))
								{
									firstGender.Assign(gender);
									propList.clear();
								}
							}
                        }
                    }
                    propList.push_back(*smiIt);
				}
			}

			if ((m_lang == SS::Core::Types::EAnalyseLanguage::ealRussian) && 
				(!m_pUnitOperator->IsPseudoAdjective(*it) || (rootUnit == nounUnit)))
			{
                break;
			}
		}
	}

	SS::Core::Features::Types::NumberType number;
	if (nounUnit)
		number = m_pUnitOperator->GetMainFormText1(nounUnit, firstGender, SS::Core::Features::Values::NumberType.ntSingle, SS::Core::Features::Values::PartOfSpeechTypes.postNoun).number;
	bool isPreviousEmpty = true;
	for(TUnitList::iterator it = unitList1.begin(); it != unitList1.end(); ++it)
	{
		bool isMissed = false;
        IUnit *pUnit = *it;
		if (!isNoun)
        {
            SS::Core::Features::CMorphoFeature oMorfo;
            m_pUnitOperator->GetUnitMorfoFeaturesFull(pUnit, oMorfo);
            if (oMorfo.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postOfficial) && !pUnit->IsInitial())
			{
				isMissed = true;
				if (oMorfo.m_OfficialType.Equal(SS::Core::Features::Values::OfficialType.otPretext))
				{ }
				else if ((oMorfo.m_OfficialType.Equal(SS::Core::Features::Values::OfficialType.otConjunction) ||
					oMorfo.m_OfficialType.Equal(SS::Core::Features::Values::OfficialType.otSymbol)) && !isPreviousEmpty)
				{
					isPreviousEmpty = true;
				}
				else
					continue;
			}
			/*else if (oMorfo.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postNumeric))
			{
			}*/
        }
		if (m_pUnitOperator->IsMorpho(pUnit, oMorfoPattern2))
            continue;

		wstring word = pUnit->GetWord();
		IUnit *pParent = pUnit->GetParentUnit();
		wstring parentWord = pParent ? pParent->GetWord() : L"";
		if (word.empty() || (m_pUnitOperator->IsUnitChild(pParent, pUnit) && !parentWord.empty()) || word==L"для")
            continue;

		int ind1 = word.find(L".", 0);
		int ind2 = word.find(L".", ind1 + 1);
		if (ind1>=0 && ind2>=0)
            continue;

		if (!isNoun && pUnit->GetChildList().empty())
		{
			SS::Core::Features::Types::PartOfSpeechTypes partOfSpeech = SS::Core::Features::Values::PartOfSpeechTypes.Undefined;
			if (pUnit == nounUnit)
				partOfSpeech = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;
			word = m_pUnitOperator->GetMainFormText1(pUnit, firstGender, number, partOfSpeech).form;
		}
		if ((pUnit != nounUnit) && m_pUnitOperator->IsMorpho(pUnit, oMorfoPatternAdjective))
			m_Adjectives.insert(word);
		if (m_pUnitOperator->IsQuote(word))
            word = L"'";
		if (((word == L",") || (word == L"и") || (word == L"или") || (word == L"and") || (word == L"or")))
			continue;
		else
			isPreviousEmpty = false;
		res += word + L" ";
		if (pUnit == nounUnit)
		{
			if (!isNoun)
			{
				if (unitList1.size() > 1)
					m_strProfession = word;

				if (m_pUnitOperator->IsSemantic(*it, SS::Core::Features::Values::TSemanticType.smtPerson,
					SS::Core::Features::Values::TSPersonType.semprstFirstName))
				{
					m_strFirstName = word;
				}
                else if (m_pUnitOperator->IsSemantic(*it, SS::Core::Features::Values::TSemanticType.smtPerson,
                    SS::Core::Features::Values::TSPersonType.semprstMiddleName))
				{
					m_strMiddleName = word;
				}
			}
			isNoun = true;
		}

		if (!isNoun && !isMissed && !m_pUnitOperator->IsQuote(word))
			m_strPrefix += word + L" ";
	}
	res = m_pUnitOperator->SpaceNormalize(res);
	res = m_pUnitOperator->QuoteNormalize(res);
	m_strPrefix = m_pUnitOperator->SpaceNormalize(m_strPrefix);
	m_strPrefix = m_pUnitOperator->QuoteNormalize(m_strPrefix);
	return res;
}

wstring CEntity::GetDefaultNormalString()
{	
	return GetDefaultNormalString(unitList);
}

CDateInterval CEntity::GetNormalForSimpleDate(IUnit* pDate,  CDateInterval inputInt, set<IUnit*>& stopUnits,int mode)											  
{
	CDateInterval resDI;
    TUnitList symbols;
	TUnitList numUnitsWithCombi;
	TUnitList dateUnits;
	TUnitList unitList;
	SS::Core::Features::CMorphoFeature oMorfoUnit;
	SS::Core::Features::CSemanticFeature oSemanticUnit;

    SS::Core::Features::CMorphoFeature oMorfoSymbol;
    oMorfoSymbol.Undefine();
    oMorfoSymbol.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;
    oMorfoSymbol.m_OfficialType = SS::Core::Features::Values::OfficialType.otSymbol;

	m_pUnitOperator->GetLinearUnits(pDate, unitList);
	for(TUnitList::iterator it = unitList.begin(); it != unitList.end(); ++it)
	{
		wstring word = (*it)->GetWord();

		m_pUnitOperator->GetUnitSemanticFeatures(*it,oSemanticUnit);
		m_pUnitOperator->GetUnitMorfoFeatures(*it,oMorfoUnit);			
		if (oMorfoUnit.m_PartOfSpeechTypes.IsIntersectsWith(&(SS::Core::Features::Values::PartOfSpeechTypes.postNumeric)) && 
            ((*it)->GetHashKey()!=0 || !this->m_pUnitOperator->isHaveParentCombiNumeric(*it)))
		{
			numUnitsWithCombi.push_back(*it);
		}
        else if(m_pUnitOperator->IsMorpho(*it, oMorfoSymbol))
            symbols.push_back(*it);

		if (stopUnits.find(*it)==stopUnits.end() && word.size()>0 && !m_pUnitOperator->IsHavePoint(*it) &&			
			oSemanticUnit.m_SemanticType.IsIntersectsWith(&(SS::Core::Features::Values::TSemanticType.smtTimeInterval)))
		{
			dateUnits.push_back(*it); // оставлять только юниты относящиеся к дате. Остальные убирать
		}
	}

	wstring patternType = pDate->GetPatternName(); 
	if (patternType.size() == 0)
    // значит выделено как синтаксическая конструкция, а не как лексическая
	{		
		wstring strNorm = m_pUnitOperator->GetMainFormText(pDate, SS::Core::Features::Values::GenderType.gtMasculine);
		wstring strNormLo = m_pUnitOperator->GetLowerWord(strNorm);	
		int numYearRoot = StrToIntW(pDate->GetWord());
		if (numYearRoot>1700 && numYearRoot<2500)
		{
			resDI.begin.nDay = 1;
			resDI.begin.nMonth = 1;
			resDI.begin.nYear = numYearRoot;

			resDI.end.nDay = 31;
			resDI.end.nMonth = 12;
			resDI.end.nYear = numYearRoot;

			return resDI;
		}
		IUnit* pModUnit = pDate->GetLeftChildUnit();	
		bool b1 = false;
		bool b2 = false;
		__int64 mod = 0;
		wstring strNormLoMod = L"";
		while (pModUnit)
		{
			wstring str1 = pModUnit->GetWord();
			wstring str2 = pModUnit->GetPatternName();		  
			b1 = pModUnit->HasChilds()&&  str1.size()==0 && str2.size()==0;
			b2 = !pModUnit->HasChilds()&& (stopUnits.find(pModUnit)==stopUnits.end());
			if (b1 || b2)
			{
				mod = pModUnit->GetHashKey();
				if (mod == 0)
                    mod = StrToIntW(pModUnit->GetWord());		 	
				if (mod == 0)
				{
					wstring str= pModUnit->GetWord();			
					IUnit* pNumUnit = pModUnit->GetLeftChildUnit();
					if (str.size()==0 &&  pNumUnit!=NULL)
						mod = StrToIntW(pNumUnit->GetWord());
				}
				wstring strNormMod = m_pUnitOperator->GetMainFormText(pModUnit, SS::Core::Features::Values::GenderType.gtMasculine);
				strNormLoMod = m_pUnitOperator->GetLowerWord(strNormMod);
				if (IsPreviousWord(strNormLoMod) ||
					IsNextWord(strNormLoMod) ||
					IsCurrentWord(strNormLoMod, true) ||
					IsLastWord(strNormLoMod) || mod!=0)
				{
					break;
				}
			}
			pModUnit = pModUnit->GetRightUnit();
		}		
		if (strNormLo==L"год" || strNormLo==L"year") // типа "прошлый год"
			GetByYearModificator(resDI, strNormLoMod, *m_pBaseDate);
        else if (strNormLo==L"полугодие" || strNormLo==L"halfyear")
            GetByHalfYearModificator(resDI, strNormLoMod, mod, *m_pBaseDate, inputInt);
        else if (strNormLo==L"квартал" || strNormLo==L"quarter")
            GetByQuarterModificator(resDI, strNormLoMod, mod, *m_pBaseDate, inputInt);
        else if (strNormLo==L"месяц" || strNorm==L"month")
            GetByMonthModificator(resDI, strNormLoMod, mod, *m_pBaseDate, inputInt);
        else if (strNormLo==L"декада" || strNorm==L"decade")
            GetByDecadeModificator(resDI, strNormLoMod, mod, *m_pBaseDate, inputInt);
		else if (strNormLo==L"неделя" || strNorm==L"week")
            GetByWeekModificator(resDI, strNormLoMod, L"",mod, *m_pBaseDate, inputInt);
        else if (strNormLo==L"день" || strNormLo==L"число" || strNorm==L"day")
            GetByDayModificator(resDI, strNormLoMod, mod, *m_pBaseDate,  inputInt);
        else if (GetNumberForMonth(strNormLo) > 0)
			GetByMonthNameModificator(resDI, strNormLo, strNormLoMod, *m_pBaseDate,  inputInt);					 
		else if (GetDayOfWeekByName(strNormLo) > 0)
			GetByWeekModificator(resDI, strNormLoMod, strNormLo, mod,*m_pBaseDate,  inputInt);
        else if (inputInt.begin.nYear!=0 && Season(strNormLo, resDI, inputInt.begin.nYear))
        {
        }
		else
            GetByBeginEndModificator(resDI, strNormLo, strNormLoMod, mod, *m_pBaseDate, inputInt);					
	}
	else if (patternType==L"DI" && numUnitsWithCombi.size()==2 && dateUnits.size()>0)
    {
		IUnit* pMonth = *(--dateUnits.end());
		wstring strNorm = m_pUnitOperator->GetMainFormText(pMonth, SS::Core::Features::Values::GenderType.gtMasculine);
		wstring strNormLo = m_pUnitOperator->GetLowerWord(strNorm);
		IUnit* pNum1 =  *(numUnitsWithCombi.begin());		
		IUnit* pNum2 =  *(++numUnitsWithCombi.begin());
		if (inputInt.begin.nYear==0)
			resDI.begin.nYear = resDI.end.nYear = this->m_pBaseDate->nYear;
		else
			resDI.begin.nYear = resDI.end.nYear = inputInt.begin.nYear;

		resDI.begin.nMonth = resDI.end.nMonth =  GetNumberForMonth(strNormLo);
		int day1 = pNum1->GetHashKey();
		if (day1==0) day1 = StrToIntW(pNum1->GetWord());
		int day2 = pNum1->GetHashKey();
		if (day2==0) day2 = StrToIntW(pNum2->GetWord());
		resDI.begin.nDay = day1;
		resDI.end.nDay = day2;
	}
    else if (patternType==L"DYI" && numUnitsWithCombi.size()==3 && dateUnits.size()>1)
	{
		IUnit* pMonth = *(--(--dateUnits.end()));
		wstring strNorm = m_pUnitOperator->GetMainFormText(pMonth, SS::Core::Features::Values::GenderType.gtMasculine);
		wstring strNormLo = m_pUnitOperator->GetLowerWord(strNorm);
		IUnit* pNum1 =  *(numUnitsWithCombi.begin());		
		IUnit* pNum2 =  *(++numUnitsWithCombi.begin());
		IUnit* pNum3 =  *(--numUnitsWithCombi.end());		
		resDI.begin.nMonth = resDI.end.nMonth =  GetNumberForMonth(strNormLo);
		int day1 = pNum1->GetHashKey();
		if (day1==0) day1 = StrToIntW(pNum1->GetWord());
		int day2 = pNum1->GetHashKey();
		if (day2==0) day2 = StrToIntW(pNum2->GetWord());
		int year = pNum1->GetHashKey();
		if (year==0) year = StrToIntW(pNum3->GetWord());
		resDI.begin.nDay = day1;
		resDI.end.nDay = day2;
		resDI.begin.nYear = resDI.end.nYear = year;
	}
	else if (patternType==L"YM" &&   numUnitsWithCombi.size()==1 && dateUnits.size()>0)
    {
		IUnit* pMonth = *(dateUnits.begin());
		IUnit* pNum1 =  *(numUnitsWithCombi.begin());
		wstring strNorm = m_pUnitOperator->GetMainFormText(pMonth, SS::Core::Features::Values::GenderType.gtMasculine);
		wstring strNormLo = m_pUnitOperator->GetLowerWord(strNorm);
		int year = pNum1->GetHashKey();
		if (year==0) year = StrToIntW(pNum1->GetWord());

		resDI.begin.nYear = resDI.end.nYear = year;
		if (!Season(strNormLo, resDI, year))
		{
			resDI.begin.nMonth = resDI.end.nMonth =  GetNumberForMonth(strNormLo);
			resDI.begin.nDay = 1;
			resDI.end.nDay =  GetLastDayForMonth(resDI.end);		
		}
	}
    else if ((patternType==L"DM" || patternType==L"DM1") && numUnitsWithCombi.size()==2 && dateUnits.size()>0)
	{
		IUnit* pNum1 =  *(numUnitsWithCombi.begin());
		IUnit* pNum2 =  *(++numUnitsWithCombi.begin());
		IUnit* pMonth = *(--dateUnits.end());
		wstring strNorm = m_pUnitOperator->GetMainFormText(pMonth, SS::Core::Features::Values::GenderType.gtMasculine);
		wstring strNormLo = m_pUnitOperator->GetLowerWord(strNorm);	
		if (inputInt.begin.nYear==0)
			resDI.begin.nYear = resDI.end.nYear = this->m_pBaseDate->nYear;
		else 
			resDI.begin.nYear = resDI.end.nYear = inputInt.begin.nYear; 
		resDI.begin.nMonth = resDI.end.nMonth =  GetNumberForMonth(strNormLo);
		resDI.begin.nDay = StrToIntW(pNum1->GetWord());
		resDI.end.nDay = StrToIntW(pNum2->GetWord());
    }
    else if ((patternType==L"Century") && numUnitsWithCombi.size()==1)
	{
		IUnit* pNum1 = *(numUnitsWithCombi.begin());
		wstring number = pNum1->GetWord();
		int century = StrToIntW(number.c_str());
		if (century == 0)
			century = m_pUnitOperator->ConvertFromRomanNumber(number);
		if (century == 0)
			century = pNum1->GetHashKey();
		resDI.begin.nYear = (century - 1) * 100 + 1;
		resDI.end.nYear = century * 100;
		// интервал - целый год
		resDI.begin.nDay = 1;
		resDI.begin.nMonth = 1;
		resDI.end.nDay = 31;
		resDI.end.nMonth = 12;
	}
    else if ((patternType==L"NumericYear")  && numUnitsWithCombi.size()==1)
	{
		IUnit* pNum1 = *(numUnitsWithCombi.begin());
		resDI.begin.nYear = resDI.end.nYear = StrToIntW(pNum1->GetWord());
		// интервал - целый год
		resDI.begin.nDay = 1;
		resDI.begin.nMonth = 1;
		resDI.end.nDay = 31;
		resDI.end.nMonth = 12;
    }
    else if ((patternType==L"Year") && numUnitsWithCombi.size()==2)
	{
		IUnit* pNum1 = *(numUnitsWithCombi.begin());
		IUnit* pNum2 = *(++numUnitsWithCombi.begin());
		resDI.begin.nYear = StrToIntW(pNum1->GetWord());
		resDI.end.nYear = StrToIntW(pNum2->GetWord());
		// интервал - целый год
		resDI.begin.nDay = 1;
		resDI.begin.nMonth = 1;
		resDI.end.nDay = 31;
		resDI.end.nMonth = 12;
	}
    else if ((patternType==L"Year1" || patternType==L"Year2") && numUnitsWithCombi.size()==1)
	{
		IUnit* pNum1 = *(numUnitsWithCombi.begin());
		int year = pNum1->GetHashKey();
		if (year==0) year = StrToIntW(pNum1->GetWord());
		resDI.begin.nYear = resDI.end.nYear = year;
		// интервал - целый год
		resDI.begin.nDay = 1;
		resDI.begin.nMonth = 1;
		resDI.end.nDay = 31;
		resDI.end.nMonth = 12;
	}
    else if ((patternType==L"DMY1" || patternType==L"DMY2")&& numUnitsWithCombi.size()==2 && dateUnits.size()==1)
    // 1999-Jan-14
	{
		IUnit* pNum1 = *(numUnitsWithCombi.begin());
		IUnit* pNum2 = *(++numUnitsWithCombi.begin());
		if (patternType == L"DMY1")
		{
			resDI.begin.nYear = resDI.end.nYear = StrToIntW(pNum1->GetWord());
			resDI.begin.nDay = resDI.end.nDay = StrToIntW(pNum2->GetWord());
		}
		else
		{
			resDI.begin.nYear = resDI.end.nYear = StrToIntW(pNum2->GetWord());
			resDI.begin.nDay = resDI.end.nDay = StrToIntW(pNum1->GetWord());
		}
		IUnit* pMonth = *(dateUnits.begin());
		wstring str = pMonth->GetWord();
		wstring strNormLo = m_pUnitOperator->GetLowerWord(str);
		resDI.begin.nMonth = resDI.end.nMonth = GetNumberForMonth(strNormLo);
	}
    else if ((patternType==L"DMY6" || patternType==L"DMY5" || patternType==L"YMD") && numUnitsWithCombi.size()==3)
    // 2008-10-27 или 02/03/2008
	{
        bool equalSymbols = true;
        if(symbols.size() > 0)
        /// выясним одинаковые ли разделители
        {
            wstring firstSymbol = (*symbols.begin())->GetWord();
            for(TUnitList::iterator it = symbols.begin(); equalSymbols && (it != symbols.end()); ++it)
                if(firstSymbol != (*it)->GetWord())
                    equalSymbols = false;
        }
        if(equalSymbols)
        /// пронормализуем, если одинаковые разделители
        {
		    TUnitList::iterator it = numUnitsWithCombi.begin();
		    IUnit* pUnit1 = *it;
		    IUnit* pUnit2 = *(++it);
		    IUnit* pUnit3 = *(++it);
		    // превращаем в числа
		    int n1 = StrToIntW(pUnit1->GetWord());
		    int n2 = StrToIntW(pUnit2->GetWord());
		    int n3 = StrToIntW(pUnit3->GetWord());
		    if (n1>100) // это формат год-месяц-день, а иначе день-месяц-год
		    {
			    resDI.begin.nYear = resDI.end.nYear =  n1;
			    if (n2 > 12)
                {
                    resDI.begin.nMonth = resDI.end.nMonth = n3;
                    resDI.begin.nDay =  resDI.end.nDay = n2;
                }
			    else
                {
                    resDI.begin.nMonth = resDI.end.nMonth = n2;
                    resDI.begin.nDay =  resDI.end.nDay = n3;
                }
		    }
            else
		    {
			    if (n3 < 100) 
			    {
				    if (n3 < 20)
                        n3 += 2000;
                    else
                        n3 += 1900;
			    }
			    resDI.begin.nYear = resDI.end.nYear = n3;
			    if (n2 > 12)
                {
                    resDI.begin.nMonth = resDI.end.nMonth = n1;
                    resDI.begin.nDay =  resDI.end.nDay = n2;
                }
			    else
                {
                    resDI.begin.nMonth = resDI.end.nMonth = n2;
                    resDI.begin.nDay =  resDI.end.nDay = n1;
                }
		    }
        }
        else
            isUsed = false;
	}
    else if (((patternType==L"DMW" && dateUnits.size()>1) || patternType==L"DMW2" || patternType==L"DMW1") && (numUnitsWithCombi.size()>0) && dateUnits.size()>0)
    // частичная дата типа 13 сентября
	{
		IUnit* pDay= NULL;
		pDay = *(numUnitsWithCombi.begin());	
		//IUnit* pDay = *(numUnits.begin());		
		IUnit* pMonth = NULL;
		if (patternType==L"DMW")
			pMonth = *(++dateUnits.begin());
        else
			pMonth = *(dateUnits.begin());
		wstring strDay = pDay->GetWord();
		int n1 = StrToIntW(strDay.c_str());
		if (n1 == 0)
            n1 = pDay->GetHashKey();

		wstring strNorm = m_pUnitOperator->GetMainFormText(pMonth, SS::Core::Features::Values::GenderType.gtMasculine);
		wstring strNormLo = m_pUnitOperator->GetLowerWord(strNorm);
		if (inputInt.begin.nYear==0)
			resDI.begin.nYear = resDI.end.nYear = this->m_pBaseDate->nYear;
		else
			resDI.begin.nYear = resDI.end.nYear = inputInt.begin.nYear;
		resDI.begin.nMonth = resDI.end.nMonth =  GetNumberForMonth(strNormLo);
		resDI.begin.nDay = resDI.end.nDay = n1;
	}
    else if (patternType==L"DMYW2")
	{
		resDI.begin = inputInt.begin;
		resDI.end = inputInt.end;
	}
    else if ((patternType==L"DMY" || patternType==L"DMY4" || patternType==L"DMY3" || patternType==L"DMYW" || patternType==L"DMYW1") && 
        (numUnitsWithCombi.size()>=2) && dateUnits.size()>0)
	{
		IUnit* pNum1 =  *(numUnitsWithCombi.begin());
		IUnit* pNum2 =  *(++numUnitsWithCombi.begin());
		int n1 = StrToIntW(pNum1->GetWord());
		if (n1 == 0)
            n1 = pNum1->GetHashKey();
		int n2 = StrToIntW(pNum2->GetWord());
		if (n2 == 0)
            n2 = pNum2->GetHashKey();
		if (patternType==L"DMY" || patternType==L"DMY3" || patternType==L"DMYW" )
		{
			resDI.begin.nDay = resDI.end.nDay = n1; 
			resDI.begin.nYear = resDI.end.nYear  = n2; 
		}
        else
		{
			resDI.begin.nDay = resDI.end.nDay = n2; 
			resDI.begin.nYear = resDI.end.nYear  = n1; 
		}
		IUnit* pMonth = 	*(dateUnits.begin());  
		if (patternType==L"DMYW" && dateUnits.size()>1)
            pMonth = *(++dateUnits.begin());  
		wstring strNorm = m_pUnitOperator->GetMainFormText(pMonth, SS::Core::Features::Values::GenderType.Undefined);
		wstring strNormLo = m_pUnitOperator->GetLowerWord(strNorm);	
		resDI.begin.nMonth = resDI.end.nMonth = GetNumberForMonth(strNormLo);	  
	}
    else if (patternType==L"DMY4" && (numUnitsWithCombi.size()>=2) && dateUnits.size()>0)
	{
		IUnit* pNum1 =  *(numUnitsWithCombi.begin());
		IUnit* pNum2 =  *(++numUnitsWithCombi.begin());
		int n1 = StrToIntW(pNum1->GetWord());
		if (n1 == 0)
            n1 = pNum1->GetHashKey();
		int n2 = StrToIntW(pNum2->GetWord());
		if (n2 == 0)
            n2 = pNum2->GetHashKey();
		resDI.begin.nDay = resDI.end.nDay = n2; 
		resDI.begin.nYear = resDI.end.nYear  = n2; 
		IUnit* pMonth = *(dateUnits.begin());  
		wstring strNorm = m_pUnitOperator->GetMainFormText(pMonth, SS::Core::Features::Values::GenderType.gtMasculine);
		wstring strNormLo = m_pUnitOperator->GetLowerWord(strNorm);	
		resDI.begin.nMonth = resDI.end.nMonth = GetNumberForMonth(strNormLo);	  
	}
	return resDI;
}

CDateInterval  CEntity::GetValueForDateForUnit(IUnit* pParentUnit,set<IUnit*>& stopUnits)
{
	SS::Core::Features::CMorphoFeature oMorfoPattern;
	oMorfoPattern.Undefine();
	oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;	

	CDateInterval resDI;
	IUnit *pUnit = pParentUnit->GetLeftChildUnit();
	wstring str = pParentUnit->GetWord();
	if (pUnit == NULL && this->extDateRootUnit==NULL) // дочерних уже нет. - пытаемся вычислить интервал
	{
		resDI = GetNormalForSimpleDate(pParentUnit,resDI,stopUnits);
		if (resDI.isBad())
			resDI.Clear();
		return resDI;
	}
	else
	{
		// запускаем цикул по всем дочерним. Получаем множство интервалов. Все непустые пересекаем
		// далее пыатемся уточнить дату с учетом данного узла и полученного интервала
		list<CDateInterval> lstInt;
		while(pUnit!=NULL)
		{
			if (str.size()==0 && !pUnit->HasChilds())
            {
                pUnit = pUnit->GetRightUnit();
                continue;
            }
			if (this->m_pUnitOperator->IsParentHavePoint(pUnit) || this->m_pUnitOperator->IsHavePoint(pUnit))
            {
                pUnit = pUnit->GetRightUnit();
                continue;
            }
			CDateInterval childInt = GetValueForDateForUnit(pUnit,stopUnits);		  
			if (childInt.begin.nDay != 0) 
			{
				if (stopUnits.find(pUnit)==stopUnits.end())
                    stopUnits.insert(pUnit);
				lstInt.push_back(childInt);
			}
			pUnit = pUnit->GetRightUnit();
		}
		if (extDateRootUnit!=NULL && lstInt.size()==0)
		{
			IUnit* pExtUnit = extDateRootUnit;
			extDateRootUnit = NULL;
			CDateInterval childInt = GetValueForDateForUnit(pExtUnit,stopUnits);
			if (childInt.begin.nDay!=0) 
			{
				if (stopUnits.find(pUnit)==stopUnits.end())
                    stopUnits.insert(pExtUnit);
				lstInt.push_back(childInt);
			}
		}
		if (lstInt.size()==0)
		{
			resDI = GetNormalForSimpleDate(pParentUnit,resDI, stopUnits);
			if (resDI.isBad())
				resDI.Clear();
			return resDI;
		}
        else
		{
			resDI.begin.nYear = -1; resDI.end.nYear = 10000;
			for(list<CDateInterval>::iterator it = lstInt.begin(); it!= lstInt.end(); it++)
			{
				if ((*it).begin>resDI.begin)
                    resDI.begin = (*it).begin;
				if ((*it).end<resDI.end)
                    resDI.end = (*it).end;
			}
			if (!m_pUnitOperator->IsMorpho(pParentUnit, oMorfoPattern))
				resDI = GetNormalForSimpleDate(pParentUnit,resDI,stopUnits);

			if (resDI.isBad())
				resDI.Clear();
			return resDI;
		}
	}
}

int CEntity::GetBeginOffset()
{
	return (*unitList.begin())->GetFirstCharPositionNew();
}

int CEntity::GetEndOffset()
{
	TUnitList::iterator it = unitList.end();
	while(it != unitList.begin())
	{
		--it;
		int size = wcslen((*it)->GetWord());
		if (size==0) continue;
		return (*it)->GetFirstCharPositionNew()+ wcslen((*it)->GetWord());
	}	
	return (*unitList.begin())->GetFirstCharPositionNew();
}

int CEntity::GetEndOffsetNew(const wstring &srcText)
{
	TUnitList::iterator it = unitList.end();
	while(it != unitList.begin())
	{
		--it;
		wstring word = (*it)->GetWord();
		if (word.empty())
			continue;
		return (*it)->GetLastCharPositionNew(srcText.c_str());
	}	
	return (*unitList.begin())->GetFirstCharPositionNew();
}

wstring CEntity::AbbrNorm(IUnit* pUnit)
{
	wstring resStr = L"";
	wstring str = pUnit->GetWord();
	bool isHi = false;
	for(int i=str.size()-1; i>=0; i--)
	{
		if (iswlower(str[i]))
		{
			if (isHi) resStr = str[i] + resStr;		  
		}else
		{
			isHi = true;
			resStr = str[i] + resStr;		
		}
	}
	return resStr;
}

bool isPoint(wstring str)
{
	if (str[str.size()-1]==L'.')
		return true;
	else return false;
}


wstring CEntity::OKATONorm(IUnit* pUnit, SS::Core::Features::CMorphoFeature& oMorfoPatternGender, int num)
{	
	wstring strBase = m_pUnitOperator->GetLowerWord(pUnit->GetWord());
	if (strBase == L"с." || strBase==L"сел.")
		oMorfoPatternGender.m_GenderType = SS::Core::Features::Values::GenderType.gtNeutral;
	if (strBase == L"дер." || strBase==L"д.")
		oMorfoPatternGender.m_GenderType = SS::Core::Features::Values::GenderType.gtFemale;
	if (strBase == L"хут." || strBase==L"х." ||
		strBase == L"гор." || strBase==L"г." ||
		strBase == L"пос." || strBase==L"п.")
	{
		oMorfoPatternGender.m_GenderType = SS::Core::Features::Values::GenderType.gtMasculine;
	}

	wstring str = m_pUnitOperator->GetMainFormText1(pUnit, oMorfoPatternGender.m_GenderType).form;
	wstring strLo = m_pUnitOperator->GetLowerWord(str);
	if (num == 1)
	{
		if (strLo==L"деревня" || strLo==L"дер." || str==L"д.")
			return L"д";
		if (strLo==L"село" || strLo==L"селение" || strLo==L"сел." || str==L"с." )
			return L"с";
		if (strLo==L"поселок" || strLo==L"пос." || str==L"п.") 
			return L"п";
		if (strLo==L"хутор" || strLo==L"хут." || str==L"х.")
			return L"х";
	}
	/*SS::Core::Features::CMorphoFeature oMorfoPattern;
	oMorfoPattern.Undefine();
	oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;
	bool bAdj = pUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase() &&
		m_pUnitOperator->IsMorpho(pUnit, oMorfoPattern);
	if (strLo==L"мор") strLo = L"море";
	if (!bAdj && !m_pUnitOperator->IsProperUnknown(pUnit) &&
		(pUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst().GetDictionaryNumber() != SS::Dictionary::DATA_TYPE::NAMES::ednRProperName) &&
		strLo!=L"озеро" && strLo!=L"оз." && strLo!=L"река" && strLo!=L"р." && strLo!=L"водохранилище" && 
		strLo!=L"край" && str!=L"кр." &&
		strLo!=L"область" && strLo!=L"обл." && strLo!=L"район" && strLo!=L"пролив" && strLo!=L"залив" && strLo!=L"кладбище" && strLo!=L"царство" && strLo!=L"парк" && strLo!=L"заповедник" && strLo!=L"округ" &&
		strLo!=L"остров"  && strLo!=L"полуостров" && strLo!=L"континент" && strLo!=L"океан" &&
		strLo!=L"море" && strLo!=L"штат" && strLo!=L"федерация" && strLo!=L"союз" && ((strLo!=L"республика" && strLo!=L"королевство"  &&
		strLo!=L"государство" && strLo!=L"султанат" && strLo!=L"джамахерия" &&  strLo!=L"княжество") || num==1))
		return L"";*/
	return str;
}

void CEntity::GetGenderForEntList(TUnitList& unitList, SS::Core::Features::CMorphoFeature& oMorfoPattern)
{	
	oMorfoPattern.Undefine();
	int count = 1;
	for(TUnitList::iterator it = unitList.begin(); it != unitList.end(); ++it)
	{
		IUnit* pUnit = *it;
		wstring str = OKATONorm(pUnit, oMorfoPattern, count);
		if (!str.empty())
		{
			if (!m_pUnitOperator->IsProperUnknown(pUnit) &&
				(pUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst().GetDictionaryNumber() != SS::Dictionary::DATA_TYPE::NAMES::ednRProperName))
			{
				SS::Core::Features::CMorphoFeature oMorpho;
				m_pUnitOperator->GetUnitMorfoFeatures(pUnit, oMorpho);
				if (oMorfoPattern.IsUndefined())
					oMorfoPattern.Assign(&oMorpho);
				else if (oMorfoPattern.IsIntersectsWith(oMorpho))
					oMorfoPattern.AssignAND(&oMorpho);
			}
		}
		++count;
	}	
}

wstring CEntity::GetRegionNormalString()
{
	wstring resStr = L"";

	TUnitList lstVisUnits;
	bool isRus = true;
	int count = 1;
	int qQount = 0;
	SS::Core::Features::CMorphoFeature oMorfoPattern;
	oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;

	bool isNounFind = false;
	SS::Core::Features::CMorphoFeature oNoun;
	oNoun.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;
	SS::Core::Features::CMorphoFeature oAdjective;
	oAdjective.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;
	SMIList lstMIFin;

	for(TUnitList::iterator it = unitList.begin(); it != unitList.end(); ++it)
	{
		IUnit *pUnit = *it;
		wstring str = pUnit->GetWord();
		bool isParent = str.empty() || !pUnit->GetChildList().empty();
		bool isQuote = m_pUnitOperator->IsQuote(str);
		if (isQuote)
		{
			lstVisUnits.push_back(pUnit);
			++qQount;
		}
		else if (!isParent && !m_pUnitOperator->IsMorpho(pUnit, oMorfoPattern))
		{				
			if (m_lang == SS::Core::Types::ealEnglish)
				lstVisUnits.push_back(pUnit);
			else
			{
				SS::Core::Features::CMorphoFeature oMorpho;
				wstring okato = str;
				if (qQount % 2 == 0)
					okato = OKATONorm(pUnit, oMorpho, count);
				if (!okato.empty() || pUnit->IsEvristicAbbr())
					lstVisUnits.push_back(pUnit);
			}
		}
		++count;

		if (!isNounFind && m_pUnitOperator->IsMorpho(pUnit, oNoun))
		{
			isNounFind = true;
			m_pUnitOperator->GetUnitMorfoFeaturesFull(pUnit, oNoun, &lstMIFin);
			for(SMIList::iterator smiIt = lstMIFin.begin(); smiIt != lstMIFin.end(); ++smiIt)
				propList.push_back(*smiIt);
		}
		else if(!isNounFind && !isParent)
		{
			str = m_pUnitOperator->GetMainFormText1(pUnit, SS::Core::Features::Values::GenderType.Undefined).form;
			if (m_pUnitOperator->IsMorpho(pUnit, oAdjective))
				m_Adjectives.insert(str);
			m_strPrefix += str + L" ";
		}
	}

	if (lstVisUnits.size()<2 || m_lang==SS::Core::Types::ealEnglish)
	{
		// просто получаем нормальную форму слова
		for(TUnitList::iterator it = lstVisUnits.begin(); it != lstVisUnits.end(); ++it)
		{
			wstring str = (*it)->GetWord();
			bool isNominal = false;
			for (SMIList::iterator iter = propList.begin(); !isNominal && (iter != propList.end()); ++iter)
			{
				if ((*iter).caseType.IsContain(SS::Core::Features::Values::CaseType.ctNominative) ||
					(*iter).caseType.IsContain(SS::Core::Features::Values::CaseType.ctCommon))
				{
					isNominal = true;
				}
			}
			if (!isNominal || (!(*it)->IsEvristicAbbr() && !m_pUnitOperator->IsAbbr(*it)))
				str = m_pUnitOperator->GetMainFormText1(*it, SS::Core::Features::Values::GenderType.Undefined).form;
			resStr += str + L" ";
		}		
	}
	else		
	{
		// если первое слово не пропер, и слов больше двух, то нормализуем только первое слово, иначе все
		bool isAllNormal = false;		
		IUnit* pFirstUnit = *lstVisUnits.begin();		
		if (qQount>0 || (lstVisUnits.size()>2 && 
			!(pFirstUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()==SS::Dictionary::DATA_TYPE::NAMES::ednRProperName)
			&& !m_pUnitOperator->IsProperUnknown(pFirstUnit)))
		{
			isAllNormal = false; 
		}
		else
			isAllNormal = true;

		SS::Core::Features::CMorphoFeature oMorpho;
		GetGenderForEntList(lstVisUnits, oMorpho);

		if (isAllNormal)
		{
			count = 1;
			for(TUnitList::iterator it = lstVisUnits.begin(); it != lstVisUnits.end(); ++it)
			{
				IUnit* pUnit = *it;
				wstring str = OKATONorm(pUnit, oMorpho, count);
				resStr += str + L" ";
				++count;
			}
		}
		else
		{
			count = 1;
			for(TUnitList::iterator it = lstVisUnits.begin(); it != lstVisUnits.end(); ++it)
			{
				IUnit* pUnit = *it;
				wstring str = pUnit->GetWord();;
				if (it==lstVisUnits.begin() && !m_pUnitOperator->IsQuote(str))
					str = OKATONorm(pUnit, oMorpho, count);
				resStr += str + L" ";
				++count;
			}
		}
	}
	resStr = m_pUnitOperator->SpaceNormalize(resStr);
	resStr = m_pUnitOperator->QuoteNormalize(resStr);
	m_strPrefix = m_pUnitOperator->SpaceNormalize(m_strPrefix);
	m_strPrefix = m_pUnitOperator->QuoteNormalize(m_strPrefix);
	return resStr;
}

SS::Interface::Core::BlackBox::IUnit* CEntity::GetFirstUnit()
{
	if (unitList.size()==0) return NULL;
	return *unitList.begin();
}

SS::Interface::Core::BlackBox::IUnit* CEntity::GetLastUnit()
{
	if (unitList.size()==0) return NULL;
	return *(--unitList.end());
}

void  CEntity::ProcessValueForDate()
{
	CDateInterval resDI;
	TUnitList dateUnits;
	TUnitList numUnits;
	TUnitList numUnitsCombi;
	TUnitList numUnitsWithCombi;
	SS::Core::Features::CMorphoFeature oMorfoUnit;	
	SS::Core::Features::CSemanticFeature oSemanticUnit;	
	wstring strDayWeekModifier=L"";
	if (this->m_pBaseDate==NULL) return;
	set<IUnit*> stopUnits;
	m_DateInterval = GetValueForDateForUnit(this->rootUnit, stopUnits);		
}

CDate::CDate(wstring strDate)
{
	CDate();
	if (strDate.size()!=10) return;
	wstring str = L"";
	int i=0;
	for(i=0;i<4;i++)
	{
		str+=strDate[i];
	}
	nYear = StrToIntW((wchar_t*)str.c_str());
	str.clear();
	for(i=i+1;i<7;i++)
	{
		str+=strDate[i];
	}
	nMonth = StrToIntW((wchar_t*)str.c_str());
	str.clear();
	for(i=i+1;i<10;i++)
	{
		str+=strDate[i];
	}
	nDay = StrToIntW((wchar_t*)str.c_str());
}

wstring CDate::GetString()
{
	wchar_t szStr[11];
	swprintf(szStr,11,L"%04d-%02d-%02d",nYear,nMonth,nDay);
	return szStr;

}

void CDate::Decrement(int n)
{
	for(int i=0;i<n;i++)
	{
		if (nDay>1) nDay--; 
		else 
		{ 
			if (nMonth>1)  { nMonth--; nDay=GetLastDayForMonth(*this);}
			else
			{
				nYear--; nMonth = 12; nDay = 31;
			}					
		}
	}
}

void CDate::Increment(int n)
{
	for(int i=0;i<n;i++)
	{
		if (nDay<GetLastDayForMonth(*this)) nDay++; 
		else 
		{ 
			if (nMonth<12)  { nMonth++; nDay=1; }
			else
			{
				nYear++; nMonth = 1; nDay = 1;
			}					
		}
	}
}

int CDateInterval::GetLength()
{
	CDate b = begin;
	CDate e = end;
	if (begin<end)
	{
		int res = 0;
		while(b<e)
		{
			b.Increment(1);
			res++;
		}
		return res;
	}else return 0;
}

wstring CDateInterval::GetString()
{
	return begin.GetString()+L"/"+ end.GetString();
}

bool CEntity::IsEngGoodEnt()
{
	SS::Core::Features::CMorphoFeature oMorfoPattern;
	oMorfoPattern.Undefine();
	oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;		

	if (this->entType==entPersonName || this->entType==entPersonInfo)
	{		
		if (unitList.size()>1)
		{
			for(TUnitList::iterator it = unitList.begin(); it!= unitList.end(); it++)
			{
				wstring str = (*it)->GetWord();
				if (str.empty())
                    continue;
				if (m_pUnitOperator->IsPersonOnlyNameDict(*it) && !m_pUnitOperator->IsProperByUpper(*it))
                    return false;

			}
		}
        else
		{			
			if (!m_pUnitOperator->IsProperByUpper(*unitList.begin()))
                return false;
			else if (m_pUnitOperator->IsNominalEng(*unitList.begin()))
                entType = entUnknownProper;			
		}
	}
    else if (entType==entOrgName)
	{
		if (m_pUnitOperator->IsHavePoint(rootUnit))
            return false;
		int lowCount = 0;
		int nomCount = 0;
		int officialCount = 0;
		for(TUnitList::iterator it = unitList.begin(); it != unitList.end(); ++it)
		{
			wstring str = (*it)->GetWord();
			if (str.empty() || m_pUnitOperator->IsMorpho(*it, oMorfoPattern))
            {
                ++lowCount;
                ++officialCount;
                continue;
            }
			if (!m_pUnitOperator->IsProperByUpper(*it))
                ++lowCount;
			if (m_pUnitOperator->IsNominalDict(str))
                ++nomCount;
		}		
		if (lowCount==unitList.size() || (nomCount==1 && unitList.size()-officialCount==1))
            return false;
	}
	return true;
}

bool CEntity::IsHaveNecessaryNoun()
{
	if ((nType == eEntityBaseType::Ceterus) || (nType == eEntityBaseType::Adjective) || (entType == entDate) || (entType == entVerb) ||
		(entType == entPronoun) || (entType == entPhone) || (entType == entAdverb) || (entType==entEvent) || (entType == entProduct ))
	{
        return true;
	}

	SS::Core::Features::CMorphoFeature oMorfoPattern;
	oMorfoPattern.Undefine();
	oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;

	bool isHaveNoun = false;
	for(TUnitList::iterator it = unitList.begin(); it != unitList.end(); ++it)
	{
		if (m_pUnitOperator->IsMorpho(*it, oMorfoPattern))
		{
            isHaveNoun = true;
			break;
		}
	}
	if (unitList.size() == 1)
		isHaveNoun = isHaveNoun || rootUnit->IsEvristicAbbr();

    return isHaveNoun;
}

bool CEntity::IsUnknownHaveReduction()
{
	if ((entType == entUnknown) || (entType == entUnknownProper))
	{
		bool isEnglish = (m_lang == SS::Core::Types::ealEnglish);
		for(TUnitList::iterator it = unitList.begin(); it!= unitList.end(); ++it)
		{
			if (!isEnglish && m_pUnitOperator->IsParentHavePoint(*it))
				return false;
			if (IsReduction(*it))
				return false;
		}
	}
	return false;
}

bool CEntity::IsGoodEnt()
{
	bool isEnglish = (m_lang == SS::Core::Types::ealEnglish);
    wstring str = GetValue();
	if (unitList.empty() || str.empty())
		return false;
    if ((nType == eEntityBaseType::Ceterus) || (entType == entShortAdjective))
        return true;
	if (entType==entUnknown || entType==entUnknownProper)
	{
        if ((str == L"oc") || (str == L"oC") || (str == L"OC") ||
            (str == L"ос") || (str == L"оС") || (str == L"ОС") ||
            (str == L"pH") || (str == L"рН"))
        /// записаны на разных языках
        {
            return false;
        }
	}

    if (entType==entDate || entType==entVerb || entType==entPronoun || entType==entPhone || entType==entAdverb ||
		entType==entEvent || entType==entProduct || nType==Adjective)
	{
        return true;
	}

	IUnit *pFirst = unitList.front();
	wstring word = pFirst->GetWord();
	if ((pFirst->GetChildList().size() > 0) && !word.empty() && (word.back() == L'.'))
		return false;

	if (this->unitList.size()==1 && this->rootUnit->GetReferenceUnit())
        return true; // разрешение анафории
	
    str = this->rootUnit->GetWord();
	int count = 0;
	for(unsigned int i = 0; i < str.size(); ++i)
		if (::iswalnum(str[i]))
			++count;

	if (count==0 && !str.empty())
        return false;

    str = (*this->unitList.begin())->GetWordFull();
	if (str.size() > 3)
	{
		wstring str1 = str.substr(0, 4);
		if (str1==L"EUR " || str1==L"RUS " || str1==L"USD " || str1==L"P.S.")
			return false;
	}

	if (str.size() > 4)
	{
		wstring str1 = str.substr(0,5);
		if (str1 == L"P. S.")
			return false;
	}

	if (unitList.size() == 1)
	{
		if((rootUnit->IsInitial()) || (count <= 1))
			return false;

		if (str==L"потом" || str==L"том" || str==L"тому")
            return false;

		/*if (m_pUnitOperator->IsSemantic(*unitList.begin(),SS::Core::Features::Values::TSemanticType.smtMeasure))
			return false;*/
	}
	if (isEnglish && !IsEngGoodEnt())
		return false;
	
    return true;
}

// проверяет пересечение морфологии двух сущностей
bool CEntity::IsEntCaseCoincident(CEntity* pEnt)
{
	for(SMIList::iterator smiIt = propList.begin(); smiIt!=propList.end(); smiIt++)
	{
		for(SMIList::iterator smiIt1 = pEnt->propList.begin(); smiIt1!=pEnt->propList.end(); smiIt1++)
		{
			if (smiIt->caseType.IsIntersectsWithout(&smiIt1->caseType))
			{
				return true;
			}
		}
	}
	return false;
}


// проверяет перечесение семантики двух сущностей
bool CEntity::IsEntSemanticCoincident(CEntity* pEnt)
{
	bool bPersonThis = m_pUnitOperator->IsSemantic(rootUnit, SS::Core::Features::Values::TSemanticType.smtPerson);
	bool bPersonArg = m_pUnitOperator->IsSemantic(pEnt->rootUnit, SS::Core::Features::Values::TSemanticType.smtPerson);

	bool bOrgThis = m_pUnitOperator->IsSemantic(rootUnit, SS::Core::Features::Values::TSemanticType.smtOrganization);
	bool bOrgArg = m_pUnitOperator->IsSemantic(pEnt->rootUnit, SS::Core::Features::Values::TSemanticType.smtOrganization);

	bool bPlaceThis = m_pUnitOperator->IsSemantic(rootUnit, SS::Core::Features::Values::TSemanticType.smtPlace);
	bool bPlaceArg = m_pUnitOperator->IsSemantic(pEnt->rootUnit, SS::Core::Features::Values::TSemanticType.smtPlace);

	if ( (bPersonThis && bPersonArg) || (bOrgThis && bOrgArg) || (bPlaceThis && bPlaceArg) ||
		!(bPersonThis || bPersonArg || bOrgThis || bOrgArg || bPlaceThis || bPlaceArg))
	{
		return true;
	}
	return false;
}

vector<wstring> CEntity::GetAdjectives()
{
	return m_pUnitOperator->GetSetStrings(m_Adjectives);
}

void CEntity::AddAdjective()
{
}

vector<wstring> CEntity::GetMorphoProp()
{
	SS::Core::Features::CMorphoFeature oMorfoPattern;
	oMorfoPattern.Undefine();
	oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;	
	oMorfoPattern.m_TransitionalType = SS::Core::Features::Values::TransitionalType.tltTransitional;
	
	set<wstring> forms;
	bool isVerb = (entType == EEntityType::entVerb) && (GetVerbType() == L"Verb");
	for(SMIList::iterator smiIt = propList.begin(); smiIt != propList.end(); ++smiIt)
	{
		SMorphoInfo *pMorpho = &(*smiIt);
		int caseIndex = 0;
		int genderIndex = 0;
		int numberIndex = 0;
		int animationIndex = 0;
		int personIndex = 0;
		int voiceIndex = 0;
		vector<wstring> result;
		
		/// падеж
		unsigned char count = pMorpho->caseType.GetValuesNumber();
		int index = 0;
		int length = 0;
		if (pMorpho->caseType.IsIncludes(&SS::Core::Features::Values::CaseType.ctNominative))
			InsertInVector(result, length, L"N");
		if (pMorpho->caseType.IsIncludes(&SS::Core::Features::Values::CaseType.ctGenitive))
            InsertInVector(result, length, L"G");
		if (pMorpho->caseType.IsIncludes(&SS::Core::Features::Values::CaseType.ctAccusative))
			InsertInVector(result, length, L"A");
		if (pMorpho->caseType.IsIncludes(&SS::Core::Features::Values::CaseType.ctDative))
            InsertInVector(result, length, L"D");
		if (pMorpho->caseType.IsIncludes(&SS::Core::Features::Values::CaseType.ctInstrumental))
            InsertInVector(result, length, L"I");
		if (pMorpho->caseType.IsIncludes(&SS::Core::Features::Values::CaseType.ctLocative))
            InsertInVector(result, length, L"L");
		if (pMorpho->caseType.IsIncludes(&SS::Core::Features::Values::CaseType.ctCommon))
		{
			InsertInVector(result, length, L"N");
			InsertInVector(result, length, L"G");
			InsertInVector(result, length, L"A");
			InsertInVector(result, length, L"D");
			InsertInVector(result, length, L"I");
			InsertInVector(result, length, L"L");
		}
		if ((count == 0) || pMorpho->caseType.IsIncludes(&SS::Core::Features::Values::CaseType.Undefined))
            InsertInVector(result, length, L"U");

		/// род
		index = 0;
		length = result.size();
		if (pMorpho->genType.IsIncludes(&SS::Core::Features::Values::GenderType.gtMasculine))
		{
			UpdateVector(result, L"M");
			++index;
		}
		if (pMorpho->genType.IsIncludes(&SS::Core::Features::Values::GenderType.gtFemale))
		{
			if (index == 0)
				UpdateVector(result, L"F");
			else
				InsertInVector(result, length, L"F");
			++index;
		}
		if (pMorpho->genType.IsIncludes(&SS::Core::Features::Values::GenderType.gtNeutral))
		{
			if (index == 0)
				UpdateVector(result, L"N");
			else
				InsertInVector(result, length, L"N");
            ++index;
		}
		if (pMorpho->genType.IsIncludes(&SS::Core::Features::Values::GenderType.gtGeneral))
		{
			if (index == 0)
				UpdateVector(result, L"M");
			else
				InsertInVector(result, length, L"M");
			InsertInVector(result, length, L"F");
			InsertInVector(result, length, L"N");
            ++index;
		}
		if ((index == 0) || pMorpho->genType.IsIncludes(&SS::Core::Features::Values::GenderType.Undefined))
		{
			if (index == 0)
				UpdateVector(result, L"U");
			else
				InsertInVector(result, length, L"U");
		}

		/// число
		index = 0;
		length = result.size();
		if (pMorpho->numberType.IsIncludes(&SS::Core::Features::Values::NumberType.ntPlural))
		{
			UpdateVector(result, L"P");
            ++index;
		}
		if (pMorpho->numberType.IsIncludes(&SS::Core::Features::Values::NumberType.ntSingle) || (entType == entPersonName))
		{
			if (index == 0)
				UpdateVector(result, L"S");
			else
				InsertInVector(result, length, L"S");
            ++index;
		}
		if ((index == 0) || pMorpho->numberType.IsIncludes(&SS::Core::Features::Values::NumberType.Undefined))
		{
			if (index == 0)
				UpdateVector(result, L"U");
			else
				InsertInVector(result, length, L"U");
		}

		if (isVerb)
		/// переходность для глаголов
		{
			if (m_pUnitOperator->IsMorpho(rootUnit, oMorfoPattern))
				UpdateVector(result, L"T");
			else
                UpdateVector(result, L"N");
		}
		else
		/// одушевленность
		{
			index = 0;
			length = result.size();
			switch (nType)
			{
				case eEntityBaseType::Person:
					UpdateVector(result, L"A");
					break;
				case eEntityBaseType::Organization:
				case eEntityBaseType::Place:
					UpdateVector(result, L"I");
					break;
				default:
					if ((nType == eEntityBaseType::Pronoun) &&
						(pMorpho->personType.IsIncludes(&SS::Core::Features::Values::PersonType.ptFirstFace) ||
						pMorpho->personType.IsIncludes(&SS::Core::Features::Values::PersonType.ptSecondFace)))
					{
						UpdateVector(result, L"A");
					}
					else if (pMorpho->animationType.IsIncludes(&SS::Core::Features::Values::AnimInAnimType.aiaAnimation))
					{
						UpdateVector(result, L"A");
			            ++index;
					}
					if(pMorpho->animationType.IsIncludes(&SS::Core::Features::Values::AnimInAnimType.aiaInanimation))
					{
						if (index == 0)
							UpdateVector(result, L"I");
						else
							InsertInVector(result, length, L"I");
						++index;
					}
					if ((index == 0) || pMorpho->animationType.IsIncludes(&SS::Core::Features::Values::AnimInAnimType.Undefined))
					{
						if (index == 0)
							UpdateVector(result, L"U");
						else
							InsertInVector(result, length, L"U");
					}
			}
		}

		/// лицо
		index = 0;
		length = result.size();
		if(pMorpho->personType.IsIncludes(&SS::Core::Features::Values::PersonType.ptFirstFace))
		{
			UpdateVector(result, L"F");
	        ++index;
		}
		if(pMorpho->personType.IsIncludes(&SS::Core::Features::Values::PersonType.ptSecondFace))
		{
			if (index == 0)
				UpdateVector(result, L"S");
			else
				InsertInVector(result, length, L"S");
            ++index;
		}
		if(pMorpho->personType.IsIncludes(&SS::Core::Features::Values::PersonType.ptThirdFace))
		{
			if (index == 0)
				UpdateVector(result, L"T");
			else
				InsertInVector(result, length, L"T");
            ++index;
		}
		if ((index == 0) || pMorpho->personType.IsIncludes(&SS::Core::Features::Values::PersonType.Undefined))
		{
			if (index == 0)
				UpdateVector(result, L"U");
			else
				InsertInVector(result, length, L"U");
		}

		if (entType == EEntityType::entVerb)
		/// залог для глагольных сущностей
		{
			length = result.size();
			index = 0;
			if(pMorpho->voiceType.IsIncludes(&SS::Core::Features::Values::VoiceType.vtActive))
			{
				UpdateVector(result, L"A");
				++index;
			}
			if(pMorpho->voiceType.IsIncludes(&SS::Core::Features::Values::VoiceType.vtPassive))
			{
				if (index == 0)
					UpdateVector(result, L"P");
				else
					InsertInVector(result, length, L"P");
				++index;
			}
			if ((index == 0) || pMorpho->voiceType.IsIncludes(&SS::Core::Features::Values::VoiceType.Undefined))
			{
				if (index == 0)
					UpdateVector(result, L"U");
				else
					InsertInVector(result, length, L"U");
			}
		}

		for (vector<wstring>::iterator it = result.begin(); it != result.end(); ++it)
		{
			wstring str = *it;
			if (forms.find(str) == forms.end()) 
				forms.insert(str);
		}
	}
	
	return m_pUnitOperator->GetSetStrings(forms);
}

wstring CEntity::GetMorphoPropStr()
{
	wstring res = L"";
	vector<wstring> prop = GetMorphoProp();
	for (vector<wstring>::iterator it = prop.begin(); it != prop.end(); ++it)
		res += (*it) + L",";
	
	if (res.size()>1 && res[res.size() - 1]==L',')
		res = res.substr(0, res.size() - 1);

	return res;
}

void CEntity::UpdateVector(vector<wstring> &collection, wstring value)
{
	for (vector<wstring>::iterator it = collection.begin(); it != collection.end(); ++it)
		(*it) += value;
}

void CEntity::InsertInVector(vector<wstring> &collection, int length, wstring value)
{
	if (length == 0)
		collection.push_back(value);
	for (int i = 0; i < length; ++i)
	{
		wstring oldValue = collection[i];
		collection.push_back(oldValue.substr(0, oldValue.length() - 1) + value);
	}
}

wstring CEntity::GetVerbType()
{
	SS::Core::Features::CMorphoFeature oMorfoParticiple;
	oMorfoParticiple.Undefine();
	oMorfoParticiple.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;	
	oMorfoParticiple.m_VerbForm = SS::Core::Features::Values::VerbForm.vfParticiple;

	SS::Core::Features::CMorphoFeature oMorfoShort;
	oMorfoShort.Undefine();	
	oMorfoShort.m_ShortType = SS::Core::Features::Values::ShortType.stShort;

	SS::Core::Features::CMorphoFeature oMorfoDeepr;
	oMorfoDeepr.Undefine();
	oMorfoDeepr.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;	
	oMorfoDeepr.m_VerbForm = SS::Core::Features::Values::VerbForm.vfDeepr;

	int verbCount = 0;
	for (TUnitList::iterator it = unitList.begin(); it != unitList.end(); ++it)
	{
		IUnit *pUnit = *it;
		IUnit *pParent = pUnit->GetParentUnit();
		if (!pParent || !m_pUnitOperator->IsUnitChild(pParent, pUnit))
			++verbCount;
	}

	if (verbCount > 1)
		return L"Verb";
	else
	{
		SS::Core::Features::CMorphoFeature oMorpho;
		m_pUnitOperator->GetUnitMorfoFeaturesFull(rootUnit, oMorpho);
		if (oMorpho.m_VerbForm.IsContain(SS::Core::Features::Values::VerbForm.vfParticiple) &&
			!oMorpho.m_ShortType.IsContain(SS::Core::Features::Values::ShortType.stShort))
		{
			return L"Participle";
		}
		else if (oMorpho.m_VerbForm.IsContain(SS::Core::Features::Values::VerbForm.vfDeepr))
			return L"Deepr";
		else if (oMorpho.m_VerbForm.IsContain(SS::Core::Features::Values::VerbForm.vfInfinitive))
			return L"Infinitive";
		else
			return L"Verb";
	}
}

int  CEntity::GetNominalNounCount()
{
	return m_pUnitOperator->GetNominalNounCount(unitList);
}


// возаращает истину, если цепочка есть органиация типа "Внешэкономбанк"
bool CEntity::IsSingleOrgInQuote()
{
	bool result = false;
	if (entType==entOrgName && unitList.size()==4)
	{
		for(TUnitList::iterator itUnit = unitList.begin(); itUnit != unitList.end(); itUnit++)
		{
			wstring str = (*itUnit)->GetWord();
			if (str.empty()) continue;
			if (m_pUnitOperator->IsSemantic(*itUnit, SS::Core::Features::Values::TSemanticType.smtOrganization))  { result = true; break; }
		}
	}

	return result;
}

bool CEntity::IsMorpho(SS::Core::Features::CMorphoFeature oMorfoPattern)
{
	return m_pUnitOperator->IsMorpho(rootUnit, oMorfoPattern);
}

// повзоляет среди организаций выйвить организации типа Министертво туризма и отделить их от компания Газпром
bool CEntity::IsOgrNameWithoutNameGroup()
{
	IUnit* pUnit = GetLastUnit();	
	if (entType!=entOrgName || m_pUnitOperator->IsProperDict(pUnit) ||
		m_pUnitOperator->IsQuote(pUnit->GetWord()))
		return false;
	else
		return true;
}

bool CEntity::IsUnitInEntity(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	bool bResult = false;
	for(TUnitList::iterator itUnit = unitList.begin(); itUnit != unitList.end(); itUnit++)
	{
		if (*itUnit == pUnit) { bResult = true; break; }
	}
	return bResult;
}

void CEntity::DeleteUncorrectUnitsFromBorders()
{
	SS::Core::Features::Types::OfficialType official = SS::Core::Features::Values::OfficialType.otConjunction;
	official.AssignOR(&SS::Core::Features::Values::OfficialType.otPretext);
    official.AssignOR(&SS::Core::Features::Values::OfficialType.otSymbol);

	TUnitList unitListForDeleting;
	for(TUnitList::reverse_iterator it = unitList.rbegin(); it != unitList.rend(); ++it)
    {
		IUnit *pUnit = *it;
        SS::Core::Features::CMorphoFeature oMorfo;
        m_pUnitOperator->GetUnitMorfoFeaturesFull(pUnit, oMorfo);
		wstring word = pUnit->GetWord();
        if (oMorfo.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postOfficial) &&
			(oMorfo.m_OfficialType.IsIntersectsWith(&official) || 
			oMorfo.m_OfficialType.IsContain(SS::Core::Features::Values::OfficialType.otDeterminers)) &&
			!m_pUnitOperator->IsQuote(word) && ((word != L")") && (word != L"]") && (word != L"}")))
		{
			if (word == L".")
			{
				/*TUnitList::reverse_iterator prev = it;
				++prev;
				if ((*prev)->IsInitial())*/
					continue;
			}
			IUnit *pParent = pUnit->GetParentUnit();
			if (pParent && m_pUnitOperator->IsUnitChild(pParent, pUnit))
				break;
			unitListForDeleting.push_back(*it);
		}
		else
			break;
    }
	for(TUnitList::iterator it = unitList.begin(); it != unitList.end(); ++it)
	{
		SS::Core::Features::CMorphoFeature oMorfo;
        m_pUnitOperator->GetUnitMorfoFeaturesFull(*it, oMorfo);
		wstring word = (*it)->GetWord();
        if (oMorfo.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postOfficial) &&
			oMorfo.m_OfficialType.IsIntersectsWith(&official) && !m_pUnitOperator->IsQuote(word))
		{
            unitListForDeleting.push_back(*it);
		}
		else
			break;
	}

	for(TUnitList::iterator it = unitListForDeleting.begin(); it != unitListForDeleting.end(); ++it)
		unitList.remove(*it);
}

void CEntity::AddAdjectiveHomogeneousPrefix(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	unitList.push_front(pUnit);
	m_lstHomogeneousAdjUnits.push_back(pUnit);
}

void CEntity::AddUnitsToList(TUnitList list)
{
    for(TUnitList::iterator it = list.begin(); it != list.end(); ++it)
        unitList.push_back(*it);
}

bool CEntity::IsReduction(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
    bool isPossibleReduction = false;
	wstring word = pUnit->GetWord();
	if (!word.empty() && (word != L"."))
		isPossibleReduction = CDictionaryPool::IsFindReduction(word);
    return isPossibleReduction;
	/*IUnit* pRightUnit = pUnit->GetRightUnit();
	if (isPossibleReduction && pRightUnit)
	{
		wstring strWord = pRightUnit->GetWord();		
		if (strWord == L".")
		{
			return true;
		}
	}*/
	//return false;
}

void CEntity::AddSubEntity(CEntity *pSubEntity)
{
	if (!pSubEntity)
		return;
	if ((pSubEntity->m_lang == SS::Core::Types::EAnalyseLanguage::ealEnglish) &&
		(pSubEntity->unitList.size() == 1))
	/// отсеим сущность с единственным юнитом на "ing"
	{
		SS::Core::Features::CMorphoFeature oMorfoVerbParticipleING;
		oMorfoVerbParticipleING.Undefine();
		oMorfoVerbParticipleING.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
        oMorfoVerbParticipleING.m_TimeType = SS::Core::Features::Values::TimeType.tmtPresent;
        oMorfoVerbParticipleING.m_VerbForm = SS::Core::Features::Values::VerbForm.vfParticiple;

		wstring word = m_pUnitOperator->GetLowerWord(pSubEntity->rootUnit->GetWord());
		if (m_pUnitOperator->IsMorpho(pSubEntity->rootUnit, oMorfoVerbParticipleING) || (word == L"the"))
			return;
	}
	m_lstSubEntites.push_back(pSubEntity);
}