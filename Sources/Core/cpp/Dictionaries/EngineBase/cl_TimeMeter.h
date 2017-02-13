////////////////////////////////////////////////////////////////////
//	Declaration of class <CSysMeter> that collects system statistic
//	NOTE: All time results are in MilliSeconds (1/1000 sec)
///////////////////////////

#pragma once

////////////////////
//	Class CSysMeter

class CSysMeter   {

	public: //

		///////////////////////
		// Default constructor
		CSysMeter();

		//////////////////////////////////////////
		// Constructor that calls Init() function
		CSysMeter(LONGLONG nInitialGlobalTime,
						LONGLONG nGoodMeteringsNum,
						LONGLONG nBadMeteringsNum);

		////////////////////
		// Empty dectructor
		~CSysMeter() { };

	public: //

		/////////////////////////////////////////////////////////////////////////////////////////
		// Sets initial values for <InitialGlobalTime>, <GoodMeteringsNum> and <BadMeteringsNum>
		void Init(LONGLONG nInitialGlobalTime = 0,
						LONGLONG nGoodMeteringsNum = 0,
						LONGLONG nBadMeteringsNum = 0);

		///////////////////
		// Starts metering
		void _Start(void);

		//////////////////
		// Stops metering
		// After this call results for functions Get...() are ready
		void _Stop(void);

		//////////////////////////////////////
		// Do we work with a good TimeMeter ?
		BOOL  GetActivityFlag(void) { return(m_TActivityFlag); }

		//////////////////////////////////
		// Returns global performance time
		// Return: LONGLONG value in MilliSeconds (1/1000 sec)
		LONGLONG GetGlobalTime(void);   // { return(m_TGlobal.QuadPart*1000i64/m_TFrequency.QuadPart); }
		CHAR *   GetGlobalTimeStr(void);

		/////////////////////////////////
		// Returns time of last metering
		// Return: LONGLONG value in MilliSeconds (1/1000 sec)
		LONGLONG GetLastOperTime(void); // { return(m_TLastOper.QuadPart*1000i64/m_TFrequency.QuadPart); }
		CHAR *   GetLastOperTimeStr(void);

		////////////////////////////////////
		// Returns number of good meterings
		// Return: LONGLONG value
		LONGLONG GetGoodMeteringsNum(void) { return(m_GoodMeteringsNum.QuadPart); }

		///////////////////////////////////
		// Returns number of bad meterings
		// Return: LONGLONG value
		LONGLONG GetBadMeteringsNum(void)  { return(m_BadMeteringsNum.QuadPart); }

	private: //

		// MEMORYSTATUS  m_MemStat;

		BOOL          m_TActivityFlag;
		BOOL          m_TStartedFlag;

		LARGE_INTEGER m_TGlobal;
		LARGE_INTEGER m_TLastOper;

		LARGE_INTEGER m_TStart;
		LARGE_INTEGER m_TStop;

		LARGE_INTEGER m_TFrequency;

		BOOL          m_ErrFlag;
		LARGE_INTEGER m_GoodMeteringsNum;
		LARGE_INTEGER m_BadMeteringsNum;
};
