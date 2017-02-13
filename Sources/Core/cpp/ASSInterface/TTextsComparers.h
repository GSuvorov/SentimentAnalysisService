#pragma once

#include <string>
#include <list>

using namespace std;

namespace TextsComaparers   {

	

	//typedef wstring                           TCategory;
	//typedef list< const wchar_t *>	         TTexts;
	//typedef list< pair< TCategory, TTexts > > TSampleTexts;

	struct TOneSampleText   {
		const wchar_t * pszText;
		const wchar_t * pszRubricName;
		const wchar_t * pszFilePath;
	};

	typedef list< TOneSampleText > TSampleTexts;

	struct TTextEstimate   {

		DOUBLE    fEstimate;
		wstring   sRubicName;
		wstring   sPathToFile;

		TTextEstimate() : fEstimate(0.0), sRubicName(L""), sPathToFile(L"") {}
	};

}