#include "StdAfx.h"
#include ".\testrunner.h"
#include <iostream>
#include <ctime>

#include "../ASSInterface/Construction.h"
#include "../ASSInterface/ISyntaxConstructions.h"

#include "../[libs]/TestHelper.h"
#include "../[libs]/CoutConvert.h"

using namespace std;
using namespace SS::Lib;
   
SS::UnitTests::AMConverter::CTestRunner::CTestRunner(void)
{
   DEBUG_MSG("CTestRunner::CTestRunner()");
}

SS::UnitTests::AMConverter::CTestRunner::~CTestRunner(void)
{
   DEBUG_MSG("CTestRunner::~CTestRunner()");
}

void SS::UnitTests::AMConverter::CTestRunner::RandomFillFeatureCollection( SS::Core::Features::CBaseFeatureCollection &FC )
{
   SS::Core::Features::CBaseFeature *pBF = 0; 
   unsigned int Value;
   auto_ptr_ex<SS::Core::Features::CBaseType> pBT;

   for( unsigned int i = 0; i < FC.GetFeatureCount(); ++i )
   {
      pBF = FC.GetFeature(i);
      
      APL_ASSERT( pBF );

      pBT = pBF->GetBaseType();
      
      Value = 0;

      if( pBT->GetValueCount() == 0 ) //Integer Feature
      {
         Value = rand() % 100;
      }
      else
      {
         for( unsigned int j = 1; j < pBT->GetValueCount(); ++j )
         {
            Value |= (rand() % 2) << j;
         }
      }
      
      pBF->SetValue(Value);
   }
}

void SS::UnitTests::AMConverter::CTestRunner::TestConstructionObject()
{
   using namespace SS::SyntaxConstructions::Types;
      
   CConstructionObject CO1, CO2;
   std::vector<unsigned char> Buf1, Buf2;

   RandomFillFeatureCollection(CO1.m_MorphoFeatureAposteriori);
   RandomFillFeatureCollection(CO1.m_MorphoFeatureApriori);
   RandomFillFeatureCollection(CO1.m_ObjectFeature);
   RandomFillFeatureCollection(CO1.m_SemanticFeatureAposteriori);
   RandomFillFeatureCollection(CO1.m_SemanticFeatureApriori);
   RandomFillFeatureCollection(CO1.m_SyntaxFeatureApriori);
   RandomFillFeatureCollection(CO1.m_SyntaxFeatureAposteriori);

   cout << "Случайная фича: " << endl;
   cout << CO1.m_MorphoFeatureAposteriori.GetPrintableDataDescription() << endl;
   cout << CO1.m_MorphoFeatureApriori.GetPrintableDataDescription() << endl;
   cout << CO1.m_ObjectFeature.GetPrintableDataDescription() << endl;
   cout << CO1.m_SemanticFeatureAposteriori.GetPrintableDataDescription() << endl;
   cout << CO1.m_SemanticFeatureApriori.GetPrintableDataDescription() << endl;
   cout << CO1.m_SyntaxFeatureApriori.GetPrintableDataDescription() << endl;
   cout << CO1.m_SyntaxFeatureAposteriori.GetPrintableDataDescription() << endl << endl;
   CO1.GetValue(Buf1); 
   CO2.SetValue(Buf1);
   CO2.GetValue(Buf2); 

   APL_ERROR( CO1 == CO2 );
   APL_ERROR( Buf1 == Buf2 );
}


void SS::UnitTests::AMConverter::CTestRunner::ShowFeature( int MorphoInfo, int SpecialMorpho, SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber DictNumber /*= SS::Dictionary::DATA_TYPE::NAMES::ednENominalName*/ )
{
   SS::Core::Features::CMorphoFeature MFTmp1;

   SS::Interface::Core::AMConverter::IAMConverterMorpho  *pAMConverter = 
      (SS::Interface::Core::AMConverter::IAMConverterMorpho*)
      GetLoadManager()->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);


   APL_ERROR( pAMConverter );
   cout << "MorphoInfo,SpecialMorpho: " << MorphoInfo << "," << SpecialMorpho << endl;

   pAMConverter->EnCode( 
      DictNumber, 
      MorphoInfo, 
      SpecialMorpho,
      &MFTmp1
      );

   cout << "EnCode:                   " << MFTmp1.GetPrintableDataDescription() << endl;
}

void SS::UnitTests::AMConverter::CTestRunner::TestFeature( const SS::Core::Features::CMorphoFeature &MF )
{

   int TmpMorphoInfo = 0;
   int TmpSpecialMorpho = 0;
   SS::Core::Features::CMorphoFeature MFTmp1, MFTmp2;
   SS::Interface::Core::AMConverter::IAMConverterMorpho  *pAMConverter = 
      (SS::Interface::Core::AMConverter::IAMConverterMorpho*)
      GetLoadManager()->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);


   APL_ERROR( pAMConverter );
                                                                          
   cout << "Изначально:               " << MF.GetPrintableDataDescription() << endl;

   pAMConverter->Code( 
      SS::Dictionary::DATA_TYPE::NAMES::ednENominalName,
      &MF,
      &TmpMorphoInfo, 
      &TmpSpecialMorpho
      );

   cout << "MorphoInfo,SpecialMorpho: " << TmpMorphoInfo << "," << TmpSpecialMorpho << endl;

   pAMConverter->EnCode( 
      SS::Dictionary::DATA_TYPE::NAMES::ednENominalName, 
      TmpMorphoInfo, 
      TmpSpecialMorpho,
      &MFTmp1
      );

   cout << "Code, EnCode:             " << MFTmp1.GetPrintableDataDescription() << endl;

   SS::Dictionary::Types::TMorphoInfo MorphoInfoOrig = TmpMorphoInfo;
   SS::Dictionary::Types::TSpecialMorpho SpecialMorphoOrig = TmpSpecialMorpho;
   pAMConverter->EnCode( 
      SS::Dictionary::DATA_TYPE::NAMES::ednENominalName, 
      MorphoInfoOrig, 
      SpecialMorphoOrig,
      &MFTmp2
      );

   cout << "Code, EnCode CAST:        " << MFTmp2.GetPrintableDataDescription() << endl;
}

void SS::UnitTests::AMConverter::CTestRunner::TestSyntaxConstruction()
{
   cout << APL_LINE << "BEGIN TestSyntaxConstruction" << endl;
   using namespace SS::Interface::Core::SyntaxConstructions; 

   ISyntaxConstructionsManager *pEng = 0, *pRus = 0;

   pEng = (ISyntaxConstructionsManager*)
      GetLoadManager()->GetInterface(L"./core/linguistics/SyntaxConstructions.dll", CLSID_ISemanticConstructionManagerEng, IID_ISyntaxConstructionManager );
   
   APL_ERROR( pEng );

   pRus = (ISyntaxConstructionsManager*)
      GetLoadManager()->GetInterface(L"./core/linguistics/SyntaxConstructions.dll", CLSID_ISemanticConstructionManagerRus, IID_ISyntaxConstructionManager );

   APL_ERROR( pRus );

   cout <<  "pEng->ToBinary()" << endl;
   APL_ERROR( pEng->ToBinary() );
   
   cout <<  "pRus->ToBinary()" << endl;
   APL_ERROR( pRus->ToBinary() );

   cout << "END TestSyntaxConstruction" << endl << APL_LINE;
}


void SS::UnitTests::AMConverter::CTestRunner::TestAMConverter()
{
   cout << APL_LINE << "BEGIN TestAMConverter" << endl;
   cout << "Размер PartOfSpeechTypes: " << sizeof(SS::Core::Features::Types::PartOfSpeechTypes) << endl;

   srand( (unsigned)time( NULL ) );

   SS::Core::Features::CMorphoFeature MorphoFeature1, MorphoFeature2, MorphoFeature3;


   cout << "После CMorphoFeature():   " << MorphoFeature1.GetPrintableDataDescription() << endl;

   cout << APL_LINE;

   ShowFeature( 28781, 193, SS::Dictionary::DATA_TYPE::NAMES::ednENominalName );
   cout << APL_LINE;

   ShowFeature( 30746, 581, SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus );
   cout << APL_LINE;


   //      ShowFeature( 28781, 193 );
   //      cout << APL_LINE;

   MorphoFeature1.m_PartOfSpeechTypes.Assign( SS::Core::Features::Values::PartOfSpeechTypes.postPronoun );
   MorphoFeature1.m_CaseType.Assign( SS::Core::Features::Values::CaseType.ctNominative );
   MorphoFeature1.m_GenderType.Assign( SS::Core::Features::Values::GenderType.gtMasculine );
   MorphoFeature1.m_NumberType.Assign( SS::Core::Features::Values::NumberType.ntSingle );
   MorphoFeature1.m_PersonType.Assign( SS::Core::Features::Values::PersonType.ptThirdFace  );
   MorphoFeature1.m_PronounClass.Assign( SS::Core::Features::Values::PronounClass.pncIndividual );
   MorphoFeature1.m_PronounGroup.Assign( SS::Core::Features::Values::PronounGroup.pngNoun );

   TestFeature( MorphoFeature1 );
   cout << APL_LINE;

   MorphoFeature2.m_PartOfSpeechTypes.Assign( SS::Core::Features::Values::PartOfSpeechTypes.postPronoun );
   MorphoFeature2.m_CaseType.Assign( SS::Core::Features::Values::CaseType.ctGenitive );
   MorphoFeature2.m_GenderType.Assign( SS::Core::Features::Values::GenderType.gtMasculine );
   MorphoFeature2.m_NumberType.Assign( SS::Core::Features::Values::NumberType.ntSingle );
   MorphoFeature2.m_PersonType.Assign( SS::Core::Features::Values::PersonType.ptThirdFace  );
   MorphoFeature2.m_PronounClass.Assign( SS::Core::Features::Values::PronounClass.pncPossessive );
   MorphoFeature2.m_PronounGroup.Assign( SS::Core::Features::Values::PronounGroup.pngAdjective );

   TestFeature( MorphoFeature2 );
   cout << APL_LINE;

   MorphoFeature3.m_PartOfSpeechTypes.Assign( SS::Core::Features::Values::PartOfSpeechTypes.postPronoun );
   MorphoFeature3.m_CaseType.Assign( SS::Core::Features::Values::CaseType.ctGenitive );
   MorphoFeature3.m_GenderType.Assign( SS::Core::Features::Values::GenderType.gtMasculine );
   MorphoFeature3.m_NumberType.Assign( SS::Core::Features::Values::NumberType.ntSingle );
   MorphoFeature3.m_PersonType.Assign( SS::Core::Features::Values::PersonType.ptThirdFace  );
   MorphoFeature3.m_PronounClass.Assign( SS::Core::Features::Values::PronounClass.pncPossessive );
   MorphoFeature3.m_PronounGroup.Assign( SS::Core::Features::Values::PronounGroup.pngNoun );

   TestFeature( MorphoFeature3 );
   cout << APL_LINE;

   ShowFeature( 30746, 581 );
   cout << APL_LINE;

   ShowFeature( 30746, 654 );
   cout << APL_LINE;

   ShowFeature( 30746, 653 );
   cout << APL_LINE;

   //ShowFeature( 28781, 193 );
   //cout << APL_LINE;

   cout << "END TestAMConverter" << endl << APL_LINE;
}


void SS::UnitTests::AMConverter::CTestRunner::Run()
{
 	cout << "Вход CTestRunner::Run() дата и время сборки модуля " << __TIMESTAMP__ << endl;
   
   #ifdef NDEBUG
      cout << "NDEBUG подключён" << endl;
   #else
      cout << "NDEBUG не подключён" << endl;
   #endif

	APL_TRY()
	{      
      cout << APL_LINE << "BEGIN AMConverterTest" << endl;
      CoInitialize(NULL);

      
      //TestSyntaxConstruction();
      //TestAMConverter();
      TestConstructionObject();
      //TestConstructionObject();
      //TestConstructionObject();

      cout << "END AMConverterTest" << endl << APL_LINE;
   }			
	APL_CATCH()

	cout << "Выход CTestRunner::Run()" << endl;
}

ULONG SS::UnitTests::AMConverter::CTestRunner::Release()
{
	delete this;
	return 0;
}

HRESULT SS::UnitTests::AMConverter::CTestRunner::QueryInterface(REFIID pIID, void** pBase)
{
	if(pIID==IID_TestRunner)
		*pBase=(SS::Interface::UnitTests::ITestRunner*)this;
	else
		if (pIID==IID_Base )
			*pBase = (IBase*)this;
	else
		*pBase=NULL;

	return S_OK;
}

