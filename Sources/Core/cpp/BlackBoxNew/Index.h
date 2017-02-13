#pragma once

#include "RootIterateItem.h"
#include "DictionaryIndex.h"

#include "..\ASSInterface\IBlackBoxIndex.h"
using namespace SS::Interface::Core::BlackBox;


namespace SS
{
namespace Core
{
namespace BlackBox
{

class CIndex : 
		public CMiddleIterateItem,
		public IIndex
{
public:
	CIndex();
	virtual ~CIndex();

public:
	virtual void SetIndexationFlag(bool bl);
	virtual bool GetIndexationFlag();
	virtual IDictionaryIndex* GetDictionaryIndex();
	virtual void AddFeature(IFeature* pFeature);
	virtual IFeature* GetFirstFeature();
	virtual IFeature* GetNextFeature();
	virtual IIndex* Pop();
	virtual IIndex* ReleaseIndex();
	virtual void ClearIndex();

private:
	bool m_bIndexationFlag;
	CDictionaryIndex m_oDictionaryIndex;
};

}
}
}
