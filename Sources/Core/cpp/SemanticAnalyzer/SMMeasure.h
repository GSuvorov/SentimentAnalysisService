#pragma once
#include "smsentenceexpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{

///выделение обстоятельств меры 
class CSMMeasure :
	public CSMSentenceExpert
{
public:
	CSMMeasure(void);
	~CSMMeasure(void);
	bool AnalizeSentence();
	bool Init(IText * pIText);
private:

	set<long> * m_s_Time;
	bool m_bManyMeasureUnits;

	/// истина, если юнит имеет семантику времени
	bool IsSemTime();
	
	/// добавляет числа спереди
	/** 
		\param l_Units - контейнер
		\return 	bool - истина - если добавил 
	*/
	bool AddDigitsFront(TBoxUnits & l_Units);
	/// добавляет числа сзади
	/** 
		\param l_Units - контейнер
		\return 	bool - истина - если добавил
	*/
	bool AddDigitsBack(TBoxUnits & l_Units);
	/// объединяет группу юнитов , даёт ей категорию меры и семантику
	/** 
		\param l_Units - группа
		\param oMeasure - семантика
	*/
	void ProcessMeasure(TBoxUnits & l_Units, Types::TSMeasureType oMeasure);

	/// выделяет конструкции типа end of и объединяет в группу
	bool AddOfFront(TBoxUnits & l_Units);
	/// объединяет группу юнитов , даёт ей категорию времени и  фильтрует семантику
	/** 
	\param l_Units - группа
	*/	
	void ProcessTimeFixedPoint(TBoxUnits & l_Units, IUnit * pUnit);
	
	/// истина, если юнит имеет семантику числа
	bool IsSemNumber(IUnit * pUnit=NULL);

	/// обработка c семантикой времени
	void TimeProcess();

	/// обработка c семантикой времени и меры
	void MeasureTimeProcess();
	
	/// обработка c семантикой меры
	void MeasureProcess();

	/// обработка c семантикой меры
	void MeasureProcessEx();

	/// истина, если проставлен индекс AjunctMeasure
	bool IsSetMeasure();

	bool SubTimeProcess();

	bool AddOfBack(TBoxUnits & l_Units);

	void MoveToAjunctTime();

	/// объединяет группу юнитов , даёт ей категорию времени и выставляет семантику undef
	void SecondProcessTimeFixedPoint(TBoxUnits & l_Units);

	bool CheckAimWords();
	bool IsTire();
	bool FindFirstTire();
	bool IsFindNumberSemTime(TBoxUnits & l_Units);

};


		}
	}
}