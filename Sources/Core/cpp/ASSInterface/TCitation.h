#pragma once

struct TCitation
{
	public:
    	void* pBeginCitata;
		void* pEndCitata;
		void* pSubjectUnit;
		int   nType;
		TCitation() : pBeginCitata(NULL), pEndCitata(NULL), pSubjectUnit(NULL)
		{
			nType = 0;
		}
};