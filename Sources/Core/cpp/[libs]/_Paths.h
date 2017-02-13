#ifndef _PATHS_H
#define _PATHS_H

#ifdef DEBUG
	#define LOADMANAGER_DLL_PATH      L"E:\\_NET2\\Lingvistics_2016\\Sources\\Core\\cpp\\LoadManager\\Debug\\LoadManager.dll"
	#define AMCONVERTER_DLL_PATH      L"E:\\_NET2\\Lingvistics_2016\\Sources\\Core\\cpp\\AMConverter\\Debug\\AMConverter.dll"
	#define DICTIONARIES_DLL_PATH     L"E:\\_NET2\\Lingvistics_2016\\Sources\\Core\\cpp\\Dictionaries\\Debug\\Dictionaries.dll"
	#define MAINANALYSE_DLL_PATH      L"E:\\_NET2\\Lingvistics_2016\\Sources\\Core\\cpp\\MainAnalyse\\Debug\\MainAnalyse.dll"
	#define COMMONCONTAINERS_DLL_PATH L"E:\\_NET2\\Lingvistics_2016\\Sources\\Core\\cpp\\CommonContainers\\Debug\\CommonContainers.dll"
#else
	#define LOADMANAGER_DLL_PATH      L"./core/LoadManager.dll"
	#define AMCONVERTER_DLL_PATH      L"./core/linguistics/AMConverter.dll"
	#define DICTIONARIES_DLL_PATH     L"./core/linguistics/Dictionaries.dll"
	#define MAINANALYSE_DLL_PATH      L"./core/linguistics/MainAnalyse.dll"
	#define COMMONCONTAINERS_DLL_PATH L"./core/linguistics/CommonContainers.dll"
#endif

#endif