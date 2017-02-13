#pragma once

#include "../ASSInterface/ITextBlockEx.h"
#include "../ASSInterface/Constants.h"
#include "./LinguisticWorkingInterfaces.h"

namespace SS
{
namespace LinguisticProcessor
{
namespace Indexation
{

class CTableIndexation : virtual public SS::LinguisticProcessor::CLinguisticWorkingInterfaces
{
	std::list<SS::Interface::Core::BlackBox::IText*>				m_l_TextForTables;
	std::list< std::list<SS::Core::Types::IndexationSearch::TNativeTextForIndexation*> >::iterator m_II;
	
	std::list< std::list<SS::Core::Types::IndexationSearch::TNativeTextForIndexation*> > * m_p_l_l_NativeText;
public:
	CTableIndexation(void);
	~CTableIndexation(void);
public:
	/// инициирует процесс индексации очередного текстового блока очередного текста 
	void Init(
		std::list< std::list<SS::Core::Types::IndexationSearch::TNativeTextForIndexation*> > * p_l_l_NativeText
		/// параметры индексации текста
		, SS::Core::Types::IndexationSearch::TIndexationParams* pIndexationParams
		);
	unsigned int GenerateNext(
		/*in*/ unsigned int& uiFistCharPosition, 
		/*out*/unsigned int& uiPosition, 
		/*in*/SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult);

	void Clear();

};

}
}
}