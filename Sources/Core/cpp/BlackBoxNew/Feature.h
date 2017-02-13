#pragma once

#include "LeafItem.h"

#include "..\ASSInterface\IBlackBoxFeature.h"
using namespace SS::Interface::Core::BlackBox;


namespace SS
{
namespace Core
{
namespace BlackBox
{

class CFeature : 
		public CLeafItem,
		public IFeature
{
public:
	CFeature();
	virtual ~CFeature();

public:
	virtual unsigned int GetMorphoIndex();
	virtual void SetMorphoIndex(unsigned int uiValue);
	virtual unsigned int GetMorphoInfo();
	virtual void SetMorphoInfo(unsigned int uiValue);
	virtual unsigned int GetPartOfSpeech();
	virtual void SetPartOfSpeech(unsigned int uiValue);
	virtual IFeature* ReleaseFeature();
	virtual void ClearFeature();

private:
	unsigned int m_uiMorphoIndex;
	unsigned int m_uiMorphoInfo;
	unsigned int m_uiPartOfSpeech;
};

}
}
}
