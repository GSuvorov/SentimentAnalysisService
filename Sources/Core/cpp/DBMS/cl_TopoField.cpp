#include "StdAfx.h"
#include "cl_Topo_DBMS.h"

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "cl_DFieldTopo.cpp"

/*! \struct SFieldTypes
	*  \brief ¬нутрн€€ структура класса DFieldPhysC
*/
struct SFieldTypes   {
	EFieldDataTypes TypeId;
	_TCHAR *        TName;
	UINT16          TSize;
};

SFieldTypes g_FieldTypesArr[] = {
//	{TYPE_VOID   , _T("VOID")         , -1},
//	{TYPE_BYTE   , _T("BYTE")         ,  2},
//	{TYPE_BYTE   , _T("_TCHAR")       ,  2},
//	{TYPE_WORD   , _T("WORD")         ,  2},
//	{TYPE_INT16  , _T("INT16")        ,  2},
	{TYPE_INT32  , _T("INT32")        ,  4},
	{TYPE_UINT32 , _T("UINT32")       ,  4},
//	{TYPE_INT32  , _T("WORD")         ,  4},
//	{TYPE_INT32  , _T("UWORD")        ,  4},
	{TYPE_BYTE_10, _T("BYTE_10")      , 10},
	{TYPE_STRING , _T("STRING")       , -1},
	{TYPE_VECTOR , _T("VECTOR")       , -1},
	{TYPE_WSTRING, _T("WSTRING")      , -1},

	{TYPE_FINAL  , _T("UNKNOWN_TYPE") , -1},
};


BOOL DFieldTopoC::SetTypeInfo(_TCHAR * pTName)   {
	SFieldTypes * pT=g_FieldTypesArr;

	while(pT->TypeId!=TYPE_FINAL)  {
		if(_tcscmp(pT->TName,pTName)==0)   {
			m_FType=pT->TypeId;
			m_FSize=pT->TSize;
			// printf("%s %s:%d %d\n",m_Name.c_str(),pTName,m_FType,m_FSize);
			return(TRUE);
		}
		pT++;
	}
	return(FALSE);
}

BOOL DFieldTopoC::SetTypeInfo(UINT32 iTValue)   {
	SFieldTypes * pT=g_FieldTypesArr;

	while(pT->TypeId!=TYPE_FINAL)  {
		if(pT->TypeId==iTValue)   {
			m_FType=pT->TypeId;
			m_FSize=pT->TSize;
			return(TRUE);
		}
		pT++;
	}
	return(FALSE);
}


CONST _TCHAR * DFieldTopoC::GetTypeName(void)   {
	SFieldTypes * pT=g_FieldTypesArr;

	while(pT->TypeId!=TYPE_FINAL)  {
		if(m_FType==pT->TypeId)   {
			m_FType=pT->TypeId;
			m_FSize=pT->TSize;
			// printf("%s %s:%d %d\n",m_Name.c_str(),pTName,m_FType,m_FSize);
			return(pT->TName);
		}
		pT++;
	}
	return(pT->TName);
}

