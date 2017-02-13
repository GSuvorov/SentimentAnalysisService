#pragma once

#include <vector>
#include <string>
#include <exception>


namespace SS
{
namespace Core
{
namespace BlackBox
{
namespace XML
{

///////////////////////////////// CTypeFeatureWrapper declaration ////////////////////////////

template< typename _TFeatureType, typename _TFeatureTypeType >
class CTypeFeatureWrapper
{
// class members
private:
	std::vector< _TFeatureType >	m_vcFeatures;
	bool							m_bInitFlag;	
	int								m_nCurIndex;
	std::string						m_strTypeDescription;
	std::string						m_strCurValDescription;
	std::string						m_strDescByIndex;
// construction / destruction
public:
	CTypeFeatureWrapper() : m_bInitFlag( false ),
							m_vcFeatures(),
							m_nCurIndex( 0 ),
							m_strTypeDescription(),
							m_strCurValDescription(),
							m_strDescByIndex()
	{}
	virtual ~CTypeFeatureWrapper()
	{}			
// class interface
	int GetValuesCount();
	bool InitValues();
	_TFeatureType* GetFirstValue();
	_TFeatureType* GetNextValue();
	char* GetTypeDescription();
	char* GetCurrentValueDescription();
	const char* GetDescriptionByIndex( int nIndex );
	char const* GetValueName( int nIndex );
};// CTypeFeatureWrapper

///////////////////////////////// CTypeFeatureWrapper implementation ////////////////////////////

template< typename _TFeatureType, typename _TFeatureTypeType >
int
CTypeFeatureWrapper< _TFeatureType, _TFeatureTypeType >::GetValuesCount()
{
	return _TFeatureTypeType().GetValueCount();
}

template< typename _TFeatureType, typename _TFeatureTypeType >
bool
CTypeFeatureWrapper< _TFeatureType, _TFeatureTypeType >::InitValues()
{
	if( true == m_bInitFlag ){
		return false;
	}
	// bufer size
	int nValues = GetValuesCount();
	if( 0 >= nValues ){
		return false;
	}
	// prepare vector
	m_vcFeatures.reserve( nValues );
	for( int nc = 0; nc < nValues; ++nc ){
		m_vcFeatures.push_back( _TFeatureType( nc ) );	
	}
	// set init flag
	m_bInitFlag = true;
	return m_bInitFlag;
}	
template< typename _TFeatureType, typename _TFeatureTypeType >
_TFeatureType*
CTypeFeatureWrapper< _TFeatureType, _TFeatureTypeType >::GetFirstValue()
{
	if( !GetValuesCount() ){
		return 0;
	}
	return &( m_vcFeatures.at( m_nCurIndex++ ) );
}

template< typename _TFeatureType, typename _TFeatureTypeType >
_TFeatureType*
CTypeFeatureWrapper< _TFeatureType, _TFeatureTypeType >::GetNextValue()
{
	try{			
		return &( m_vcFeatures.at( m_nCurIndex++ ) );
	}catch( std::out_of_range& ){
		return 0;
	}
}

template< typename _TFeatureType, typename _TFeatureTypeType >
char* 
CTypeFeatureWrapper< _TFeatureType, _TFeatureTypeType >::GetTypeDescription()
{
	m_strTypeDescription = _TFeatureTypeType().GetTypeDescription();
	return const_cast< char* >( m_strTypeDescription.c_str() );
}

template< typename _TFeatureType, typename _TFeatureTypeType >
char* 
CTypeFeatureWrapper< _TFeatureType, _TFeatureTypeType >::GetCurrentValueDescription()
{
	m_strCurValDescription = _TFeatureTypeType().GetValueDescription( m_nCurIndex - 1 );
	return const_cast< char* >( m_strCurValDescription.c_str() );
}

template< typename _TFeatureType, typename _TFeatureTypeType >
const char* 
CTypeFeatureWrapper< _TFeatureType, _TFeatureTypeType >::GetDescriptionByIndex( int nIndex )
{
	return _TFeatureTypeType().GetValueDescription( nIndex );
}

template< typename _TFeatureType, typename _TFeatureTypeType >
char const*
CTypeFeatureWrapper< _TFeatureType, _TFeatureTypeType >::GetValueName( int nIndex )
{
	return _TFeatureTypeType().GetValueName( nIndex );
}

}
}
}
}
