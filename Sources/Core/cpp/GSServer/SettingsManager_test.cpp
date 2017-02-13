#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\settingsmanager_test.h"
#include ".\settings_service.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::CommonServices::CSettingsManager_test);

namespace SS
{
namespace UnitTests
{
namespace CommonServices
{

using namespace SS::CommonServices;
using namespace SS::Interface::Core::CommonServices;

//--------------------------------------------------------------------//

void CSettingsManager_test::Test(void)
{
	CSettingsService SettingsService;
	SettingsService.SetWorkPath(L".\\");

	std::wstring ws=L"xxxx";
	SettingsService.SetString(L"SS::Temp", &ws);

	unsigned int i=1;
	SettingsService.AddParameter(L"1", ptUInt, (void*)&i);
	SettingsService.SetUInt(L"1", i);
	std::wstring wsTemp=L"xxx";
	SettingsService.AddParameter(L"2", ptString);
	SettingsService.SetString(L"1", &wsTemp);
	double d=3.13546;
	SettingsService.AddParameter(L"3", ptDouble);
	SettingsService.SetDouble(L"1", d);

	i=0;
	i=SettingsService.GetUInt(L"1");
	wsTemp=L"";
	SettingsService.GetString(L"2", &wsTemp);
	d=0;
	d=SettingsService.GetDouble(L"3");
	SettingsService.FlushToDisk();

	i=0;
	i=SettingsService.GetUInt(L"1");
	wsTemp=L"";
	SettingsService.GetString(L"2", &wsTemp);
	d=0;
	d=SettingsService.GetDouble(L"3");
}

void CSettingsManager_test::TestAdding(void)
{
	CSettingsService SettingsService;
	SettingsService.SetWorkPath(L".\\");
	
	SettingsService.ResetSettings();
	
	srand(0);

	unsigned int i=0;
	while(i<100){
		wchar_t buf[30];
		unsigned int k=rand();
		swprintf(buf, L"ptUInt_%u", k);
		SettingsService.AddParameter(buf, ptUInt);
		SettingsService.SetUInt(buf, k);
		i++;	
	}

	i=0;
	while(i<100){
		wchar_t buf[50];
		double d=(double)rand()/3.14;
		swprintf(buf, L"ptDouble%f", d);
		SettingsService.AddParameter(buf, ptDouble, (void*)&d);
		SettingsService.SetDouble(buf, d);
		i++;	
	}

	i=0;
	while(i<100){
		wchar_t buf[30];
		std::wstring wsTemp;
		swprintf(buf, L"ptString_%u", rand());
		wsTemp.assign(buf);
		SettingsService.AddParameter(buf, ptString);
		SettingsService.SetString(buf, &wsTemp);
		i++;	
	}

	i=0;
	while(i<100){
		wchar_t buf[30];
		swprintf(buf, L"ptUintArr_%u", rand());
		
		std::vector<unsigned int> vTemp;
		for(int k=0; k<100; k++){
			vTemp.push_back(rand());
		}
		
		SettingsService.AddParameter(buf, ptUInt, true);
		SettingsService.SetArray(buf, ptUInt, (void*)&vTemp);
		i++;	
	}

	i=0;
	while(i<100){
		wchar_t buf[30];
		wchar_t buf1[30];
		swprintf(buf, L"ptStringArr_%u", rand());
		
		std::vector<std::wstring> vTemp;
		std::wstring wsTemp;
		for(int k=0; k<10; k++){
			swprintf(buf1, L"ptString_%u", rand());
			wsTemp.assign(buf1);
			vTemp.push_back(wsTemp);
		}
		
		SettingsService.AddParameter(buf, ptString, true);
		SettingsService.SetArray(buf, ptString, (void*)&vTemp);
		i++;	
	}

	SettingsService.FlushToDisk();
	
	TestGetting();
	TestGetList();
}

void CSettingsManager_test::TestGetList(void)
{
	CSettingsService SettingsService;
	SettingsService.SetWorkPath(L".\\");
	
	std::vector<std::wstring> SettingsNames;
	SettingsService.GetParameterList(&SettingsNames, true);

	bool bIsArray;
	EParameterType Type;
	wchar_t buf[200];
	for(unsigned int i=0; i<SettingsNames.size(); i++){

		Type=SettingsService.GetParameterType(SettingsNames[i].c_str(), &bIsArray);
		
		if(Type==ptUInt && !bIsArray){
			SettingsService.SetUInt(SettingsNames[i].c_str(), SettingsService.GetUInt(SettingsNames[i].c_str())*10);
		}

		switch(Type){
		case ptUInt:	
			if(!bIsArray)		
				swprintf(buf, L"Setting: %s %u \n", SettingsNames[i].c_str(), SettingsService.GetUInt(SettingsNames[i].c_str()));
			break;
		case ptDouble:	
			if(!bIsArray)		
				swprintf(buf, L"Setting: %s %f \n", SettingsNames[i].c_str(), SettingsService.GetDouble(SettingsNames[i].c_str()));
			break;
		case ptString:	
			if(!bIsArray){
				std::wstring wsTemp;
				SettingsService.GetString(SettingsNames[i].c_str(), &wsTemp);
				swprintf(buf, L"Setting: %s %s \n", SettingsNames[i].c_str(), wsTemp.c_str());
			}
			break;
		default:	
			swprintf(buf, L"Setting: %s \n", SettingsNames[i].c_str());
			break;
		}

		if(!wcslen(buf)){
			swprintf(buf, L"Setting: %s \n", SettingsNames[i].c_str());
		}

		wprintf(buf);
		buf[0]=0;	
	}
}

void CSettingsManager_test::TestGetting(void)
{
	CSettingsService SettingsService;
	SettingsService.SetWorkPath(L".\\");
	
	srand(0);

	unsigned int i=0;
	while(i<100){
		wchar_t buf[30];
		unsigned int k=rand();
		swprintf(buf, L"ptUInt_%u", k);
		unsigned int a=SettingsService.GetUInt(buf);
		CPPUNIT_ASSERT_EQUAL(a, k);

		i++;	
	}

	i=0;
	while(i<100){
		wchar_t buf[50];
		double d=(double)rand()/3.14;
		swprintf(buf, L"ptDouble%f", d);
		double t=SettingsService.GetDouble(buf);
		CPPUNIT_ASSERT_EQUAL(d, t);
		i++;	
	}

	i=0;
	while(i<100){
		wchar_t buf[30];
		std::wstring wsTemp;
		std::wstring wsTemp1;
		swprintf(buf, L"ptString_%u", rand());
		wsTemp.assign(buf);
		SettingsService.GetString(buf, &wsTemp1);
		CPPUNIT_ASSERT(wsTemp==wsTemp1);
		i++;	
	}

	i=0;
	while(i<100){
		wchar_t buf[30];
		swprintf(buf, L"ptUintArr_%u", rand());
		
		std::vector<unsigned int> vTemp;
		std::vector<unsigned int> vTemp1;
		for(int k=0; k<100; k++){
			vTemp.push_back(rand());
		}

		SettingsService.GetArray(buf, ptUInt, (void*)&vTemp1);
		CPPUNIT_ASSERT(vTemp==vTemp1);
		i++;	
	}

	i=0;
	while(i<100){
		wchar_t buf[30];
		wchar_t buf1[30];
		swprintf(buf, L"ptStringArr_%u", rand());
		
		std::vector<std::wstring> vTemp;
		std::vector<std::wstring> vTemp1;
		std::wstring wsTemp;
		for(int k=0; k<10; k++){
			swprintf(buf1, L"ptString_%u", rand());
			wsTemp.assign(buf1);
			vTemp.push_back(wsTemp);
		}

		SettingsService.GetArray(buf, ptString, (void*)&vTemp1);
		CPPUNIT_ASSERT(vTemp==vTemp1);
		i++;	
	}
}

//--------------------------------------------------------------------//

}
}
}

#endif //_SS_UNITTESTS
