#pragma once

#include "..\ASSInterface\IBlackBox.h"
using namespace SS::Interface::Core::BlackBox;
using namespace SS::Interface::Core::BlackBox::Query;

#include "..\FeatureLibrary\define.h"
using namespace SS::Core::Features::Types;

#include "..\ASSInterface\TMainAnalyseTypes.h"
using namespace SS::Core::Types;

#include "..\ASSInterface\TCoSeDiDataTypes.h"
using namespace SS::CoSeDi::DataTypes;

#include "..\ASSInterface\ICoSeDi.h"
using namespace SS::Interface::Core::CoSeDi;


#include "Group.h"

#include <list>
#include <vector>
using namespace std;


namespace SS
{
namespace TransformGenerator
{

/// Создает трансформации исходя из заданной последовательности групп
class CTransform
{
public:
	CTransform(ISemanticAnalysis* pSemanticAnalysis, IBlackBoxTextStorage* pTextStorage,
			IBlackBoxQueryStorage* pQueryStorage, ICombinatory* pCombinatory);
	~CTransform();

public:
	/// Возвращает true если 2 трансформации совпадают
	bool Compare(CTransform* pTransform);

	/// Добавляет синтаксическую категорию к трансформации
	void AddSC(int sc);

	/// Добавляет группу к трансформации, в зависимости от ее синтаксической категории
	void AddGroup(IUnit* pUnit);

	/// Генерирует трансформацию на основе первого семантического класса
	ITransformation* GetFirstTransform(EAnalyseLanguage eLanguage, const SUnitParams& oParams);

	/// Генерирует трансформацию на основе последующих семантических классов
	ITransformation* GetNextTransform();

	/// Устанавливает вес, который будет присвоен всем генерируемым трансформациям
	void SetWeight(float fWeight);

private:
	/// vector of a unit's combinatory classes
	typedef vector<TDictionaryIndex> TClasses;
	struct SCombiUnit
	{
		IUnit* pUnit;
		TClasses vClasses;
		bool bMerge;
	};

	/// list of units in a group that contain combinatory indexes
	typedef list<SCombiUnit*> TCombiUnits;
	struct SGroupItem
	{
		~SGroupItem()
		{
			for(TCombiUnits::iterator it = lCombiUnits.begin(); it != lCombiUnits.end(); ++it)
				delete *it;
		}

		int sc;
		IUnit* pUnit;
		TCombiUnits lCombiUnits;
	};

	/// list of all groups in a transformation
	typedef list<SGroupItem*> TItemList;
	class CItemList
	{
	public:
		~CItemList()
		{
			for(TItemList::iterator it = m_lSeq.begin(); it != m_lSeq.end(); ++it)
				delete *it;
		}

	public:
		void push_back(SGroupItem* pItem)
		{
			m_lSeq.push_back(pItem);
		}

		TItemList::iterator begin()
		{
			return m_lSeq.begin();
		}

		TItemList::iterator end()
		{
			return m_lSeq.end();
		}

		TItemList::iterator FirstValid()
		{
			TItemList::iterator it = m_lSeq.begin();
			if(!(*it)->pUnit)
				NextValid(it);
			return it;
		}

		void NextValid(TItemList::iterator& it)
		{
			for(++it; it != m_lSeq.end(); ++it)
			{
				if((*it)->pUnit)
					break;
			}
		}

	private:
		TItemList m_lSeq;
	};

private:
	void RebuildGroup(IUnit* pTopUnit);

	ITransformation* GenerateTransformation();
	void FillSemanticClasses();
	void FillSemanticClasses(SGroupItem* pItem, IUnit* pUnit);
	IGroup* GetGroup(SGroupItem* pItem);
	void AddClassToCombination(SCombiUnit* pCombi, int iNum, bool bRemoveOldUnits);
	void FillUnit(IUnit* pUnit, CoSeDi::DataTypes::SWordIndex* pInfo);

private:
	CItemList m_lSeq;
	int m_iMax;
	int m_iCurrClass;
	EAnalyseLanguage m_eLanguage;
	SUnitParams m_oParams;
	float m_fWeight;

	ISemanticAnalysis* m_pSemanticAnalysis;
	IBlackBoxTextStorage* m_pTextStorage;
	IBlackBoxQueryStorage* m_pQueryStorage;
	ICombinatory* m_pCombinatory;
};

typedef list<CTransform*> TTransformList;

/// Список объектов типа CTransform*
class CTrasfromList
{
public:
	CTrasfromList()
	{
	}

	~CTrasfromList()
	{
		for(TTransformList::iterator it = m_lst.begin(); it != m_lst.end(); it++)
			delete *it;
	}

public:
	void Init(int* pSeq, ISemanticAnalysis* pSemanticAnalysis, IBlackBoxTextStorage* pTextStorage,
		IBlackBoxQueryStorage* pQueryStorage, ICombinatory* pCombinatory)
	{
		float fNum = 1.;
		for(int i = 0; pSeq[i]; i++)
		{
			CTransform* pTrans = new CTransform(pSemanticAnalysis, pTextStorage, pQueryStorage, pCombinatory);
			pTrans->SetWeight(1. / fNum++);
			m_lst.push_back(pTrans);
			for(; pSeq[i]; i++)
				pTrans->AddSC(pSeq[i]);
		}
	}

	CTransform* GetFirstTransform()
	{
		m_it = m_lst.begin();
		if(m_it != m_lst.end())
			return *m_it;
		return NULL;
	}

	CTransform* GetNextTransform()
	{
		if(++m_it != m_lst.end())
			return *m_it;
		return NULL;
	}

private:
	TTransformList m_lst;
	TTransformList::iterator m_it;
};

}
}
