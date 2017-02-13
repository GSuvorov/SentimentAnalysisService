#ifndef USEFULL_STR
#define USEFULL_STR

#include <string>
#include <set>
#include <vector>
using std::string;
using std::vector;
using std::set;


namespace  UseString 
{

	inline int GetStr( FILE *f, char* pszStr )
	{
		int bFileContinued = 1;
		char c, *pszPos = pszStr;
		while( ((bFileContinued = fread( &c, 1, 1, f )) == 1) && (c != 13) && (c != 10) && (pszPos-pszStr) < 1023 )
			*(pszPos++) = c;
		*pszPos = '\0';
		return bFileContinued ? (int)(pszPos - pszStr) : -1;
	}


	inline int SplitStr( const char* pszStr, set<char> &mBreak, vector<string> &vsString, bool bQuoteBreak = false )
	{
		vsString.clear();
		const char *pszPos = pszStr;
		string sWord;
		while( *pszPos )
		{
			if( mBreak.find( *pszPos ) != mBreak.end() )	//if( (*pszPos == ' ') || (*pszPos == '\t') )
			{
				if( sWord.length() )
					vsString.push_back( sWord ), sWord.resize( 0 );
			}else
				sWord += *pszPos;
			pszPos ++;
		}  
		if( sWord.length() )	vsString.push_back( sWord );


		if( bQuoteBreak )
		{
			std::vector<string>::iterator iW = vsString.begin();
			while(  iW != vsString.end() )
			{
				if( iW->length() > 1 && (*iW)[0] == '\"' )
				{
					iW->assign( *iW, 1, iW->length()-1 ); 
					iW = vsString.insert( iW, string( "\"" ) );  
					iW ++;
				}

				if( iW->length() > 1 && (*iW)[iW->length()-1] == '\"' )
				{
					iW->assign( *iW, 0, iW->length()-1 ); 
					iW ++;
					iW = vsString.insert( iW, string( "\"" ) );  
				}
				iW ++;
			}
		}

		return 0;
	}

	inline void SpaceNormalizeStr( char* pszStr, string& sNormalStr )
	{
		vector<string> vsString;
		set<char> Break; Break.insert( ' ' ); Break.insert( '\t' );
		SplitStr( pszStr, Break, vsString );		
		for( std::vector<string>::iterator iS = vsString.begin(); iS != vsString.end(); iS ++ )
		{
			if( iS != vsString.begin() )
				sNormalStr += " ";
			sNormalStr += *iS;
		}
	}

};  // namespace  UseString 



#endif // USEFULL_STR