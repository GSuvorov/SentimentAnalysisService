#pragma once

#include <string>
#include <list>

#include "../ASCInterface/TDictionaryIndex.h"

namespace SS
{
namespace Dictionary
{
namespace Types
{

/// полный индекс слова
struct SWordIndex
{
	TDictionaryIndex m_DictIndex;
	TIDForm          m_IDForm;
	SWordIndex() : m_IDForm(0)
	{}

	bool operator == (const SWordIndex & value) const	
	{
		return ((m_DictIndex.GetDictionaryIndex() == value.m_DictIndex.GetDictionaryIndex())&&
				(m_IDForm ==	value.m_IDForm)
				) ? true : false;
	}

	SWordIndex& operator= (const SWordIndex& value)
	{
		m_DictIndex.SetDictionaryIndex(value.m_DictIndex.GetDictionaryIndex());
		m_IDForm = value.m_IDForm;
		
		return *this;
	}
};

/// морфологическа€ информаци€ основы
struct SMorphoTypeInfo
{
	TIDType		m_IDType;
	TMorphoInfo m_MorphoInfo;
	SMorphoTypeInfo() : m_IDType(0), m_MorphoInfo(0)
	{}
};

/// содержит полную информацию об основе
struct SSource
{
	TDictionaryIndex  m_DictIndex;
	std::wstring	  m_wSource;
	SMorphoTypeInfo   m_MorphoTypeInfo;

};

/// содержит полную информацию о типе морфообразовани€
struct SMorphoType 
{
	TIDEnding		m_IDEnding;
	TIDForm			m_IDForm;
	TSpecialMorpho	m_SpecialMorpho;
	SMorphoTypeInfo m_MorphoTypeInfo;

	SMorphoType() : m_IDEnding(0), m_IDForm(0), m_SpecialMorpho(0)
	{}
};

/// полна€ морфологическа€ информаци€ о простом слове
struct SWordInfo  
{
	std::wstring	m_wWord;
	TSpecialMorpho	m_SpecialMorpho;
	SWordIndex      m_WordIndex; 
	SMorphoTypeInfo m_MorphoTypeInfo;
    bool            m_NewWord;

	SWordInfo():   m_SpecialMorpho(0), m_NewWord(0)
	{
	}

};

/// полна€ морфологическа€ информаци€ о составном слове
typedef std::list<SWordInfo> TWordInfo;


//ќпредил€ет область поиска в IMorphologyAnalyser
enum EFindZone
{
    /*  поиск во всех словар€х, включа€ словарь новых слов, если слово отсутсвует, добавить в словарь новых слов */
    efzFullAddNewWords,		
    
    /*	только поиск во всех словар€х, включа€ словарь новых слов, если слово отсутсвует, в словарь новых слов не добавл€ть*/
    efzOnlySearch,			
    
    /*	только поиск в только словаре аббревиатур */
    efzAbbreviation,		
    
    /* поиск в виртуальных и дереве */
    efzBaseDictionary,		
    
    /*	только поиск в словаре новых слов */
    efzNewWord,

    /* ѕоиск в дереве, аббревиатурах и виртуальных */
    efzBaseAndAbbreviation,
    
    /*  поиск во всех словар€х, включа€ словарь новых слов и определение составных слов, если слово отсутсвует, добавить в словарь новых слов */
    efzFullAddNewWordsCompound
};

}
}
}