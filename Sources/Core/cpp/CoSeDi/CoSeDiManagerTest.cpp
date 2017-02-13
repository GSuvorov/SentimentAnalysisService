#include "StdAfx.h"
#include ".\cosedimanagertest.h"

#include ".\cosedimanager.h"

#include "../ASSInterface/IAMConverter.h"

#include "../FeatureLibrary/MorphoFeature.h"
#include "../FeatureLibrary/MorphoFeatureAposteriori.h"
#include "../FeatureLibrary/MorphoFeatureApriori.h"

#include <iostream>

#ifdef _SS_UNITTESTS


void SS::UnitTests::CoSeDi::CCoSeDiManagerTest::TestManyLoadAMConverter(void)
{
	SS_TRY;
	{
		for(int i = 0; i < 1000; i++)
		{
			TestLoadAMConverter();
		}

	}
	SS_CATCH(L"");
}
void SS::UnitTests::CoSeDi::CCoSeDiManagerTest::TestLoadAMConverter(void)
{
	SS_TRY;
	{
	
		SS::Interface::Core::AMConverter::IAMConverterMorpho* pIAMConverterMorpho;

		///// инициализация модуля конвертации
		//GetLoadManager()->CreateAndRegister(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho, 
		//	(void**)&pIAMConverterMorpho);


	}
	SS_CATCH(L"");
}

void SS::UnitTests::CoSeDi::CCoSeDiManagerTest::FullTest(void)
{
	try
	{
//		TestManyLoadAMConverter();

		TestLoad();

		//int iTester = 0;
		//S_bf_ui_WorkRegions o_bf_ui_WorkRegions;

		//iTester = *((unsigned short*)&o_bf_ui_WorkRegions);

		//o_bf_ui_WorkRegions.SetSyntax();

		//iTester = *((unsigned short*)&o_bf_ui_WorkRegions);

		///*	не прошел ошибка в 	*/
		//TestCombinatory();


//		TestSemanticByWord();
//		TestSyntax();
//		FullTestSemanticsInDBMS();
//		TestSemantic();
//		TestSemanticIDSourceTree();

//		SemanticClassesTreeTest1();
//		SemanticClassesTreeTest2();
		
//		SemanticClassesTreeTest();

//		TestSyntaxConstruction();
		
//		TestSyntaxConstructionGroups();
		
//		TestFetures();

		std::cout << __FUNCTION__ << " OK \n";
		
	}
	catch(...)
	{
		SS_THROW(L"");
	}
}

void SS::UnitTests::CoSeDi::CCoSeDiManagerTest::TestSemanticByWord(void)
{
	SS_TRY;
	{

		CoInitialize(NULL);
	
		SS::Interface::Core::CoSeDi::ICoSeDiManager* pICoSeDiManager;


		SS::CoSeDi::CCoSeDiManager* pCoSeDiManager = 
			new SS::CoSeDi::CCoSeDiManager();

		pCoSeDiManager->QueryInterface(IID_ICoSeDiManagerSQL, (void**)&pICoSeDiManager);

		if(pICoSeDiManager == NULL)
			SS_THROW(L"");	

		pICoSeDiManager->SetLoadManager(GetLoadManager());

		/*if(!pICoSeDiManager->GetLoader()->ToBinary())
		{
			SS_THROW(L"");
		}*/


/////////////////////////////////////////////////////////////////////////////////
/// Тест загрузки из DBMS	
	
	
		
		pCoSeDiManager->QueryInterface(IID_ICoSeDiManager, (void**)&pICoSeDiManager);

		if(pICoSeDiManager == NULL)
			SS_THROW(L"");	
		
		pICoSeDiManager->SetLoadManager(GetLoadManager());
		if(!pICoSeDiManager->GetLoader()->Load())
			SS_THROW(L"");
		
		
		std::list<long> list_IDClass;
		
		TFormInfo oFormInfoDBMS;
		
		oFormInfoDBMS.m_shIDForm			= 0;
		oFormInfoDBMS.m_uchDictionaryNumber = 5;
		oFormInfoDBMS.m_stIDSource			= 78314;		
		


		pICoSeDiManager->GetSemanticAnalysis()->GetClasses(oFormInfoDBMS, &list_IDClass);
		

		
		pICoSeDiManager->Release();

		CoUninitialize();
	}
	SS_CATCH(L"");
}

void SS::UnitTests::CoSeDi::CCoSeDiManagerTest::TestFetures(void)
{
	SS_TRY;

		SS::CoSeDi::DataTypes::S_bf_ui_WorkRegions obf_ui_WorkRegions;


		unsigned short ushResult;

		obf_ui_WorkRegions.SetSyntax();
		ushResult = *(unsigned short*)&obf_ui_WorkRegions;
		obf_ui_WorkRegions.SetSemantic();
		ushResult = *(unsigned short*)&obf_ui_WorkRegions;

		obf_ui_WorkRegions.SetCombinatory();
		ushResult = *(unsigned short*)&obf_ui_WorkRegions;
		
		

	/**	проверка операции  IsInclude */
		SS::Core::Features::CMorphoFeature oMorphoFeature01;
		SS::Core::Features::CMorphoFeature oMorphoFeature02;

		oMorphoFeature01.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;
		oMorphoFeature02.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;
		oMorphoFeature02.IsIncludes(&oMorphoFeature01);
		
		oMorphoFeature01.m_PartOfSpeechTypes	= SS::Core::Features::Values::PartOfSpeechTypes.postNoun;
		oMorphoFeature01.m_CountType			= SS::Core::Features::Values::CountType.cntCount;
		oMorphoFeature01.m_NumberType			= SS::Core::Features::Values::NumberType.ntSingle;
		oMorphoFeature02.m_PartOfSpeechTypes	= SS::Core::Features::Values::PartOfSpeechTypes.postNoun;
		bool bRetVal =	oMorphoFeature02.IsIncludes(&oMorphoFeature01);


		SS::Core::Features::CMorphoFeatureApriori	oMorphoFeatureAprioriSentence;
		SS::Core::Features::CMorphoFeatureApriori	oMorphoFeatureAprioriConstruction;

		oMorphoFeatureAprioriSentence.m_PartOfSpeechTypes		= SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
		int i = 4|2;
		int ii = 2|1;
		oMorphoFeatureAprioriConstruction.m_PartOfSpeechTypes	= SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
		oMorphoFeatureAprioriConstruction.m_PartOfSpeechTypes.AssignOR(&SS::Core::Features::Values::PartOfSpeechTypes.postOfficial);
		oMorphoFeatureAprioriSentence.IsIncludes(&oMorphoFeatureAprioriConstruction);
		
		oMorphoFeatureAprioriSentence.m_PartOfSpeechTypes		= SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;
		oMorphoFeatureAprioriConstruction.m_PartOfSpeechTypes	= SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
		oMorphoFeatureAprioriConstruction.m_PartOfSpeechTypes.AssignOR(&SS::Core::Features::Values::PartOfSpeechTypes.postOfficial);
		oMorphoFeatureAprioriSentence.IsIncludes(&oMorphoFeatureAprioriConstruction);
		


		SS::Core::Features::CSyntaxFeatureApriori	oSyntaxFeatureApriori1;
		SS::Core::Features::CSyntaxFeatureApriori	oSyntaxFeatureApriori2;

		oSyntaxFeatureApriori1 = oSyntaxFeatureApriori2;

		SS::Core::Features::CMorphoFeatureApriori	oMorphoFeatureApriori1;
		SS::Core::Features::CMorphoFeatureApriori	oMorphoFeatureApriori2;

		oMorphoFeatureApriori1	= oMorphoFeatureApriori2;
		
		SS::Core::Features::CSyntaxFeatureAposteriori	oSyntaxFeatureAposteriori;
		oSyntaxFeatureAposteriori.m_SyntaxCategories	= 9;
		//if(oSyntaxFeatureAposteriori.m_SyntaxCategories	== sсAjunct)
		//	return ;

		SS::Core::Features::Types::SyntaxCategories	oSyntaxCategorie;
		oSyntaxCategorie = 9;


		SS::Core::Features::CSemanticFeature  oSemanticFeature1;
		SS::Core::Features::CSemanticFeature  oSemanticFeature2;


		if(oSemanticFeature2.Equal(&oSemanticFeature1))
			int i = 0;
		oSemanticFeature2 = oSemanticFeature1;
		

		std::list<SS::Core::Features::CSemanticFeature*> listOfSemanticFeature;

		SS::Core::Features::CSemanticFeature * pSemanticFeature1;
		
		pSemanticFeature1 = new SS::Core::Features::CSemanticFeature;
		listOfSemanticFeature.push_back(pSemanticFeature1);
		
		SS::Core::Features::CSemanticFeature * pSemanticFeature2;		
		pSemanticFeature2 = new SS::Core::Features::CSemanticFeature;
		listOfSemanticFeature.push_back(pSemanticFeature2);
		
		SS::Core::Features::CSemanticFeature * pSemanticFeature3;		
		pSemanticFeature3 = new SS::Core::Features::CSemanticFeature;
		listOfSemanticFeature.push_back(pSemanticFeature3);

		SS::Core::Features::CSemanticFeature * pSemanticFeature4;		
		pSemanticFeature4 = new SS::Core::Features::CSemanticFeature;
		listOfSemanticFeature.push_back(pSemanticFeature4);

		std::vector<unsigned char> vecOfUI;
		listOfSemanticFeature.front()->GetValue(vecOfUI);
		listOfSemanticFeature.back()->GetValue(vecOfUI);
		listOfSemanticFeature.front()->GetValue(vecOfUI);


		delete pSemanticFeature1;
		delete pSemanticFeature2;
		delete pSemanticFeature3;
		delete pSemanticFeature4;



		
	SS_CATCH(L"");
	
}

void SS::UnitTests::CoSeDi::CCoSeDiManagerTest::TestLoad(void)
{
	CoInitialize(NULL);
	
	SS::Interface::Core::CoSeDi::ICoSeDiManager* pICoSeDiManagerSQL;
	pICoSeDiManagerSQL = 
		(SS::Interface::Core::CoSeDi::ICoSeDiManager*)
		GetLoadManager()->GetInterface(L"./core/linguistics/CoSeDi.dll", CLSID_ICoSeDiManager, IID_ICoSeDiManagerSQL);

	pICoSeDiManagerSQL->GetLoader()->Load();

	if(!pICoSeDiManagerSQL->GetLoader()->ToBinary())
		SS_THROW(L"");

	
//	SS::CoSeDi::CCoSeDiManager* pCoSeDiManager = new SS::CoSeDi::CCoSeDiManager();
//	
//	SS::Interface::Core::CoSeDi::ICoSeDiManager* pICoSeDiManager;
//	
///// Тест загрузки SQL		
//
//	pCoSeDiManager->QueryInterface(IID_ICoSeDiManagerSQL, (void**)&pICoSeDiManager);
//
//	if(pICoSeDiManager == NULL)
//		SS_THROW(L"");	
//
//	pICoSeDiManager->SetLoadManager(GetLoadManager());
//
//	if(!pICoSeDiManager->GetLoader()->ToBinary())
//		SS_THROW(L"");
//
//
///// Тест загрузки из DBMS	
//	
//	pCoSeDiManager->QueryInterface(IID_ICoSeDiManager, (void**)&pICoSeDiManager);
//
//	if(pICoSeDiManager == NULL)
//		SS_THROW(L"");	
//
//	pICoSeDiManager->SetLoadManager(GetLoadManager());
//
//	
//	pICoSeDiManager->Release();
//
//
////////---------------------------//////////////////////////
//	/// русская база
//	pCoSeDiManager = new SS::CoSeDi::CCoSeDiManager();
//	
///// Тест загрузки SQL		
//
//	pCoSeDiManager->QueryInterface(IID_ICoSeDiManagerSQL, (void**)&pICoSeDiManager);
//
//	if(pICoSeDiManager == NULL)
//		SS_THROW(L"");	
//
//	pICoSeDiManager->SetLoadManager(GetLoadManager());
//
//
///// Тест загрузки из DBMS	
//	
//	pCoSeDiManager->QueryInterface(IID_ICoSeDiManager, (void**)&pICoSeDiManager);
//
//	if(pICoSeDiManager == NULL)
//		SS_THROW(L"");	
//
//	pICoSeDiManager->SetLoadManager(GetLoadManager());
//
//	
//	pICoSeDiManager->Release();

	CoUninitialize();

}

void SS::UnitTests::CoSeDi::CCoSeDiManagerTest::TestCombinatory(void)
{
	CoInitialize(NULL);
	
	
	SS::CoSeDi::CCoSeDiManager* pCoSeDiManager = new SS::CoSeDi::CCoSeDiManager();
	
	SS::Interface::Core::CoSeDi::ICoSeDiManager* pICoSeDiManager;
	
/// Тест загрузки SQL		
//	pCoSeDiManager->QueryInterface(IID_ICoSeDiManagerSQL, (void**)&pICoSeDiManager);
//
//	if(pICoSeDiManager == NULL)
//		SS_THROW(L"");	
//
//	pICoSeDiManager->SetLoadManager(GetLoadManager());
/////	Начальная загрузка  - откиньтесь на спинку кресла и подождите пока программа произведет подготовку данных к работе
//
//	SS::CoSeDi::DataTypes::TFormInfo oFormInfo;	
//	TIDSource oIDSource;
//	TCombinationInfo  oCombinationInfoWrite;
//	oCombinationInfoWrite.shIDType	   = -1;
//	oCombinationInfoWrite.eCombiType	=  (ECombiType)0;
//
//	for(int i = 1; i < 3; i++)
//	{
//		oIDSource.ushIDRoot		= i;
//		oIDSource.uchIDSuffics	= 0;
//		oFormInfo.m_shIDForm	= i;
//		oFormInfo.m_uchDictionaryNumber	= 2;
//		oFormInfo.m_stIDSource = oIDSource;
//		
//		oCombinationInfoWrite.m_T_list_FormInfo.push_back(oFormInfo);
//	}
//	
//		/*	создание новой комбинации по 	TCombinationInfo */
//	int iNewCombinationID =	pICoSeDiManager->GetCombinatoryAnalysis()->CreateCombination(&oCombinationInfoWrite);
//	
/////	установка комбинации с которой в дальнейшем будем работать
//	pICoSeDiManager->GetCombinatoryAnalysis()->SetCombination(iNewCombinationID);
//
//	TCombinationInfo  oCombinationInfoRead;
////// Считывание данных о текущей комбинации, выбранной в предыдущей команде
//	pICoSeDiManager->GetCombinatoryAnalysis()->GetCombinationInfo(&oCombinationInfoRead);
//
//
//	std::list<int> list_OfCombination;
//	
//
//	/*	удаление текущей комбинации	*/
//	pICoSeDiManager->GetCombinatoryAnalysis()->DeleteCombination();
//	

// Тест загрузки из DBMS	
	
	pCoSeDiManager->QueryInterface(IID_ICoSeDiManager, (void**)&pICoSeDiManager);

	if(pICoSeDiManager == NULL)
		SS_THROW(L"");	

	pICoSeDiManager->SetLoadManager(GetLoadManager());


///	установка комбинации с которой в дальнейшем будем работать
//	pICoSeDiManager->GetCombinatoryAnalysis()->SetCombination(1);
//	
//	TCombinationInfo  oCombinationInfo;
////// Считывание данных о текущей комбинации, выбранной в предыдущей команде
//	pICoSeDiManager->GetCombinatoryAnalysis()->GetCombinationInfo(&oCombinationInfo);
//

	std::list<int> list_IDCombination;
	
	SS::CoSeDi::DataTypes::TFormInfo oFormInfoDBMS;

	oFormInfoDBMS.m_shIDForm			= 0;
	oFormInfoDBMS.m_stIDSource			= 16781;
	oFormInfoDBMS.m_uchDictionaryNumber = 5;

	/*	получение списка комбинаций в которые вступает основа	*/
	pICoSeDiManager->GetCombinatoryAnalysis()->GetCombinations(oFormInfoDBMS, &list_IDCombination);

	if(list_IDCombination.size() == 0)
	{
		std::cout<<"list_IDCombination.size() == 0";
		SS_THROW(L"");
	}

	list_IDCombination.clear();

	oFormInfoDBMS.m_shIDForm			= 0;
	oFormInfoDBMS.m_stIDSource			= 105425;
	oFormInfoDBMS.m_uchDictionaryNumber = 0;

	/*	получение списка комбинаций в которые вступает основа	*/
	pICoSeDiManager->GetCombinatoryAnalysis()->GetCombinations(oFormInfoDBMS, &list_IDCombination);

	if(list_IDCombination.size() == 0)
	{
		std::cout<<"list_IDCombination.size() == 0";
		SS_THROW(L"");
	}


	SS::CoSeDi::DataTypes::TCombinationInfo oCombinationInfo;
	
	pICoSeDiManager->GetCombinatoryAnalysis()->SetCombination(1 + (SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory<<24));
	pICoSeDiManager->GetCombinatoryAnalysis()->GetCombinationInfo(&oCombinationInfo);
	
	if((oCombinationInfo.m_T_list_FormInfo.begin()->m_stIDSource.ushIDRoot != 14375)||
		(oCombinationInfo.m_T_list_FormInfo.back().m_stIDSource.ushIDRoot != 117257))
	{
		SS_THROW(L"");
	}


	/*pICoSeDiManager->GetCombinatoryAnalysis()->SetCombination(15322);
	pICoSeDiManager->GetCombinatoryAnalysis()->GetCombinationInfo(&oCombinationInfo);
	
	if((oCombinationInfo.m_T_list_FormInfo.begin()->m_stIDSource.ushIDRoot != 129871)||
		(oCombinationInfo.m_T_list_FormInfo.back().m_stIDSource.ushIDRoot != 122486))
	{
		SS_THROW(L"");
	}*/

//
	/*	окончание работы с 	ICoSeDiManager */
	pICoSeDiManager->Release();

	CoUninitialize();

}

void SS::UnitTests::CoSeDi::CCoSeDiManagerTest::TestSemantic(void)
{
	CoInitialize(NULL);
	
	
	SS::Interface::Core::AMConverter::IAMConverterSemantic* pIAMConverterSemantic = NULL;

	/// инициализация модуля конвертации
	pIAMConverterSemantic = (SS::Interface::Core::AMConverter::IAMConverterSemantic*)GetLoadManager()->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterSemantic, IID_AMConverterSemantic);


	//SS::Core::Features::CSemanticFeature oSemanticFeatureIn;
	//SS::Core::Features::CSemanticFeature oSemanticFeatureOut;

	////oSemanticFeatureIn.m_SemanticTypes = SS::Semantic::DataTypes::semtSecondName;
	//oSemanticFeatureIn.m_SemanticType		= smtTimeInterval;

	//SS::CoSeDi::DataTypes::TSemanticType iSymanticType = 0;

	//pIAMConverterSemantic->Code(&oSemanticFeatureIn, &iSymanticType);

	//pIAMConverterSemantic->EnCode(iSymanticType, &oSemanticFeatureOut);



	SS::Interface::Core::CoSeDi::ICoSeDiManager* pICoSeDiManagerSQL;
	pICoSeDiManagerSQL = 
		(SS::Interface::Core::CoSeDi::ICoSeDiManager*)
		GetLoadManager()->GetInterface(L"./core/linguistics/CoSeDi.dll", CLSID_ICoSeDiManager, IID_ICoSeDiManagerSQL);

	
/// Тест загрузки SQL		
	//if(pICoSeDiManagerSQL == NULL)
	//	SS_THROW(L"");	

	//pICoSeDiManagerSQL->SetLoadManager(GetLoadManager());



	//TFormInfo oFormInfo;
	//TIDSource oIDSource;
	//
	//oFormInfo.m_uchDictionaryNumber		= 1;
	//oFormInfo.m_shIDForm				= 1;
	//oFormInfo.m_stIDSource.uchIDSuffics	= 1;
	//oFormInfo.m_stIDSource.ushIDRoot	= 1;

	//
	//pICoSeDiManagerSQL->GetSemanticAnalysis()->SetClass(iSymanticType);

	//std::list<long> list_OfIDClass;



	
/////////////////////////////////////////////////////////////////////////////////
/// Тест загрузки из DBMS	
	
	SS::Interface::Core::CoSeDi::ICoSeDiManager* pICoSeDiManager;
	pICoSeDiManager = 
		(SS::Interface::Core::CoSeDi::ICoSeDiManager*)
		GetLoadManager()->GetInterface(L"./core/linguistics/CoSeDi.dll", CLSID_ICoSeDiManager, IID_ICoSeDiManager);

	if(pICoSeDiManager == NULL)
		SS_THROW(L"");	
	
	pICoSeDiManager->SetLoadManager(GetLoadManager());
	if(!pICoSeDiManager->GetLoader()->Load())
		SS_THROW(L"");
	
	TFormInfo oFormInfoDBMS;
	std::list<long> list_IDClass;

	oFormInfoDBMS.m_stIDSource			= 34920;
	oFormInfoDBMS.m_uchDictionaryNumber	= 0;

	pICoSeDiManager->GetSemanticAnalysis()->GetClasses(oFormInfoDBMS, &list_IDClass);
	
	if(list_IDClass.size() == 0)
	{
		SS_THROW(L"");	
	}
	list_IDClass.clear();

	
	oFormInfoDBMS.m_stIDSource			= 7265;
	oFormInfoDBMS.m_uchDictionaryNumber	= 6;

	pICoSeDiManager->GetSemanticAnalysis()->GetClasses(oFormInfoDBMS, &list_IDClass);
	if(list_IDClass.size() == 0)
	{
		SS_THROW(L"");	
	}
	
	SS::CoSeDi::DataTypes::T_list_FormInfo  o_list_FormInfo;
	pICoSeDiManager->GetSemanticAnalysis()->SetClass(list_IDClass.front());
	pICoSeDiManager->GetSemanticAnalysis()->GetWords(&o_list_FormInfo);
	
	CoUninitialize();
}

void SS::UnitTests::CoSeDi::CCoSeDiManagerTest::FullTestSemanticsInDBMS(void)
{
	SS_TRY
	{
		CoInitialize(NULL);
		
		
		SS::CoSeDi::CCoSeDiManager* pCoSeDiManager = new SS::CoSeDi::CCoSeDiManager();
		
		SS::Interface::Core::CoSeDi::ICoSeDiManager* pICoSeDiManager;
		
	/////////////////////////////////////////////////////////////////////////////////
	/// Тест загрузки из DBMS	
		
		pCoSeDiManager->QueryInterface(IID_ICoSeDiManager, (void**)&pICoSeDiManager);

		if(pICoSeDiManager == NULL)
			SS_THROW(L"");	
		
		pICoSeDiManager->SetLoadManager(GetLoadManager());
		if(!pICoSeDiManager->GetLoader()->Load())
			SS_THROW(L"");
		
		

		SS::CoSeDi::DataTypes::TFormInfo oFormInfo;
		SS::CoSeDi::DataTypes::T_list_FormInfo olist_FormInfo;

		oFormInfo.m_stIDSource.uchIDSuffics = 0;
		oFormInfo.m_stIDSource.ushIDRoot	= 10000;
		oFormInfo.m_uchDictionaryNumber		= 0;

		SS::Interface::Core::CoSeDi::ISemanticAnalysis* pSemanticAnalysis = pICoSeDiManager->GetSynonimsSemanticAnalysis();
		pSemanticAnalysis->SetClass(1000000);
		pSemanticAnalysis->AddWord(oFormInfo);

		pSemanticAnalysis->GetWords(&olist_FormInfo);

		if(olist_FormInfo.size() != 1)
		{
			SS_THROW(L"Информация о классе внесена, но она отсутсвует в базе");
		}

		if(!(olist_FormInfo.front() == oFormInfo))
		{
			SS_THROW(L"Вносилась одна информация, а получена другая");
		}

		olist_FormInfo.clear();

		pSemanticAnalysis->DeleteClass( (SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber) oFormInfo.m_uchDictionaryNumber);

		pSemanticAnalysis->GetWords(&olist_FormInfo);

		if(olist_FormInfo.size() != 0)
		{
			SS_THROW(L"Класс удален, но информация осталась");
		}

		pSemanticAnalysis->AddWord(oFormInfo);

		pSemanticAnalysis->GetWords(&olist_FormInfo);

		if(olist_FormInfo.size() != 1)
		{
			SS_THROW(L"Информация о классе внесена, но она отсутсвует в базе");
		}

		if(!(olist_FormInfo.front() == oFormInfo))
		{
			SS_THROW(L"Вносилась одна информация, а получена другая");
		}

		olist_FormInfo.clear();

		pSemanticAnalysis->DeleteWord(oFormInfo);

		pSemanticAnalysis->GetWords(&olist_FormInfo);

		if(olist_FormInfo.size() != 0)
		{
			SS_THROW(L"Единственное слово из класса удалено, но в классе информация осталась");
		}

		pICoSeDiManager->Release();

		CoUninitialize();
	}
	SS_CATCH(L"")

}

void SS::UnitTests::CoSeDi::CCoSeDiManagerTest::SemanticClassesTreeTest1(void)
{
	CoInitialize(NULL);
	
	
	SS::CoSeDi::CCoSeDiManager* pCoSeDiManager = new SS::CoSeDi::CCoSeDiManager();
	
	SS::Interface::Core::CoSeDi::ICoSeDiManager* pICoSeDiManager;
	
/// Тест загрузки SQL		
	pCoSeDiManager->QueryInterface(IID_ICoSeDiManagerSQL, (void**)&pICoSeDiManager);

	if(pICoSeDiManager == NULL)
		SS_THROW(L"");	

	pICoSeDiManager->SetLoadManager(GetLoadManager());


	std::string sClassNameIn = "НевозможныйСемантическийКласс1", sClassNameOut = ""; 
	int iClassIn = -1, iClassOut = -1;

	pICoSeDiManager->GetSemanticClassesTree()->SetClass(0);
	/// создаем класс с именем sClassNameIn и получаем его индекс
	iClassIn = pICoSeDiManager->GetSemanticClassesTree()->CreateChild(&sClassNameIn);
	/// устанавливаем данный индекс текущим для работы
	pICoSeDiManager->GetSemanticClassesTree()->SetClass(iClassIn);
	/// получаем по индексу имя класса
	pICoSeDiManager->GetSemanticClassesTree()->GetName(&sClassNameOut);
	/// имя должно совпадать с тем, которое мы использовали для создания класса
	/// по имени получаем номер класса
	iClassOut = pICoSeDiManager->GetSemanticClassesTree()->GetClass(&sClassNameIn);
	/// номер класса должен совпадать с текущим рабочим классом
	/// изменим имя текущего класса
	sClassNameIn = "НевозможныйСемантическийКласс2";
	pICoSeDiManager->GetSemanticClassesTree()->SetName(&sClassNameIn);
	/// получаем по индексу имя класса
	pICoSeDiManager->GetSemanticClassesTree()->GetName(&sClassNameOut);
	/// имя должно совпадать с измененным
	/// по имени получаем номер класса
	iClassOut = pICoSeDiManager->GetSemanticClassesTree()->GetClass(&sClassNameIn);
	/// Удалим свои чудачества	
	pICoSeDiManager->GetSemanticClassesTree()->DeleteClass();

	

/// Тест загрузки из DBMS	
	
	pCoSeDiManager->QueryInterface(IID_ICoSeDiManager, (void**)&pICoSeDiManager);

	if(pICoSeDiManager == NULL)
		SS_THROW(L"");	

	pICoSeDiManager->SetLoadManager(GetLoadManager());
	

	pICoSeDiManager->Release();

	CoUninitialize();
}

void SS::UnitTests::CoSeDi::CCoSeDiManagerTest::SemanticClassesTreeTest(void)
{
	CoInitialize(NULL);
	
	
	SS::CoSeDi::CCoSeDiManager* pCoSeDiManager = new SS::CoSeDi::CCoSeDiManager();
	
	SS::Interface::Core::CoSeDi::ICoSeDiManager* pICoSeDiManager;
	
/// Тест загрузки SQL		
	pCoSeDiManager->QueryInterface(IID_ICoSeDiManagerSQL, (void**)&pICoSeDiManager);

	if(pICoSeDiManager == NULL)
		SS_THROW(L"");	

	pICoSeDiManager->SetLoadManager(GetLoadManager());


	std::string sName = "";
	pICoSeDiManager->GetSemanticClassesTree()->SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednENominalName);
	pICoSeDiManager->GetSemanticClassesTree()->SetClass(100);
	pICoSeDiManager->GetSemanticClassesTree()->GetName(&sName);

	sName = "";
	pICoSeDiManager->GetSemanticClassesTree()->SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName);
	pICoSeDiManager->GetSemanticClassesTree()->SetClass(100);
	pICoSeDiManager->GetSemanticClassesTree()->GetName(&sName);
	

/// Тест загрузки из DBMS	
	
	pCoSeDiManager->QueryInterface(IID_ICoSeDiManager, (void**)&pICoSeDiManager);

	if(pICoSeDiManager == NULL)
		SS_THROW(L"");	

	pICoSeDiManager->SetLoadManager(GetLoadManager());
	

	pICoSeDiManager->Release();

	CoUninitialize();
}


void SS::UnitTests::CoSeDi::CCoSeDiManagerTest::TestSemanticIDSourceTree(void)
{
	CoInitialize(NULL);
	
	
	SS::CoSeDi::CCoSeDiManager* pCoSeDiManager = new SS::CoSeDi::CCoSeDiManager();
	
	SS::Interface::Core::CoSeDi::ICoSeDiManager* pICoSeDiManager;
	
/// Тест загрузки SQL		
	pCoSeDiManager->QueryInterface(IID_ICoSeDiManagerSQL, (void**)&pICoSeDiManager);

	if(pICoSeDiManager == NULL)
		SS_THROW(L"");	
	
	pICoSeDiManager->SetLoadManager(GetLoadManager());



	SS::CoSeDi::DataTypes::SFullIDSource m_oFullIDSourceParent;
	SS::CoSeDi::DataTypes::SFullIDSource m_oFullIDSourceChild;
	
	/*	добавляем элементы (родитель, ребенок) в базу	*/
	m_oFullIDSourceChild.m_ouiIDSource.uchIDSuffics	= 1;
	m_oFullIDSourceChild.m_ouiIDSource.ushIDRoot	= 2;
	m_oFullIDSourceChild.m_uchDictionaryNumber		= 3;

	pICoSeDiManager->GetSemanticIDSourceTree()->SetIDChild(m_oFullIDSourceChild);

	m_oFullIDSourceParent.m_ouiIDSource.uchIDSuffics	= 2;
	m_oFullIDSourceParent.m_ouiIDSource.ushIDRoot		= 3;
	m_oFullIDSourceParent.m_uchDictionaryNumber			= 4;

	pICoSeDiManager->GetSemanticIDSourceTree()->SetIDParent(m_oFullIDSourceParent);
	pICoSeDiManager->GetSemanticIDSourceTree()->Add();

	m_oFullIDSourceChild.m_ouiIDSource.uchIDSuffics	= 2;
	m_oFullIDSourceChild.m_ouiIDSource.ushIDRoot	= 3;
	m_oFullIDSourceChild.m_uchDictionaryNumber		= 4;

	pICoSeDiManager->GetSemanticIDSourceTree()->SetIDChild(m_oFullIDSourceChild);

	m_oFullIDSourceParent.m_ouiIDSource.uchIDSuffics	= 3;
	m_oFullIDSourceParent.m_ouiIDSource.ushIDRoot		= 4;
	m_oFullIDSourceParent.m_uchDictionaryNumber			= 5;

	pICoSeDiManager->GetSemanticIDSourceTree()->SetIDParent(m_oFullIDSourceParent);
	pICoSeDiManager->GetSemanticIDSourceTree()->Add();

	m_oFullIDSourceChild.m_ouiIDSource.uchIDSuffics	= 3;
	m_oFullIDSourceChild.m_ouiIDSource.ushIDRoot	= 4;
	m_oFullIDSourceChild.m_uchDictionaryNumber		= 5;

	pICoSeDiManager->GetSemanticIDSourceTree()->SetIDChild(m_oFullIDSourceChild);

	m_oFullIDSourceParent.m_ouiIDSource.uchIDSuffics	= 4;
	m_oFullIDSourceParent.m_ouiIDSource.ushIDRoot		= 5;
	m_oFullIDSourceParent.m_uchDictionaryNumber			= 6;

	pICoSeDiManager->GetSemanticIDSourceTree()->SetIDParent(m_oFullIDSourceParent);
	pICoSeDiManager->GetSemanticIDSourceTree()->Add();

	
	/*	определяем  ребенка, для которого будем искать родителей	*/
	m_oFullIDSourceChild.m_ouiIDSource.uchIDSuffics		= 1;
	m_oFullIDSourceChild.m_ouiIDSource.ushIDRoot		= 2;
	m_oFullIDSourceChild.m_uchDictionaryNumber			= 3;

	pICoSeDiManager->GetSemanticIDSourceTree()->SetIDChild(m_oFullIDSourceChild);

	std::list<SS::CoSeDi::DataTypes::SFullIDSource> olistOfFullIDSourceParents;
	/*	получаем всех родителей ребенка	*/
	pICoSeDiManager->GetSemanticIDSourceTree()->GetParents(&olistOfFullIDSourceParents);

	/*	проверяем на качество	*/

	m_oFullIDSourceParent.m_ouiIDSource.uchIDSuffics	= 2;
	m_oFullIDSourceParent.m_ouiIDSource.ushIDRoot		= 3;
	m_oFullIDSourceParent.m_uchDictionaryNumber			= 4;


	m_oFullIDSourceParent.m_ouiIDSource.uchIDSuffics	= 4;
	m_oFullIDSourceParent.m_ouiIDSource.ushIDRoot		= 5;
	m_oFullIDSourceParent.m_uchDictionaryNumber			= 6;

	/*	удаляем добавленные нами элементы (родитель, ребенок) из базы	*/
	m_oFullIDSourceChild.m_ouiIDSource.uchIDSuffics	= 1;
	m_oFullIDSourceChild.m_ouiIDSource.ushIDRoot	= 2;
	m_oFullIDSourceChild.m_uchDictionaryNumber		= 3;

	pICoSeDiManager->GetSemanticIDSourceTree()->SetIDChild(m_oFullIDSourceChild);

	m_oFullIDSourceParent.m_ouiIDSource.uchIDSuffics	= 2;
	m_oFullIDSourceParent.m_ouiIDSource.ushIDRoot		= 3;
	m_oFullIDSourceParent.m_uchDictionaryNumber			= 4;



	pCoSeDiManager->QueryInterface(IID_ICoSeDiManager, (void**)&pICoSeDiManager);

	if(pICoSeDiManager == NULL)
		SS_THROW(L"");	

	
	m_oFullIDSourceChild.m_ouiIDSource.uchIDSuffics	= 1;
	m_oFullIDSourceChild.m_ouiIDSource.ushIDRoot	= 2;
	m_oFullIDSourceChild.m_uchDictionaryNumber		= 3;

	pCoSeDiManager->GetSemanticIDSourceTree()->SetIDChild(m_oFullIDSourceChild);

	std::list<SS::CoSeDi::DataTypes::SFullIDSource> listOfFullIDSource;
	pCoSeDiManager->GetSemanticIDSourceTree()->GetParents(&listOfFullIDSource);


//// Удаление внесенных данных

	pICoSeDiManager->GetSemanticIDSourceTree()->SetIDParent(m_oFullIDSourceParent);
	pICoSeDiManager->GetSemanticIDSourceTree()->Delete();

	m_oFullIDSourceChild.m_ouiIDSource.uchIDSuffics	= 2;
	m_oFullIDSourceChild.m_ouiIDSource.ushIDRoot	= 3;
	m_oFullIDSourceChild.m_uchDictionaryNumber		= 4;

	pICoSeDiManager->GetSemanticIDSourceTree()->SetIDChild(m_oFullIDSourceChild);

	m_oFullIDSourceParent.m_ouiIDSource.uchIDSuffics	= 3;
	m_oFullIDSourceParent.m_ouiIDSource.ushIDRoot		= 4;
	m_oFullIDSourceParent.m_uchDictionaryNumber			= 5;

	pICoSeDiManager->GetSemanticIDSourceTree()->SetIDParent(m_oFullIDSourceParent);
	pICoSeDiManager->GetSemanticIDSourceTree()->Delete();

	m_oFullIDSourceChild.m_ouiIDSource.uchIDSuffics	= 3;
	m_oFullIDSourceChild.m_ouiIDSource.ushIDRoot	= 4;
	m_oFullIDSourceChild.m_uchDictionaryNumber		= 5;

	pICoSeDiManager->GetSemanticIDSourceTree()->SetIDChild(m_oFullIDSourceChild);

	m_oFullIDSourceParent.m_ouiIDSource.uchIDSuffics	= 4;
	m_oFullIDSourceParent.m_ouiIDSource.ushIDRoot		= 5;
	m_oFullIDSourceParent.m_uchDictionaryNumber			= 6;

	pICoSeDiManager->GetSemanticIDSourceTree()->SetIDParent(m_oFullIDSourceParent);
	pICoSeDiManager->GetSemanticIDSourceTree()->Delete();



	pICoSeDiManager->Release();

	CoUninitialize();
}


void SS::UnitTests::CoSeDi::CCoSeDiManagerTest::SemanticClassesTreeTest2(void)
{
	CoInitialize(NULL);
	
	
	SS::CoSeDi::CCoSeDiManager* pCoSeDiManager = new SS::CoSeDi::CCoSeDiManager();
	
	SS::Interface::Core::CoSeDi::ICoSeDiManager* pICoSeDiManager;
	
/// Тест загрузки SQL		
	pCoSeDiManager->QueryInterface(IID_ICoSeDiManagerSQL, (void**)&pICoSeDiManager);

	if(pICoSeDiManager == NULL)
		SS_THROW(L"");	

	pICoSeDiManager->SetLoadManager(GetLoadManager());

	std::string sClassNameIn = "НевозможныйСемантическийКласс", sClassNameOut = ""; 
	int iClassIn = -1, iClassOut = -1;

	std::string sClassNameWrite = "";

	/// перемещаемся в корень	
	pICoSeDiManager->GetSemanticClassesTree()->SetClass(0);
		
	char szToChar[20];		
	for(int i = 0; i < 3; i++)
	{
		sClassNameWrite = sClassNameIn + itoa(i, szToChar, 10);
		/// создаем класс с именем sClassNameIn и получаем его индекс
		iClassIn = pICoSeDiManager->GetSemanticClassesTree()->CreateChild(&sClassNameWrite);
		/// устанавливаем данный индекс текущим для работы
		pICoSeDiManager->GetSemanticClassesTree()->SetClass(iClassIn);	
	}
	// получим родителя
	iClassIn = pICoSeDiManager->GetSemanticClassesTree()->GetParent();
	// сделаем его текущим классом для работы
	pICoSeDiManager->GetSemanticClassesTree()->SetClass(iClassIn);	
	// создаем еще одного ребенка - получим ветвистое дерево невозомжных семантических классов
	sClassNameWrite = sClassNameIn + itoa(4, szToChar, 10);
	iClassIn = pICoSeDiManager->GetSemanticClassesTree()->CreateChild(&sClassNameWrite);
	/// устанавливаем данный индекс текущим для работы
	pICoSeDiManager->GetSemanticClassesTree()->SetClass(iClassIn);	


	std::list<int> o_listChild;
	pICoSeDiManager->GetSemanticClassesTree()->GetSame(&o_listChild);
	o_listChild.clear();
	
	std::list<int> o_listParent;
	pICoSeDiManager->GetSemanticClassesTree()->GetParents(100, &o_listParent);
	o_listParent.clear();

	int iIDPrent = pICoSeDiManager->GetSemanticClassesTree()->GetParent();
	
	while(iIDPrent != 0)
	{
		pICoSeDiManager->GetSemanticClassesTree()->SetClass(iIDPrent);
		iIDPrent = pICoSeDiManager->GetSemanticClassesTree()->GetParent();
	}

	pICoSeDiManager->GetSemanticClassesTree()->DeleteClass();

/// Тест загрузки из DBMS	
	
	pCoSeDiManager->QueryInterface(IID_ICoSeDiManager, (void**)&pICoSeDiManager);

	if(pICoSeDiManager == NULL)
		SS_THROW(L"");	
	
	pICoSeDiManager->SetLoadManager(GetLoadManager());
	
	pICoSeDiManager->GetSemanticClassesTree()->SetClass(1);

	std::list<int> list_IDCilds;

	pICoSeDiManager->GetSemanticClassesTree()->GetChilds(100, &list_IDCilds);

	pICoSeDiManager->GetSemanticClassesTree()->SetClass(list_IDCilds.back());
	
	std::list<int> list_Parents;
	pICoSeDiManager->GetSemanticClassesTree()->GetParents(100, &list_Parents);

	std::list<int> p_list_SameChilds;
	pICoSeDiManager->GetSemanticClassesTree()->GetSame(&p_list_SameChilds);

	pICoSeDiManager->Release();

	CoUninitialize();
}

void SS::UnitTests::CoSeDi::CCoSeDiManagerTest::TestSyntax(void)
{
 	const int ciMinIDSource = 200000, ciMaxIDSource = 200005;

	CoInitialize(NULL);
	
	
	SS::Interface::Core::AMConverter::IAMConverterSyntax* pIAMConverterSyntax = NULL;

	/// инициализация модуля конвертации
	pIAMConverterSyntax = 
		(SS::Interface::Core::AMConverter::IAMConverterSyntax*)
		GetLoadManager()->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterSyntax, IID_AMConverterSyntax);

	//SS::CoSeDi::CCoSeDiManager* pCoSeDiManager = new SS::CoSeDi::CCoSeDiManager();
	
	SS::Interface::Core::CoSeDi::ICoSeDiManager* pICoSeDiManagerSQL;
	pICoSeDiManagerSQL = 
		(SS::Interface::Core::CoSeDi::ICoSeDiManager*)
		GetLoadManager()->GetInterface(L"./core/linguistics/CoSeDi.dll", CLSID_ICoSeDiManager, IID_ICoSeDiManagerSQL);

	SS::CoSeDi::DataTypes::TIDSource oIDSource;
	std::list<TSyntaxInfo> list_ofSyntaxInfoIn;

	
/// Тест SQL		

	//if(pICoSeDiManager == NULL)
	//	SS_THROW(L"");	

	//pICoSeDiManagerSQL->SetLoadManager(GetLoadManager());

	///// подготовка исходной синтаксической информации для заполнения
	//
	//
	//SS::Syntax::DataTypes::S_bf_ui_TypeOfEndingObjectForVerb	o_bf_ui_TypeOfEndingObjectForVerb ;
	//SS::Core::Features::Types::VerbConstruction				oVerbConstruction = SS::Core::Features::Values::TypeOfEndingObject::vcOneObjectPlus;

	//SS::Core::Features::CSyntaxFeatureApriori			oSyntaxFeatureApriori;

	//	
	//for(int i = 1; i < 5; i++ )
	//{
	//	TSyntaxInfo iSyntaxInfo;
	//	oSyntaxFeatureApriori.m_TypeOfEndingObject	= 
	//		SS::Syntax::DataTypes::toeoAdv||
	//		SS::Syntax::DataTypes::toeoIng||
	//		SS::Syntax::DataTypes::toeoObject;
	//	oSyntaxFeatureApriori.m_VerbConstruction			= oVerbConstruction;
	//	pIAMConverterSyntax->Code( 
	//		SS::MORPHOLOGY_ANALYSIS::MorphoTypes::postVerb,
	//		&oSyntaxFeatureApriori,
	//		&iSyntaxInfo);
	//	list_ofSyntaxInfoIn.push_back(iSyntaxInfo);
	//}
	//	

	//list_ofSyntaxInfoIn.sort();

	//
	///// внесение информации в SQL
	//for(int i = ciMinIDSource, int j = 0; i < ciMaxIDSource; i++ , j++)
	//{

	//	oIDSource.uchIDSuffics	= 0;
	//	oIDSource.ushIDRoot		= i;
	//	pICoSeDiManagerSQL->GetSyntaxAnalysis()->SetIDSource(oIDSource);	
	//	pICoSeDiManagerSQL->GetSyntaxAnalysis()->SetDictionaryNumber(j);
	//	pICoSeDiManagerSQL->GetSyntaxAnalysis()->Add(&list_ofSyntaxInfoIn);
	//}

	///// проверка правильности внесения информации в SQL
	//for(int i = ciMinIDSource, int j = 0; i < ciMaxIDSource; i++ , j++)
	//{
	//	std::list<TSyntaxInfo> list_ofSyntaxInfoOut;
	//
	//	oIDSource.uchIDSuffics	= 0;
	//	oIDSource.ushIDRoot		= i;
	//	pICoSeDiManagerSQL->GetSyntaxAnalysis()->SetIDSource(oIDSource);	
	//	pICoSeDiManagerSQL->GetSyntaxAnalysis()->SetDictionaryNumber(j);
	//	pICoSeDiManagerSQL->GetSyntaxAnalysis()->Get(&list_ofSyntaxInfoOut);

	//	list_ofSyntaxInfoOut.sort();
	//}
	//// перенесение информации из SQL в DBMS

	///// удаление информации из SQL
	//for(int i = ciMinIDSource, int j = 0; i < ciMaxIDSource; i++, j++ )
	//{
	//	oIDSource.uchIDSuffics	= 0;
	//	oIDSource.ushIDRoot		= i;
	//	pICoSeDiManagerSQL->GetSyntaxAnalysis()->SetIDSource(oIDSource);	
	//	pICoSeDiManagerSQL->GetSyntaxAnalysis()->SetDictionaryNumber(j);
	//	pICoSeDiManagerSQL->GetSyntaxAnalysis()->Delete();
	//}


// Тест DBMS	

	SS::Interface::Core::CoSeDi::ICoSeDiManager* pICoSeDiManagerDBMS;
	pICoSeDiManagerDBMS = 
		(SS::Interface::Core::CoSeDi::ICoSeDiManager*)
		GetLoadManager()->GetInterface(L"./core/linguistics/CoSeDi.dll", CLSID_ICoSeDiManager, IID_ICoSeDiManager);

	if(pICoSeDiManagerDBMS == NULL)
		SS_THROW(L"");	

	pICoSeDiManagerDBMS->SetLoadManager(GetLoadManager());

	
	/// проверка правильности внесения информации в DBMS
	/*for(int i = ciMinIDSource, int j = 0; i < ciMaxIDSource; i++, j++ )
	{
		std::list<TSyntaxInfo> list_ofSyntaxInfoOut;
	
		oIDSource.uchIDSuffics	= 0;
		oIDSource.ushIDRoot		= i;
		pICoSeDiManagerDBMS->GetSyntaxAnalysis()->SetIDSource(oIDSource);	
		pICoSeDiManagerDBMS->GetSyntaxAnalysis()->SetDictionaryNumber(j);
		pICoSeDiManagerDBMS->GetSyntaxAnalysis()->Get(&list_ofSyntaxInfoOut);

		list_ofSyntaxInfoOut.sort();

	}*/

	std::list<TSyntaxInfo> list_ofSyntaxInfoOut;
	oIDSource = 21;
	pICoSeDiManagerDBMS->GetSyntaxAnalysis()->SetIDSource(oIDSource);	
	pICoSeDiManagerDBMS->GetSyntaxAnalysis()->SetDictionaryNumber((SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber)5);
	pICoSeDiManagerDBMS->GetSyntaxAnalysis()->Get(&list_ofSyntaxInfoOut);

	list_ofSyntaxInfoOut.push_back(1);
	pICoSeDiManagerDBMS->GetSyntaxAnalysis()->Add(&list_ofSyntaxInfoOut);


	CoUninitialize();
}

#endif


