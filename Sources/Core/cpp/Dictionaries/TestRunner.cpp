#include "stdafx.h"
#ifdef _SS_UNITTESTS

#include "TestAuxiliary.h"
#include "TestSmartDictionary.h"
#include "PerfomanceTestRunner.h"
#include "../[libs]/Progress.hpp"
#include "../[libs]/_Paths.h"
#include "MinimalHypothesisTree.h"

#include "TestRunner.h"
#include <ctime>


namespace SS{ 
namespace UnitTests{ 
namespace Dictionary{
using namespace std;

//Старая функция преобразования	Римских в арабские
unsigned int RomanNumberStrToInt(const char * pszRomanNumberStr)  
{

	unsigned int iRetValue = 0;        // Value for return
	unsigned int iHlpValue;

	SS_TRY
	{
		int iValueAndPriority;    // Value and Priority
		int iCurPriority=1000000; // Current Digit Priority (initial MAX value)

		// Here must be correct "clc" processing
		// Among upper-case Roman digits "clc" means <1000> like "M" does.

		do   
		{

			switch(toupper(*pszRomanNumberStr))   
			{
			case 'I': iValueAndPriority =1;    break;
			case 'V': iValueAndPriority =5;    break;
			case 'X': iValueAndPriority =10;   break;
			case 'L': iValueAndPriority =50;   break;
			case 'C': iValueAndPriority =100;  break;
			case 'D': iValueAndPriority =500;  break;
			case 'M': iValueAndPriority =1000; break;

				//case ' ': // Comment this case-block if spaces are not accepted inside <RomanNumberStr>
				//	pszRomanNumberStr++;
				//	continue;
				//break;

			case '\0': // Correct variations of end of <RomanNumberStr>
			case '\r':
			case '\n':
			case '\t':
				return(iRetValue);
				//			break;

			default:
				return(0);  // It is not a RomanNumber
				//			break;
			}

			if(iRetValue==0)   
			{
				iRetValue    = iValueAndPriority; // iValueAndPriority acts as VALUE
				iCurPriority = iValueAndPriority; // iValueAndPriority acts as PRIORITY
			} else   
			{
				if(iCurPriority<iValueAndPriority)   
				{   // We use <iValueAndPriority> as a priority
					if((iHlpValue=RomanNumberStrToInt(pszRomanNumberStr))==0)  return(0);
					iRetValue =  iHlpValue - iRetValue;
					break;
				} else if(iCurPriority==iValueAndPriority)   
				{ // We use <iValueAndPriority> as a priority
					iRetValue += iValueAndPriority;
				} else  
				{
					if((iHlpValue=RomanNumberStrToInt(pszRomanNumberStr))==0)  return(0);
					iRetValue += iHlpValue; 
					break;
				}
			}

			pszRomanNumberStr++;

		} while(1);

	}
	SS_CATCH(L"")

	return(iRetValue);
}

///////////////////////////////////////////////////////////////////////////////
// class CTestRunner
///////////////////////////////////////////////////////////////////////////////
void CTestRunner::TestIMorphologyAnalyser(Detail::IBasicMorphologyAnalyser *pMA, std::istream & InStream, std::ostream &OutStream)
{
	std::string InputStr;
	std::wstring CurWord;
	bool Exist;
	typedef std::list<TWordInfo> TWIList;
	TWIList WIList, WIList2;
	SWordInfo WordInfoTmp;
	wchar_t Word[SS::Constants::MAX_CHARS_IN_WORD];
	typedef	std::set<unsigned int> TDictionaryIndexesSet;
	TDictionaryIndexesSet DictionaryIndexesSet;

	while( std::getline(InStream, InputStr )  )
	{
		if( InputStr.empty() )
			continue;

		Convert( InputStr, CurWord );

		OutStream  << APL_LINE << "Проверяем в словаре: '" << CurWord; 
		Exist = pMA->IsExistWord( CurWord.c_str() );

		OutStream  <<	"' результат: "  << Exist << endl;

		if( Exist )
		{
			APL_ERROR( pMA->GetWordInfo(CurWord.c_str(), CurWord.c_str(), false, &WIList) );
			APL_ERROR( !WIList.empty()  );

			OutStream << "GetWordInfo: " << endl;
			Print( OutStream, WIList );
			OutStream << endl << endl;

			DictionaryIndexesSet.clear();

			for( TWIList::iterator I = WIList.begin(); I != WIList.end();	++I )
				for( TWordInfo::iterator J = I->begin(); J != I->end(); ++J )
				{
					APL_ERROR( pMA->GetWordInfo(&J->m_WordIndex, &WordInfoTmp) ); 

					APL_ERROR( Cmp(WordInfoTmp, *J, true) );
					APL_WARNING( Cmp(WordInfoTmp, *J, false) );

					APL_ERROR( pMA->GetWord(&J->m_WordIndex, Word) );
					APL_ERROR( StrCmpI(CurWord.c_str(), Word) == 0 );
					APL_WARNING( CurWord == Word );

					unsigned int DictionaryIndex = J->m_WordIndex.m_DictIndex.GetDictionaryIndex();
					
					//Находим из нашего списка все уникальные IDSource и выводим полную информацию
					if( DictionaryIndexesSet.find(DictionaryIndex) == DictionaryIndexesSet.end() )
					{
						APL_ERROR( pMA->GetWordFormsByIDSource(&J->m_WordIndex.m_DictIndex, &WIList2) );
						APL_ERROR( !WIList2.empty() );

						//Один элемент из WIList2 должен быть равен *J
						bool CaseSensCmp = false, CaseInsensCmp = false;
						for( TWIList::iterator K = WIList.begin(); K != WIList.end();	++K )
							for( TWordInfo::iterator L = K->begin(); L != K->end(); ++L )
							{
								if( Cmp(*L, *J, true) )
								{
									APL_ERROR(!CaseInsensCmp);
									CaseInsensCmp = true;
								}

								if( Cmp(*L, *J, false) )
								{
									APL_ERROR(!CaseSensCmp);
									CaseSensCmp = true;
								}
							}
						
						APL_ERROR( CaseInsensCmp );
						APL_WARNING( CaseSensCmp );

						OutStream <<  "GetWordFormsByIDSource: " << DictionaryIndexDescription(J->m_WordIndex.m_DictIndex) 
							<< " " << J->m_WordIndex.m_DictIndex.GetId() << endl;

						Print( OutStream, WIList2 );
						OutStream << endl << endl;

						DictionaryIndexesSet.insert(DictionaryIndex);
					}
				}
		}
		else
		{
			APL_ERROR( !pMA->GetWordInfo(CurWord.c_str(), CurWord.c_str(), false, &WIList) );
			APL_ERROR( WIList.empty()  );
		}

		OutStream  << APL_LINE;
	}
}
///////////////////////////////////////////////////////////////////////////////


void CTestRunner::TestCBaseDictionary()
{
	//cout << APL_LINE << "BEGIN TestCBaseDictionary" << endl;
	//
	//cout << "Инстанцируем CBaseDictionary" << endl;
	//CBaseDictionary BaseDictionary;
	//
   //BaseDictionary.Init(m_pDataBase, L"..\\..\\..\\Database\\MorphoEnglish",  DATA_TYPE::NAMES::ednENominalName, DATA_TYPE::NAMES::ednEProperName );
	//cout << "Создаём словари" << endl;
	//BaseDictionary.ToBinary();

	//cout << "Загружаем CBaseDictionary" << endl;
	//APL_ERROR( BaseDictionary.Load() );
	//
	//ifstream IStr( "..\\test.txt" );
	//cout << "Начинаем тестирование CBaseDictionary" << endl;

	//TestIMorphologyAnalyser( &BaseDictionary, IStr, cout );
	//
	//cout << "END TestCBaseDictionary" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestNumericDictionary()
{
	cout << APL_LINE << "BEGIN TestNumericDictionary" << endl;

	cout << "Инстанцируем CNumberDictionary" << endl;

	SS::Interface::Core::AMConverter::IAMConverterMorpho  *pAMConverter = 
		(SS::Interface::Core::AMConverter::IAMConverterMorpho*)
		GetLoadManager()->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);

	APL_ERROR(pAMConverter != 0);

	CNumberDictionary Dictionary(pAMConverter);

	ifstream IStr( "..\\numberdict.txt" );
	cout << "Начинаем тестирование CNumberDictionary" << endl;

	TestIMorphologyAnalyser( &Dictionary, IStr, cout );

	cout << "END TestNumericDictionary" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestRomanNumericDictionary()
{
	cout << APL_LINE << "BEGIN TestRomanNumericDictionary" << endl;

	cout << "Инстанцируем CRomanNumberDictionary" << endl;

	SS::Interface::Core::AMConverter::IAMConverterMorpho  *pAMConverter = 
		(SS::Interface::Core::AMConverter::IAMConverterMorpho*)
		GetLoadManager()->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);

	APL_ERROR(pAMConverter != 0);

	CRomanNumberDictionary Dictionary(pAMConverter);

	ifstream IStr( "..\\romannumberdict.txt" );
	cout << "Начинаем тестирование CRomanNumberDictionary" << endl;

	TestIMorphologyAnalyser( &Dictionary, IStr, cout );

	cout << "END TestRomanNumericDictionary" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestSymbolsDictionary()
{
	cout << APL_LINE << "BEGIN TestSymbolsDictionary" << endl;

	cout << "Инстанцируем CSymbolsDictionary" << endl;

	SS::Interface::Core::AMConverter::IAMConverterMorpho  *pAMConverter = 
		(SS::Interface::Core::AMConverter::IAMConverterMorpho*)
		GetLoadManager()->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);

	APL_ERROR(pAMConverter != 0);

	CSymbolsDictionary Dictionary(pAMConverter);

	ifstream IStr( "..\\symbolsdict.txt" );
	cout << "Начинаем тестирование CSymbolsDictionary" << endl;

	TestIMorphologyAnalyser( &Dictionary, IStr, cout );

	cout << "END TestSymbolsDictionary" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestCMorphoTypesCollection(std::istream & InStream, std::ostream &OutStream)
{
	cout << APL_LINE << "BEGIN TestCMorphoTypesCollection" << endl;

	cout << "Инстанцируем CMorphoTypesCollection" << endl;
	Edit::CMorphoTypesCollection	MorphoTypesCollection;
	MorphoTypesCollection.Init(m_pDataBase);

	cout << "Начинаем тестирование CMorphoTypesCollection" << endl;

	//Генерим большой IDType с которы будем в последствии работать
	TIDType BaseIDType = std::numeric_limits<TIDType>::max() - 100;
	const int TypesCount = 3; //Сколько последовательных значений BaseIDType будем использовать
	const int VecCount = 1000;
	const int IDFormCount = 10;
	const int MorphoInfoCount = 20;
	
	std::list<SMorphoType> DBMTList, TmpMTList;  
	SMorphoType MorphoTypeTmp;

	for( int i = 0; i < TypesCount; ++i )
	{
		MorphoTypesCollection.GetElements( BaseIDType + i,	&DBMTList );

		if( !DBMTList.empty() )
		{
			APL_WARNING( !DBMTList.empty() );
			MorphoTypesCollection.DeleteType(BaseIDType + i);

			MorphoTypesCollection.GetElements( BaseIDType + i,	&DBMTList );

			APL_ERROR( DBMTList.empty() );
		}
	}

	//Создаём список добавляемых элементов и за одно добавляем их в список в памяти
	for( int i = 0; i < TypesCount; ++i )
		for( int j = 0; j < VecCount; ++j )
		{
			MorphoTypeTmp.m_IDEnding = j;
			MorphoTypeTmp.m_IDForm = (j % IDFormCount)* 2;
			MorphoTypeTmp.m_MorphoTypeInfo.m_IDType = BaseIDType + i;
			MorphoTypeTmp.m_MorphoTypeInfo.m_MorphoInfo = (j % MorphoInfoCount) * 3;
			MorphoTypeTmp.m_SpecialMorpho = j * 4;

			TmpMTList.push_back( MorphoTypeTmp );
		}

	//Мы должны вставить не все записи
	APL_ERROR( MorphoTypesCollection.AddElements( &TmpMTList ) == E_INVALIDARG );

	for( int i = 0; i < TypesCount; ++i )
	{
		 MorphoTypesCollection.GetElements( BaseIDType + i,	&TmpMTList );
		 DBMTList.splice( DBMTList.end(), TmpMTList );
	}

	APL_ERROR( DBMTList.size() == TypesCount * IDFormCount * MorphoInfoCount );

	//Проходим по всем записям и проверяем значения
	for( std::list<SMorphoType>::const_iterator I = DBMTList.begin(); I != DBMTList.end(); ++I )
	{
		int j = I->m_IDEnding;

		APL_ERROR( I->m_IDEnding == j );
		APL_ERROR( I->m_IDForm == (j % IDFormCount)* 2 );
		APL_ERROR( I->m_MorphoTypeInfo.m_IDType >= BaseIDType && I->m_MorphoTypeInfo.m_IDType < BaseIDType + TypesCount );
		APL_ERROR( I->m_MorphoTypeInfo.m_MorphoInfo >= 0 && I->m_MorphoTypeInfo.m_MorphoInfo <= (MorphoInfoCount - 1) * 3 );
		APL_ERROR( I->m_SpecialMorpho == j * 4 );
	}

	//Все записи мы удалить не сможем
	APL_ERROR( MorphoTypesCollection.DeleteElements(&DBMTList) == E_INVALIDARG );
	DBMTList.clear();
	for( int i = 0; i < TypesCount; ++i )
	{
		MorphoTypesCollection.GetElements( BaseIDType + i,	&TmpMTList );
		DBMTList.splice( DBMTList.end(), TmpMTList );
	}

	APL_ERROR( DBMTList.size() == TypesCount * 1 * MorphoInfoCount ); //Должно остаться по одному IDForm

	//Ещё раз проверяем
	for( std::list<SMorphoType>::const_iterator I = DBMTList.begin(); I != DBMTList.end(); ++I )
	{
		int j = I->m_IDEnding;

		APL_ERROR( I->m_IDEnding == j );
		APL_ERROR( I->m_IDForm == (j % IDFormCount)* 2 );
		APL_ERROR( I->m_MorphoTypeInfo.m_IDType >= BaseIDType && I->m_MorphoTypeInfo.m_IDType < BaseIDType + TypesCount );
		APL_ERROR( I->m_MorphoTypeInfo.m_MorphoInfo >= 0 && I->m_MorphoTypeInfo.m_MorphoInfo <= (MorphoInfoCount - 1) * 3 );
		APL_ERROR( I->m_SpecialMorpho == j * 4 );
	}

	//Удаляем всё что было
	for( int i = 0; i < TypesCount; ++i )
	{
		MorphoTypesCollection.DeleteType(BaseIDType + i);
	}
	
	DBMTList.clear();
	for( int i = 0; i < TypesCount; ++i )
	{
		MorphoTypesCollection.GetElements( BaseIDType + i,	&TmpMTList );
		DBMTList.splice( DBMTList.end(), TmpMTList );
	}

	APL_ERROR( DBMTList.empty() ); 

	cout << "END TestCMorphoTypesCollection" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////
void CTestRunner::TestDBMS(std::istream & InStream, std::ostream &OutStream)
{
	cout << APL_LINE << "BEGIN TestDBMS" << endl;
	SS::Interface::Core::DBMS::IDataTable *pMorphoTypes;

	if( m_pDataBase->MoveToDataTable(_T("MorphoTypes")) !=  S_OK )
		SS_THROW(L"Open MorphoTypes");
	
	pMorphoTypes = m_pDataBase->OpenDataTable();
	
	if( pMorphoTypes == 0 )
		SS_THROW(L"pMorphoTypes == 0");		

	INT32 dbIDForm = 0;   
	INT32 dbIDType = std::numeric_limits<INT32>::max();
	INT32 dbIDEnding = 0;    
	INT32 dbPartOfSpeech = 0; 

	void *RecMapMorphoTypes[4] = {&dbIDForm, &dbIDType, &dbIDEnding, &dbPartOfSpeech};

	if( pMorphoTypes->AddInit(RecMapMorphoTypes) != S_OK )   
		SS_THROW( L"Ошибка при инициализации добавления в MorphoType" );

	for( int i = 0; i < 150; ++i )
	{
		dbIDForm = i;   
		dbIDEnding = i*2;    
		dbPartOfSpeech = i*3; 

		if( pMorphoTypes->Add() != S_OK )
			SS_THROW( L"Ошибка при добавлении в MorphoType" );
	}


	void *RecMapMorphoTypes2[4] = {};

	WORD FintID;


	if( pMorphoTypes->FindInitEx(_T("IDType"), &dbIDType, RecMapMorphoTypes2,&FintID) != S_OK )   
		SS_THROW( L"Ошибка при инициализации поиска в MorphoType по IDType" );

	int DelCount = 0;
	while( pMorphoTypes->Find(FintID) == S_OK )
	{
		if( pMorphoTypes->DeleteRecord(pMorphoTypes->GetFoundIdx(FintID)) != S_OK )
		{
			SS_THROW(L"Ошибка удаления записи из MorphoTypes");
		}	

		++DelCount;
	}

	cout << "DelCount = " << DelCount << endl;
		
	pMorphoTypes->FindCancel(FintID);
	pMorphoTypes->Close();

	cout << "END TestDBMS" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestDBMS_Smart(std::istream & InStream, std::ostream &OutStream)
{
	cout << APL_LINE << "BEGIN TestDBMS" << endl;
	CTablePtr pMorphoTypes;

	if( pMorphoTypes.Open( m_pDataBase, _T("MorphoTypes") ) != S_OK )
		SS_THROW(L"Open MorphoTypes");

	INT32 dbIDForm = 0;   
	INT32 dbIDType = 0;
	INT32 dbIDEnding = 0;    
	INT32 dbPartOfSpeech = 0; 

	void *RecMapMorphoTypes[4] = {&dbIDForm, &dbIDType, &dbIDEnding, &dbPartOfSpeech};

	if( pMorphoTypes->AddInit(RecMapMorphoTypes) != S_OK )   
		SS_THROW( L"Ошибка при инициализации добавления в MorphoType" );

	for( int i = 0; i < 150; ++i )
	{
		dbIDForm = i;   
		dbIDEnding = i*2;    
		dbPartOfSpeech = i*3; 

		if( pMorphoTypes->Add() != S_OK )
			SS_THROW( L"Ошибка при добавлении в MorphoType" );
	}

	void *RecMapMorphoTypes2[4] = {};

	CFindHelper MorphoTypesFind(pMorphoTypes);

	if( MorphoTypesFind.Init(_T("IDType"), &dbIDType, RecMapMorphoTypes2) != S_OK )   
		SS_THROW( L"Ошибка при инициализации поиска в MorphoType по IDType" );

	int DelCount = 0;
	while( MorphoTypesFind.Find() == S_OK )
	{
		if( pMorphoTypes->DeleteRecord(MorphoTypesFind.GetFoundIdx()) != S_OK )
		{
			SS_THROW(L"Ошибка удаления записи из MorphoTypes");
		}	
		
		++DelCount;
	}

	cout << "DelCount = " << DelCount << endl;

	cout << "END TestDBMS" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestCSource(std::istream &InStream, std::ostream &OutStream)
{
	cout << APL_LINE << "BEGIN TestCSource" << endl;

	Edit::CSource	Source; 
	cout << "Инстанцируем CMorphoTypesCollection" << endl;
	Edit::CMorphoTypesCollection	MorphoTypesCollection;
	MorphoTypesCollection.Init( m_pDataBase );

	std::list<SMorphoType> TmpMTList; 
	SMorphoType MorphoTypeTmp;
	TIDType BaseIDType = std::numeric_limits<TIDType>::max() - 200;

	SSource SourceItem, SourceItemTmp;
	Edit::CSource::TIDSource IDSource;
	wchar_t Word[SS::Constants::MAX_CHARS_IN_WORD];
	unsigned int PrevSourceID = std::numeric_limits<unsigned int>::max();
 
	cout << "Инстанцируем CSource" << endl;
	Source.Init( m_pDataBase, DATA_TYPE::NAMES::ednENominalName, _T("NominalSourceForm"), _T("NominalSourceToMorpho") );
   
	int i = 1;
	for( int j = 0; j < 10; ++j )
	{
		SourceItem.m_DictIndex.SetDictionaryNumber(DATA_TYPE::NAMES::ednENominalName);
		SourceItem.m_MorphoTypeInfo.m_IDType = BaseIDType + i;
		SourceItem.m_MorphoTypeInfo.m_MorphoInfo = i * 10 + 1;
		_itot( i * 10 + 2, Word, 4 );
		SourceItem.m_wSource = Word;
		
		Source.Add(	SourceItem, IDSource );
		SourceItem.m_DictIndex.SetId( IDSource );

		//Добавляем другую информацию
		TmpMTList.clear();
		MorphoTypeTmp.m_IDEnding = j;
		MorphoTypeTmp.m_IDForm = j * 2;
		MorphoTypeTmp.m_MorphoTypeInfo = SourceItem.m_MorphoTypeInfo;
		MorphoTypeTmp.m_SpecialMorpho = j * 4;
		TmpMTList.push_back( MorphoTypeTmp );

		MorphoTypesCollection.AddElements( &TmpMTList ); //Кое что можем и не добавить

		APL_ERROR( SourceItem.m_DictIndex.GetId() != PrevSourceID );
		PrevSourceID = SourceItem.m_DictIndex.GetId();

		APL_ERROR( Source.Get(IDSource, SourceItemTmp ) );
		
      if( !Cmp(SourceItemTmp, SourceItem) )
      {
         APL_ERROR(Cmp(SourceItemTmp, SourceItem));
      }

		Source.Delete(IDSource);	
		APL_ERROR( !Source.Get(IDSource, SourceItemTmp ) );
	}

	cout << "END TestCSource" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestCOneRoot(std::istream &InStream, std::ostream &OutStream)
{
	cout << APL_LINE << "BEGIN TestCOneRoot" << endl;

	cout << "Инстанцируем COneRoot" << endl;
	Edit::COneRoot OneRoot;
	OneRoot.Init( m_pDataBase, DATA_TYPE::NAMES::ednUnknown, _T("NominalOneRoot") ); 

	unsigned int IDSource = TDictionaryIndex::MaxId() - 100;
	std::list<TDictionaryIndex> TestList, TestListTmp;


	OneRoot.Get( IDSource, TestListTmp );
	
	if( !TestListTmp.empty() )
	{
		OneRoot.Delete(IDSource, TestListTmp);
		OneRoot.Get(IDSource, TestListTmp);
	}
	
	APL_ERROR( TestListTmp.empty() );

	for( int i = 0; i < 50; ++i )
	{
		TestList.push_back( TDictionaryIndex() );
		TestList.back().SetDictionaryNumber(DATA_TYPE::NAMES::ednUnknown);
		TestList.back().SetId( IDSource + i + 1 );
	}

	OneRoot.Add(IDSource, TestList);
	OneRoot.Get(IDSource, TestListTmp );

	APL_ERROR( TestList.size() == TestListTmp.size() );
	for( std::list<TDictionaryIndex>::const_iterator I = TestList.begin(); I != TestList.end(); ++I )
	{
		std::list<TDictionaryIndex>::const_iterator J;

		for( 
			J = TestListTmp.begin(); 
			J != TestListTmp.end() && I->GetDictionaryIndex() != J->GetDictionaryIndex(); 
			++J 
		) {;}

		APL_ERROR( J != TestListTmp.end() );
	}

	OneRoot.Delete(IDSource, TestList);
	
	OneRoot.Get( IDSource, TestListTmp );
	APL_ERROR( TestListTmp.empty() );

	cout << "END TestCOneRoot" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestRomanDigits(std::istream & InStream, std::ostream &OutStream)
{
	cout << APL_LINE << "BEGIN TestRomanDigits" << endl;

	typedef std::vector<TIArabic2Roman> TStorage;
	TStorage Storage;

	TIArabic2Roman Tmp;

	while( 1)
	{	
		InStream >> Tmp.Arabic >> Tmp.szRoman;

		if( InStream )
			Storage.push_back(Tmp);
		else
			break;
	}

	std::string sTmp;
	std::wstring wsTmp;
	unsigned int N;
	TCHAR Buf[100];

	for( size_t i = 0; i < Storage.size(); ++i )
	{	
		//Попробуем из римских в арабские
		sTmp = Storage[i].szRoman;
		Convert( sTmp, wsTmp );
		
		N = RomanNumberStrToInt(sTmp.c_str());
		if( N != Storage[i].Arabic )
		{
			OutStream << "RomanNumberStrToInt " << sTmp << " != " << Storage[i].Arabic << " = " << N << endl;
		}

		if( Detail::CRomanArabicConvertor::Roman2Arabic(wsTmp.c_str(), wsTmp.c_str() + wsTmp.size(), N) != wsTmp.c_str() + wsTmp.size() )
		{
			OutStream << "CRomanArabicConvertor::Roman2Arabic Not Convert "	<< wsTmp << endl;
		}
		else if( N != Storage[i].Arabic )
		{
			OutStream << "CRomanArabicConvertor::Roman2Arabic " << wsTmp << " != " << Storage[i].Arabic << " = " << N << endl;
		}

		//Попробуем из арабских в римские
		APL_ERROR( Detail::CRomanArabicConvertor::Arabic2Roman( Storage[i].Arabic, Buf, APL_ARRSIZE(Buf)) != 0 );

		if( Buf != wsTmp )
		{
			OutStream << "CRomanArabicConvertor::Arabic2Roman " << Storage[i].Arabic << " != " << wsTmp << " = " << Buf << endl;
		}
	}

	cout << "END TestRomanDigits" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestCNewWordsCacheImpl(DBMS::IDataTable *pTable, TDictNumber DictNumber, bool AddData, std::vector<unsigned int> &AddedIndexex )
{
   Detail::CNewWordsCache NewWordsCache( pTable, DictNumber );
   Detail::CNewWordsCache::CSourceData SourceData; 
   const Detail::CNewWordsCache::CSourceData *pSourceData;

   const int WordsCount = 10000;
   wchar_t Word[SS::Constants::MAX_CHARS_IN_WORD];
   wstring TmpStr;
   std::set<std::wstring> DeletedSet;
   
   if( AddData )
   {
      AddedIndexex.clear();
      AddedIndexex.resize(WordsCount);
      //Добавляем данные

      for( int i = 0; i < WordsCount; ++i )
      {
         TDictionaryIndex DI;
         DI.SetId(0);
         DI.SetDictionaryNumber(DictNumber);

         SourceData.SetIDSource( DI );
         SourceData.SetIDType( i * 10 + 2);
         _itot( (i % 100) * 10 + 3, Word, 10 );
         TmpStr = L"ЭТО ТЕСТ №: ";
         TmpStr += Word;

         SourceData.SetSource( TmpStr );

         NewWordsCache.DeleteByIDSource( DI );
         NewWordsCache.DeleteBySource( TmpStr );

         if( i % 5 == 0 ) continue;  

         NewWordsCache.Add( SourceData );
         APL_ERROR( SourceData.GetIDSource().GetId() != 0 );
         APL_ERROR( std::find( AddedIndexex.begin(), AddedIndexex.end(), SourceData.GetIDSource().GetId()) == AddedIndexex.end() );
         AddedIndexex[i] = ( SourceData.GetIDSource().GetId() );
         
         APL_ERROR( NewWordsCache.FindByIDSource(SourceData.GetIDSource()) != 0 );
      }

      //Удаляем некоторые из них
      for( int i = 0; i < WordsCount; ++i )
      {
         if( i % 3 == 0 )
         {
            TDictionaryIndex DI;
            DI.SetId(AddedIndexex[i]);
            DI.SetDictionaryNumber(DictNumber);

            NewWordsCache.DeleteByIDSource( DI );
         }

         if( i % 7 == 0 )
         {
            _itot( (i % 100) * 10 + 3, Word, 10 );
            TmpStr = L"Это тест №: ";
            TmpStr += Word;

            NewWordsCache.DeleteBySource( TmpStr );
         }
      }
   }

   for( int i = 0; i < WordsCount; ++i )
   {
      if( i % 7 == 0 )
      {
         _itot( (i % 100) * 10 + 3, Word, 10 );
         TmpStr = L"Это тест №: ";
         TmpStr += Word;

         DeletedSet.insert(TmpStr);
      }
   }

   if( pTable != 0 )
      NewWordsCache.Clear();

   APL_ERROR( AddedIndexex.size() == WordsCount );

   //Проверяем  данные
   for( int i = 0; i < WordsCount; ++i )
   {
      _itot( (i % 100) * 10 + 3, Word, 10 );
      TmpStr = L"Это тест №: ";
      TmpStr += Word;

      TDictionaryIndex DI;
      DI.SetId(AddedIndexex[i]);
      DI.SetDictionaryNumber(DictNumber);

      if( i % 5 == 0 || i % 3 == 0 || DeletedSet.find(TmpStr) != DeletedSet.end() ) 
      {
         APL_ERROR( NewWordsCache.FindBySource(TmpStr).End() );
         APL_ERROR( NewWordsCache.FindByIDSource(DI) == 0 );
      }
      else
      {
         Detail::CNewWordsCache::CCursor Curs;

         Curs = NewWordsCache.FindBySource(TmpStr);
         APL_ERROR( !Curs.End() );
         
         pSourceData = 0;
         
         //Находим Source с нужным нам IDType
         for( ; !Curs.End(); Curs.Next() )
         {
            APL_ERROR( NewWordsCache.FindByIDSource(Curs.Get().GetIDSource()) == &Curs.Get() );

            if( Curs.Get().GetIDType() == i * 10 + 2 )
            {
               APL_ERROR( pSourceData == 0 );
               pSourceData = &Curs.Get();
            }
         }

         APL_ERROR( pSourceData != 0 );
         
         APL_ERROR( pSourceData->GetIDSource().GetId() == AddedIndexex[i] );
         APL_ERROR( StrCmpI(pSourceData->GetSourceForFind().c_str(), pSourceData->GetSource().c_str()) == 0 );
         APL_ERROR( StrCmpI(TmpStr.c_str(), pSourceData->GetSource().c_str()) == 0 );
         APL_ERROR( pSourceData->GetIDType() == i * 10 + 2 );
      }
   }
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestCNewWordsCache(std::istream & InStream, std::ostream &OutStream)
{
   cout << APL_LINE << "BEGIN TestCNewWordsCache" << endl;
   
   cout << "Тестируем в памяти" << endl;
   std::vector<unsigned int> AddedIndexex;
   TestCNewWordsCacheImpl( 0, DATA_TYPE::NAMES::ednUnknown, true, AddedIndexex );
   AddedIndexex.clear();

   //Попробуем сделать тоже самое с таблицей
   DBMS::IDBManager *pDBManager = (DBMS::IDBManager *)GetLoadManager()->GetInterface(L"./core/linguistics/DBMS.dll" , CLSID_DBMS, IID_DBManager);
   APL_ERROR( pDBManager != 0 );

                               
   APL_ERROR( pDBManager->Init(L"..\\..\\..\\..\\Database\\Ndx_dbms.ini", L"..\\..\\..\\..\\Database\\ILPTest1\\") == S_OK );
   APL_ERROR( pDBManager->MoveToDataBase(L"DBMS") == S_OK );

   DBMS::IDataBase *pDataBase = pDBManager->OpenDataBase();
   APL_ERROR( pDataBase != 0 );

   {
      CTablePtr pTable;
      APL_ERROR( pTable.Open(pDataBase,  L"NewFormRussian") == S_OK );

      cout << "Тестируем в таблице" << endl;
      TestCNewWordsCacheImpl( pTable.Get(), DATA_TYPE::NAMES::ednUnknownRus, true, AddedIndexex );
   
      cout << "Тестируем в таблице без добавления" << endl;
      TestCNewWordsCacheImpl( pTable.Get(), DATA_TYPE::NAMES::ednUnknownRus, false, AddedIndexex );
   }
  
   pDataBase->Close();

   cout << "END TestCNewWordsCache" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::FillAbbreviationsTable()
{
   cout << APL_LINE << "BEGIN FillAbbreviationsTable" << endl;
   //Заполняет таблицу AbbreviationEnglish.SingleAbbreviations данными из файла
   INT32 IDSource = 0;      
   std::wstring Form;  
   std::wstring FormLower;  
   std::string TmpStr;
   INT32 IDType = 0;   
   INT32 IDForm = 0;  
   void *RecMap[5] = { &IDSource, &FormLower, &Form, &IDType, &IDForm };


   CTablePtr pTable;
   APL_ERROR( pTable.Open(m_pDataBaseAbbreviations,  L"SingleAbbreviations") == S_OK );
   APL_ERROR( pTable->Clear() == S_OK );
   APL_ERROR( pTable->AddInit(RecMap) == S_OK );

   std::ifstream flIn( "..\\Abbreviations.txt" );

   int Count = 0;
   
   for(;; ++Count)
   {
      flIn >> IDSource >> TmpStr >> IDType >> IDForm;

      if( !flIn )
         break;

      Convert( TmpStr, Form );

      ToLower( Form, FormLower );
       
      APL_ERROR( pTable->Add() == S_OK );
   }
   cout << "В таблицу AbbreviationEnglish.SingleAbbreviations, было добавлено записей " << Count << endl;
   cout << "END FillAbbreviationsTable" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestCAbbreviationsDictionaryBase()
{
   cout << APL_LINE << "BEGIN TestCAbbreviationsDictionary" << endl;

   cout << "Инстанцируем CAbbreviationsDictionary" << endl;

   CAbbreviationsDictionary::SInitStruct InitData[] = {
      {DATA_TYPE::NAMES::ednENominalName, _T("NominalSingleAbbreviations")}, 
      {DATA_TYPE::NAMES::ednEProperName, _T("ProperSingleAbbreviations")},
      {DATA_TYPE::NAMES::ednAbbreviationsEng, _T("MultipleAbbreviations")}
   };

   CAbbreviationsDictionary	BaseDictionary;
   BaseDictionary.Init(m_pDataBase, m_pDataBaseAbbreviations, InitData, InitData + APL_ARRSIZE(InitData) );

   ifstream IStr( "..\\AbbreviationsTest.txt" );
   cout << "Начинаем тестирование CAbbreviationsDictionaryBase" << endl;

   TestIMorphologyAnalyser( &BaseDictionary, IStr, cout );

   cout << "END TestCAbbreviationsDictionaryBase" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestThreads()
{
   cout << APL_LINE << "BEGIN TestThreads" << endl;

   CTestMultiThreaded MTObject;
   
   MTObject.CreateThreads();
   getch();
   MTObject.StopThreads();
 
   cout << "END TestThreads" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestIDictionary()
{
   CTestIDictionary TestIDictionary(GetLoadManager());

   TestIDictionary.NoThreadTest();
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestIDictionaryMultiThreaded()
{
   CTestIDictionary TestIDictionary(GetLoadManager());

   TestIDictionary.ThreadTest();
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestSmartDictionary( InitData::Languages::eLanguages Lang, CSmartDictionaryAdapter::TDictionaryTypes DictionaryType /*= CSmartDictionaryAdapter::EStandart*/)
{
   DBMS::IDataBase* pCurBase = 0;

   switch(Lang)
   {
   case InitData::Languages::English:
      pCurBase = m_pDataBase;
      break;
   
   case InitData::Languages::Russian:
      pCurBase = m_pDataBaseRus;
      break;
   };
   
   CTestSmartDictionary TSD( pCurBase, Lang, GetLoadManager());   
   TSD.NoThreadTest(DictionaryType);
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestSmartDictionaryMultiThreaded(InitData::Languages::eLanguages Lang, CSmartDictionaryAdapter::TDictionaryTypes DictionaryType /*= CSmartDictionaryAdapter::EStandart*/)
{
   DBMS::IDataBase* pCurBase = 0;

   switch(Lang)
   {
   case InitData::Languages::English:
      pCurBase = m_pDataBase;
      break;

   case InitData::Languages::Russian:
      pCurBase = m_pDataBaseRus;
      break;
   };
   
   CTestSmartDictionary TSD( pCurBase, Lang, GetLoadManager());
   TSD.ThreadTest(DictionaryType);
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestCSymbolCharacteristic()
{
   cout << APL_LINE << "BEGIN TestIDictionary" << endl;
   
   Detail::CSymbolCharacteristic<wchar_t, 5> SymbolCharacteristic;

   APL_ASSERT( InitData::g_hModuleCurrentDLL != 0 );
   SymbolCharacteristic.SetFlags( InitData::g_hModuleCurrentDLL, L"SymbolCharacteristic", IDR_ENGLISH_BASE_CHARACTERS, 1 );

   SymbolCharacteristic.GetFlags( L"Test" ); 
   cout << "SymbolCharacteristic.MemSize() " << (unsigned int)SymbolCharacteristic.MemSize() << endl;

   cout << "END TestIDictionary" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestCPartOfWord()
{
   cout << APL_LINE << "BEGIN TestCPartOfWord" << endl;

   SS::Interface::Core::CommonServices::IWorkRegister * pIWorkRegister = 
      (SS::Interface::Core::CommonServices::IWorkRegister*)
      GetLoadManager()->GetInterface(L"./core/LoadManager.dll", CLSID_WorkRegister, IID_WorkRegister);

   APL_ERROR( pIWorkRegister != 0 );

   std::wstring wDataBasePath = 
      pIWorkRegister->GetParameter(L"PathDataBase") ;

   DBMS::IDataBase *pMorphoRusDB = 0;

   m_pLingvoBaseManager->CreateOpenAndRegister( SS::MANAGER::Constants::c_szMorphoRus, &pMorphoRusDB);	

   APL_ERROR( pMorphoRusDB != 0 );

   //wDataBasePath += _T("MorphoRussian\\Ending");	 
   wDataBasePath += _T("MorphoRussian\\NominalSourceForm");	
   wDataBasePath += _T("\\");
   
   CTablePtr m_pEndingDT;

   APL_ERROR( m_pEndingDT.Open(pMorphoRusDB, _T("NominalSourceForm")) == S_OK );

   CPartOfWordsSearch PartOfWordsSearch;

   PartOfWordsSearch.Init( wDataBasePath.c_str(),  m_pEndingDT.Get() );
   PartOfWordsSearch.LoadIfFilesExists();
   
   std::vector< SPartOfWord > EndingVecBuf; 
   const wchar_t wcWord[200] = L"увеличением";
   int SearchPos = 8;//static_cast<int>(std::wcslen( wcWord )) - 1;

   PartOfWordsSearch.GetByForm(wcWord, SearchPos, &EndingVecBuf, TRUE);

   for( size_t i = 0; i < EndingVecBuf.size(); ++i )
   {
      cout << "PositionInWord: " << EndingVecBuf[i].PositionInWord << " Id: " << EndingVecBuf[i].Id << endl; 
   }

   cout << "END TestCPartOfWord" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void  CTestRunner::TestCDirectoryFileList()
{
   cout << APL_LINE << "BEGIN TestCDirectoryFileList" << endl;

   TCHAR *Exts[] = { _T("rar"), _T("zip"), _T("7z") };

   CDirectoryFileList DFL( _T("c:\\!MY_DOCS\\MY_PROGS"), Exts, Exts + APL_ARRSIZE(Exts), false );

   while( DFL.Next() )
   {
      cout << DFL.GetDir() << DFL.GetFile() << endl;
   }

   cout << "END TestCDirectoryFileList" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestPerfCWordsCashe()
{
   cout << APL_LINE << "BEGIN TestPerfCWordsCashe" << endl;

   CTestPerpformance TP( GetLoadManager() );

   //wstring Ext[] = { _T("h"), _T("cpp"), _T("hpp") };
   //wstring Path( L"c:\\program files\\Microsoft Visual Studio .NET 2003\\Vc7\\STLport-4.6.2\\" );
   
   //wstring Ext[] = { _T("txt") };
   //wstring Path( L"D:\\Ekaterina\\Мои документы\\Star_team\\Документы\\SS\\UnitTestsBin\\Core\\MainAnalyse" );

   wstring Ext[] = { _T("txt") };
   wstring Path( L"c:\\!my_docs\\MYtemp\\xc\\" );

   
   wstring *ExtFirst = Ext;
   wstring *ExtLast = Ext + APL_ARRSIZE(Ext);

   TP.FillNdxDBMS( Path, ExtFirst, ExtLast );
   TP.FindDummy( Path, ExtFirst, ExtLast );
   TP.FindNdxInDBMS( Path, ExtFirst, ExtLast );
   TP.FindNdxInCWewWordsCache( Path, ExtFirst, ExtLast );

   cout << "END TestPerfCWordsCashe" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////
void CTestRunner::TestFullWordsIDictionary()
{
   ofstream OutStream("TestFullWordsIDictionary.txt");
   
   CTestIDictionary TestIDictionary(GetLoadManager());

   TestIDictionary.TestFullWordsIDictionary(OutStream);
}
///////////////////////////////////////////////////////////////////////////////


void CTestRunner::TestPerfDictTree()
{
   cout << APL_LINE << "BEGIN TestPerfDictTree" << endl;

   CTestPerpformance TP( GetLoadManager() );
   
   wstring Ext[] = { _T("rpts") };
   wstring Path( L"..\\" );

   wstring *ExtFirst = Ext;
   wstring *ExtLast = Ext + APL_ARRSIZE(Ext);

   TP.TestIDictionaryTree( Path, ExtFirst, ExtLast );

   cout << "END TestPerfDictTree" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestPerfDictTreeText()
{
   cout << APL_LINE << "BEGIN TestPerfDictTree" << endl;

   CTestPerpformance TP( GetLoadManager() );

   wstring Ext[] = { _T("txt") };
   wstring Path( L"..\\New words\\" );

   wstring *ExtFirst = Ext;
   wstring *ExtLast = Ext + APL_ARRSIZE(Ext);

   TP.TestIDictionaryTreeFromText( Path, ExtFirst, ExtLast );

   cout << "END TestPerfDictTree" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

class CTestLoadManagerCreater
{
   //CLoadManagerCreater m_LoadManagerCreater;
   
   typedef HRESULT (*CREATE_INSTANCE)(const GUID* pGuid, void** pBase);

   HMODULE m_hLoadManager;
   CREATE_INSTANCE m_CreateInstanceFunc;
   unsigned int N;


public:

   void Test(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
   {
      //CDBMSPtrsManager m_DBMSPtrsManager;
      DBMS::IDataBase *m_pMorphoEngDB = 0;
      DBMS::IDataBase *m_pMorphoRusDB = 0;
      DBMS::IDataBase *m_pAbbreviationEngDB = 0;
      DBMS::IDataBase *m_pAbbreviationRusDB = 0;
      DBMS::IDataBase *m_pNdxDB = 0;

      using namespace DATA_TYPE::NAMES;

      APL_ASSERT_PTR( pLoadManager );

      SS::Interface::Core::ResourceManagers::ILingvoBaseManager* pLingvoBaseManager = 
         (SS::Interface::Core::ResourceManagers::ILingvoBaseManager*)
         pLoadManager->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_LingvoBaseManager);

      if( pLingvoBaseManager == 0 )
         SS_THROW( L"Ошибка при получении LingvoBaseManager" );

 
      //const wchar_t *NameActiveStorage = pLingvoBaseManager->GetNameActiveStorage();
      //bool InitNdxTables = NameActiveStorage != NULL && *NameActiveStorage != L'\0';

      //APL_ASSERT( 
      //   m_pMorphoEngDB == 0 && m_pMorphoRusDB == 0 && m_pAbbreviationEngDB == 0 &&
      //   m_pAbbreviationRusDB == 0 && m_pNdxDB == 0 
      //   );

      //pLingvoBaseManager->CreateOpenAndRegister( SS::MANAGER::Constants::c_szMorphoEng, &m_pMorphoEngDB );
      //pLingvoBaseManager->UnRegisterDeleteAndClose( SS::MANAGER::Constants::c_szMorphoEng, &m_pMorphoEngDB );
      
      //m_DBMSPtrsManager.SetLingvoBaseManager(pLingvoBaseManager);
      //m_DBMSPtrsManager.Clear();
      //m_DBMSPtrsManager.Register( SS::MANAGER::Constants::c_szMorphoEng, &m_pMorphoEngDB );
      //m_DBMSPtrsManager.Register( SS::MANAGER::Constants::c_szMorphoRus, &m_pMorphoRusDB );
      //m_DBMSPtrsManager.Register( SS::MANAGER::Constants::c_szAbbreviationEng, &m_pAbbreviationEngDB );
      //m_DBMSPtrsManager.Register( SS::MANAGER::Constants::c_szAbbreviationRus, &m_pAbbreviationRusDB );

      //if( InitNdxTables )
      //   m_DBMSPtrsManager.Register( SS::MANAGER::Constants::c_szNdxBase, &m_pNdxDB );

      //m_DBMSPtrsManager.Open();

      ////Находим путь к базе данных
      //SS::Interface::Core::CommonServices::IWorkRegister * pIWorkRegister = 
      //   (SS::Interface::Core::CommonServices::IWorkRegister*)
      //   pLoadManager->GetInterface(L"./core/LoadManager.dll", CLSID_WorkRegister, IID_WorkRegister);

      //if( pIWorkRegister == 0 )
      //   SS_THROW(L"Ошибка при получении WorkRegister");

      //std::wstring wDataBasePath = 
      //   pIWorkRegister->GetParameter(SS::Dictionary::DATA_TYPE::NAMES::REGISTER::csDataBasePath) ;

      //SS::Interface::Core::AMConverter::IAMConverterMorpho  *pAMConverter = 
      //   (SS::Interface::Core::AMConverter::IAMConverterMorpho*)
      //   pLoadManager->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);

      //if( pAMConverter == 0 )
      //   SS_THROW(L"ошибка при получении AMConverterMorpho");
   }

   CTestLoadManagerCreater(int): N(0) 
   {
      std::wstring Path = GetExeDirPath();

      ::SetCurrentDirectory(Path.c_str());  

      m_hLoadManager = ::LoadLibrary(_T("LoadManager.dll"));			//загрузка библиотеки и получение на неё указателя
      APL_ERROR( m_hLoadManager != NULL )

         m_CreateInstanceFunc = (CREATE_INSTANCE)GetProcAddress(m_hLoadManager, "CreateInstance" );	//получ указ. на ф-ю CreateInstance

      APL_ERROR( m_CreateInstanceFunc != NULL ); 
   }


   CTestLoadManagerCreater::~CTestLoadManagerCreater()
   {
	   ::FreeLibrary(m_hLoadManager);
      //---APL_ERROR( ::FreeLibrary(m_hLoadManager) )
   }


   bool Run( int ThreadNum, volatile LONG * )
   {
      Console.Write( ThreadNum, " Test ", ++N );

      //CLoadManagerCreater::TAutoPtr pLoadManager = m_LoadManagerCreater.Create();
      //Test(pLoadManager.get());
      //APL_ERROR((m_CreateInstanceFunc)(&CLSID_LoadManager, (void**)&pLoadManager) == S_OK && pLoadManager != NULL );

      ILoadManager *pLoadManager;
      (m_CreateInstanceFunc)(&CLSID_LoadManager, (void**)&pLoadManager);

      Test( pLoadManager );

      pLoadManager->Release();

      return true;
   }
};
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::Menu()
{
   struct SInfo
   {
      string Name;

      void (CTestRunner::*FuncPtr)();
   };

   SInfo Data[] = {
      {
         "Полнынй поиск по словам (\\..\\IDictionary_IMorphologyAnalyser.txt)",
         &CTestRunner::TestIDictionary
      },
      {
         "Сравнение данных в SQLServer и словаре  (Вход :\\..\\*.rpts (Query result), Выход: TestFullWordsIDictionary.txt )",
         &CTestRunner::TestFullWordsIDictionary
      },
      {
         "Тест производительности на данных из SQLServer'а (\\..\\*.rpts (Query result))",
         &CTestRunner::TestPerfDictTree
      },
      {
         "Тест производительности на тексте (\\..\\*.txts)",
         &CTestRunner::TestPerfDictTreeText
      }
   };

   int i;

   int CurN = 0;

   for(;;) 
   {
      cout << APL_LINE;
      for( i = 0; i < APL_ARRSIZE(Data); ++i ) 
      {
         cout << i + 1 << " - "<< Data[i].Name << endl;
      }

      cout << i + 1 << " - "<< "Выход" << endl;
      cout << APL_LINE;
      cout << "Что делать? :";
      
      //Очистка буфера
      ClearIStreamBuff( cin );

      if( cin >> CurN )
      {
         if( CurN > 0 && CurN <= APL_ARRSIZE(Data) )
            (this->*Data[CurN - 1].FuncPtr)();
         else if( CurN == APL_ARRSIZE(Data) + 1 )
            return;
         else
            cout << "Чаво, Чаво???" << endl;
      }
      else
      {
         cout << "Чаво, Чаво???" << endl;
      }
   } 
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestCNumberCombinatory()
{
   cout << APL_LINE << "BEGIN TestCNumberCombinatory" << endl;
   
   INumberCombinatory *pNC = (INumberCombinatory *)GetLoadManager()->GetInterface( DICTIONARIES_DLL_PATH, CLSID_INumberCombinatory, IID_INumberCombinatory);
   APL_ERROR( pNC != 0 );

   cout << "END TestCNumberCombinatory" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::CreateUndifinedNewWordsFile( const std::string &InFileName, const std::string &OutFileName )
{
   CFileWordReader FileWordReader(InFileName.c_str());
   std::ofstream flOut(OutFileName.c_str());
   CFileProgres<> FileProgress( FileWordReader.GetStream() );

   std::string Str;
   unsigned int i = 0;

   while( FileWordReader.Get(Str) && i < 300000 )
   {
      flOut << Str << ++i << endl;

      FileProgress.Update();
   }

   FileProgress.EndTask();
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::BuildMinimalHypothesisTree()
{
   cout << APL_LINE << "BEGIN BuildMinimalHypothesisTree" << endl;
   typedef unsigned int UI;

   {
      typedef CMorphoTypesSearchCreateManager::TPointer TMorphoTypesSearchPtr;
      
      IAMConverterMorpho  *pAMConverter = (IAMConverterMorpho*)GetLoadManager()->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);
      MinimalHypothesisTree::CFacade Facade;

      MinimalHypothesisTree::CFacade::TStatistic Statistic;

      Facade.Init(pAMConverter, m_pDataBase, DATA_TYPE::NAMES::ednENominalName);

      string FilePref( "..\\EnglishNewWord_N1" );
      size_t TotalWordCount, TotalCharCount;

      cout << "LoadFromFile " << (FilePref + ".rpt") <<  endl;
      Facade.LoadFromFile( (FilePref + ".rpt").c_str() );
      
      //cout << "SaveToFileStrict IN" << endl;
      //Facade.SaveToFileStrict(  (FilePref + "_IN_STRICT.rpt").c_str(), TotalWordCount, TotalCharCount );

      Facade.CalcStatisticData(Statistic);
      cout << "Узлов: " << (UI)Statistic.NodeCount << " Слов: " << (UI)Statistic.WordsCount << " Символов: " << (UI)Statistic.CharCount << " Типов: " << (UI)Statistic.TypesCount << endl;
      
      //APL_ERROR( TotalWordCount == Statistic.WordsCount && TotalCharCount == Statistic.CharCount );

      cout << "Optimize" << endl;
      Facade.Optimize();

      cout << "SaveToFileOriginal" << endl;
      Facade.SaveToFileOriginal( (FilePref + "_OUT_ORIGIN.rpt").c_str() );

      cout << "SaveToFileStrict" << endl;
      Facade.SaveToFileStrict(  (FilePref + "_OUT_STRICT.rpt").c_str(), TotalWordCount, TotalCharCount );
      
      Facade.CalcStatisticData(Statistic);
      cout << "Узлов: " << (UI)Statistic.NodeCount << " Слов: " << (UI)Statistic.WordsCount << " Символов: " << (UI)Statistic.CharCount << " Типов: " << (UI)Statistic.TypesCount << endl;

      APL_ERROR( TotalWordCount == Statistic.WordsCount && TotalCharCount == Statistic.CharCount );

      //cout << "TestTree " << (FilePref + ".rpt") <<  endl;
      //APL_ERROR( Facade.TestTree( (FilePref + ".rpt").c_str() ) );

      CTablePtr pTable;
      pTable.Open( m_pDataBase, L"NewWordsDeduction" );
      pTable->Clear();

      cout << "SaveToTable" << endl;
      Facade.SaveToTable( pTable.Get()  );

      //cout << "CMinimalHypothesisTree::SMorhpo::Count() " << (unsigned int)MinimalHypothesisTree::CMinimalHypothesisTree::SMorhpo::Count() << endl;
   }

   //cout << "CMinimalHypothesisTree::SMorhpo::Count() " << (unsigned int)MinimalHypothesisTree::CMinimalHypothesisTree::SMorhpo::Count() << endl;

   cout << APL_LINE << "END BuildMinimalHypothesisTree" << endl;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestUnicodeFileReader()
{
   cout << APL_LINE << "BEGIN TestUnicodeFileReader" << endl;

   wchar_t RgTest[5];

   CUnicodeFileReader FR;
   size_t CurLen;
   bool Rez;
   std::basic_string<wchar_t> Str, Str1, Str2, FileName(_T("..\\Ivanovo.txt"));

   APL_ERROR( FR.Open( FileName.c_str() ) == CUnicodeFileReader::EOROk );
   CUnicodeFileReader FR2(FileName.c_str());

   while( !FR.Eof() )
   {
      Rez = FR.Read(RgTest, APL_ARRSIZE(RgTest), CurLen, CUnicodeFileReader::SNewLineCharacter());
      
      cout << RgTest;
      Str1 += RgTest;

      if( Rez )
      {
         FR2.Read(Str2, CUnicodeFileReader::SNewLineCharacter());
         APL_ERROR( Str1 == Str2 );
         Str1.clear();

         cout << endl;
      }

      APL_ERROR( _tcslen(RgTest) == CurLen );
   }

   cout << APL_LINE << "END TestUnicodeFileReader" << endl;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestMinimalHypothesisTree()
{
   cout << APL_LINE << "BEGIN TestMinimalHypothesisTree" << endl;

   typedef std::list<TWordInfo> TWIList;

   std::stringstream Parser;
   ifstream FlIn( "..\\MinimalHypothesisTree.txt" );
   string InpStr;
   string Incorrect, Correct, Type;
   wstring WIncorrect, WCorrect;
   TWIList WIList1, WIList2;

   int TotalWords = 0, FullCompare = 0, PSCompare = 0;

   IMorphologyAnalyser *pMA = GetIDictionary(GetLoadManager())->GetMorphologyAnalyser();
   IAMConverterMorpho  *pAMConverter = (IAMConverterMorpho*)GetLoadManager()->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);

   APL_ERROR( pAMConverter != 0 );

   SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams = 
	   (SS::Interface::Core::MainAnalyse::IAnalyseParams*)
		GetLoadManager()->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_AnalyseParams, IID_AnalyseParams);

   APL_ERROR( pAnalyseParams != 0 );
   pAnalyseParams->SetAnalyseMod( eamIndexation );
   pAnalyseParams->SetCurrentSystemMode( ecsmGlobal );

   while( getline(FlIn, InpStr) )
   {
      ++TotalWords;

      Parser.clear();
      Parser.str( InpStr );

      if( !(Parser >> Incorrect >> Correct >> Type) )
         continue;

      pMA->NextText();
      Convert( Incorrect, WIncorrect );
      Convert( Correct, WCorrect );
      
      pMA->GetWordInfo( WCorrect.c_str(), &WIList1 );
      pMA->GetWordInfo( WIncorrect.c_str(), &WIList2 );

      WIList1.sort( STWordInfoMorphoCmp() );
      WIList1.unique( STWordInfoMorphoCmpEqual() );

      WIList2.sort( STWordInfoMorphoCmp() );
      WIList2.unique( STWordInfoMorphoCmpEqual() );

      if( 
         Mismatch(
            WIList1.begin(), WIList1.end(), 
            WIList2.begin(), WIList2.end(), 
            STWordInfoMorphoCmpEqual()
         ).first == WIList1.end() 
      )
      {
         ++FullCompare;

         cout << WCorrect <<  " == " <<  WIncorrect << ": ";
         
         for( TWIList::iterator I = WIList1.begin(); I != WIList1.end();	++I )
            for( TWordInfo::iterator J = I->begin(); J != I->end(); ++J )
               cout << J->m_MorphoTypeInfo.m_IDType << " ";

         cout << endl;
      }
      else
      {
         cout << WCorrect <<  " != " <<  WIncorrect << endl;
         cout << WCorrect << ":";
         Print( cout, WIList1 );
         cout << endl;
         cout << Incorrect << ":";
         Print( cout, WIList2 );
         cout << endl;
      }
     
      bool CheckList1Ok = false;

      for( TWIList::iterator I = WIList1.begin(); I != WIList1.end();	++I )
         for( TWordInfo::iterator J = I->begin(); J != I->end(); ++J )
            if( CheckMorphoInfo( pAMConverter, *J, Type ) )
               CheckList1Ok = true;
      
      APL_WARNING( CheckList1Ok );

      bool CheckList2Ok = false;

      for( TWIList::iterator I = WIList2.begin(); I != WIList2.end();	++I )
         for( TWordInfo::iterator J = I->begin(); J != I->end(); ++J )
            if( CheckMorphoInfo( pAMConverter, *J, Type ) )
               CheckList2Ok = true;
      
      if( CheckList2Ok ) ++PSCompare;

      APL_WARNING( CheckList2Ok );

      cout << endl;
   }

   cout << "Всего слов " << TotalWords << " Полных совпадений " << FullCompare << " Совпадений по частям речи " << PSCompare << endl;
   cout << APL_LINE << "END TestMinimalHypothesisTree" << endl;
}
///////////////////////////////////////////////////////////////////////////////


void CTestRunner::TestSourceDelete()
{
   cout << APL_LINE << "BEGIN TestSourceDelete" << endl;
   
   //typedef std::list<TWordInfo> TWIList;
   //TWIList WIList;
   //wstring SearchWord(L"Воззрение");
   //
   //IDictionary *pDictionary = GetIDictionary(m_pLoadManager);

   //if( !pDictionary->GetMorphologyAnalyser()->GetWordInfo(SearchWord.c_str(), &WIList, efzOnlySearch) )
   //{
   //   APL_ERROR( WIList.empty() );
   //   cout << "Слово " << SearchWord << " отсутсвует в словаре" << endl;
   //}
   //else
   //{
   //   APL_ERROR( !WIList.empty() );
   //   cout << "По слову " << SearchWord << " найденна  информация:" << endl;
   //   Print( OutStream, WIList );

   //   for( TWIList::iterator I = WIList.begin(); I != WIList.end();	++I )
   //      for( TWordInfo::iterator J = I->begin(); J != I->end(); ++J )

   //}

   cout << APL_LINE << "END TestSourceDelete" << endl;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestNewWordsList()
{
   cout << APL_LINE << "BEGIN TestNewWordsList" << endl;
   
   IWordsList *pWordsList = GetIDictionary(GetLoadManager())->GetWordList();
   TWordInfo CurWordInfo;

   TDictNumber rgDictNumbers[] = { DATA_TYPE::NAMES::ednUnknownEng, DATA_TYPE::NAMES::ednUnknownRus };

   for( size_t i = 0; i < APL_ARRSIZE( rgDictNumbers ); ++i )
   {
      cout << "Количество слов в словаре " << DictNumberDescription(rgDictNumbers[i]) << ": ";
      pWordsList->SetDictionaryNumber(rgDictNumbers[i]);
      cout << pWordsList->GetCount() << endl;

      if( pWordsList->MoveFirst() )
      {
         pWordsList->GetWord( &CurWordInfo );

         do 
         {
            Print(cout, CurWordInfo); 
            cout << endl;
         } 
         while( pWordsList->MoveNext(&CurWordInfo) );
      }
      
      cout << APL_LINE;
   }

   cout << APL_LINE << "END TestNewWordsList" << endl;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestSearchParam()
{
   cout << APL_LINE << "BEGIN TestSearchParam" << endl;
   
   IDictionary *pDictionary = GetIDictionary(GetLoadManager());
   SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams = 
	   (SS::Interface::Core::MainAnalyse::IAnalyseParams*)
		GetLoadManager()->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_AnalyseParams, IID_AnalyseParams);
   
   typedef std::list<SS::Dictionary::Types::TWordInfo> TWIList;
   typedef SS::Dictionary::Types::SWordInfo SWordInfo;

   APL_ERROR( pAnalyseParams != 0 );

   const wchar_t Word[] = L"новоеслово";
   TWIList WIList;
   SWordInfo WordInfo;

   pDictionary->GetMorphologyAnalyser()->GetWordInfo( Word, &WIList );
   pDictionary->GetMorphologyAnalyser()->NextText();
   pDictionary->GetMorphologyAnalyser()->GetWordInfo( &WIList.front().front().m_WordIndex, &WordInfo );
   pDictionary->GetMorphologyAnalyser()->GetWordInfo( Word, &WIList );

   cout << APL_LINE << "END TestSearchParam" << endl;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestFormDeleate()
{
   cout << APL_LINE << "BEGIN TestFormDeleate" << endl;

   using namespace DATA_TYPE::NAMES;
   
   IDictionary *pDictionary = GetIDictionary(GetLoadManager());
   SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams = 
	   (SS::Interface::Core::MainAnalyse::IAnalyseParams*)
		GetLoadManager()->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_AnalyseParams, IID_AnalyseParams);

   typedef std::list<SS::Dictionary::Types::TWordInfo> TWIList;
   typedef std::list<SMorphoType> TMTList;
   typedef SS::Dictionary::Types::SWordInfo SWordInfo;

   SMorphoType MT;
   TMTList MTList;
   TWIList WIList;
   const wchar_t Word[] = L"case";

   MT.m_IDEnding = 0;
   MT.m_IDForm = 3;
   MT.m_SpecialMorpho = 9;
   MT.m_MorphoTypeInfo.m_IDType = 10;
   MT.m_MorphoTypeInfo.m_MorphoInfo = 8197;

   TDictionaryIndex DI;
   DI.SetId(12226);
   DI.SetDictionaryNumber((TDictNumber)5);

   MTList.push_back(MT);
   

   pDictionary->GetMorphologyAnalyser()->GetWordFormsByIDSource(&DI, &WIList);
   cout << "Словоформы до удаления: " << endl;
   Print( cout, WIList );
   cout  << endl << APL_LINE;

   pDictionary->GetMorphologyAnalyser()->GetWordInfo( Word, &WIList );
   cout << "До удаления: " << endl;
   Print( cout, WIList );
   cout  << endl << APL_LINE;

   pDictionary->GetMorphologyAnalyser()->GetWordFormsByIDSource(&DI, &WIList);

   pDictionary->GetMorphoTypesCollection()->SetDictionaryNumber( ednENominalName );
   APL_ERROR( pDictionary->GetMorphoTypesCollection()->DeleteElements(&MTList) == S_OK );
   pDictionary->GetLoader()->ReLoad();
   pDictionary->GetMorphologyAnalyser()->GetWordInfo( Word, &WIList );
  
   cout << "После  удаления: " << endl;
   Print( cout, WIList );
   cout  << endl << APL_LINE;

   pDictionary->GetMorphologyAnalyser()->GetWordFormsByIDSource(&DI, &WIList);
   cout << "Словоформы после удаления: " << endl;
   Print( cout, WIList );
   cout  << endl << APL_LINE;

   cout << APL_LINE << "END TestFormDeleate" << endl;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestPerfDictDir()
{
   cout << APL_LINE << "BEGIN TestPerfDictDir" << endl;

   CTestPerpformance TP( GetLoadManager() );

   wstring Ext[] = { _T("txt") };
   wstring Path1( L"..\\speedtest\\fast\\" );
   wstring Path2( L"..\\speedtest\\slow\\" );

   wstring *ExtFirst = Ext;
   wstring *ExtLast = Ext + APL_ARRSIZE(Ext);

   TP.TestPerfForDir( Path1, ExtFirst, /*ExtLast*/ ExtFirst );
   TP.TestPerfForDir( Path2, ExtFirst, /*ExtLast*/ ExtFirst );

   cout << APL_LINE << "END TestPerfDictDir" << endl;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::TestGetWordForIUnit()
{
   CTestIDictionary TestIDictionary(GetLoadManager());

   TestIDictionary.TestGetWordForIUnit();
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::CreateCompoundFile()
{
   cout << APL_LINE << "BEGIN CreateCompoundFile" << endl;

   std::srand( (unsigned)time( NULL ) );
   
   const char *rgFileNames[] = { "../EnglishWord.txt", "../RussianWord.txt" };
   ifstream rgflIn[APL_ARRSIZE(rgFileNames)];
   ofstream flOut("../CompoundWords.txt");
   std::vector<std::string> rgStrings[APL_ARRSIZE(rgFileNames)];
   std::string StrTmp;
   size_t i, j, k;
   size_t CompundCount = 4;
   size_t WordsCount = 5000;

   for( i = 0; i < APL_ARRSIZE(rgflIn); ++i )
   { 
      rgflIn[i].open(rgFileNames[i]);
      APL_ERROR( rgflIn[i] );
   }  

   for( i = 0; i < APL_ARRSIZE(rgflIn); ++i )
   {
      while( rgflIn[i] >> StrTmp )
         rgStrings[i].push_back(StrTmp);
   }

   for( ; CompundCount > 1; --CompundCount )
   {
      for( i = 0; i < APL_ARRSIZE(rgflIn); ++i )
         for( j = 0; j < WordsCount; ++j )
         {
            StrTmp.clear();
            
            for( k = 0; k < CompundCount; ++k )
               StrTmp += rgStrings[i][rand() % rgStrings[i].size()];

            if( rand() % 10 == 0 )
               CharUpperBuffA( &StrTmp[0], 1 );

            if( StrTmp.size() >= Constants::MAX_CHARS_IN_WORD )
               continue;

            flOut << StrTmp << endl;
         }
   }
   
   cout << APL_LINE << "END CreateCompoundFile" << endl;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::Run()
{	
	cout << "Вход CTestRunner::Run() дата и время сборки модуля " << __TIMESTAMP__ << endl;
   
   #ifdef NDEBUG
      cout << "NDEBUG подключён" << endl;
   #else
      cout << "NDEBUG не подключён" << endl;
   #endif

   #ifdef NO_SYNCHRONIZATION
         cout << "NO_SYNCHRONIZATION подключён" << endl;
   #else
         cout << "NO_SYNCHRONIZATION не подключён" << endl;
   #endif

	APL_TRY()
	{         
      CoInitialize( NULL );

      cout << "Получаеем DBMS::IDataBase"	<< endl;
      
      m_pLingvoBaseManager = (SS::Interface::Core::ResourceManagers::ILingvoBaseManager*)

         GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_LingvoBaseManager);

		APL_ERROR(m_pLingvoBaseManager != 0);

      SS::Interface::Core::ResourceManagers::IBaseManager* pBaseManager = 
         (SS::Interface::Core::ResourceManagers::IBaseManager*)
         GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_BaseManager);

      APL_ASSERT(pBaseManager != 0);

      CDBMSPtrsManager DBMSPtrsManager(m_pLingvoBaseManager);

      DBMSPtrsManager.Register( SS::MANAGER::Constants::c_szMorphoEng, &m_pDataBase );
      DBMSPtrsManager.Register( SS::MANAGER::Constants::c_szMorphoRus, &m_pDataBaseRus );
      DBMSPtrsManager.Register( SS::MANAGER::Constants::c_szAbbreviationEng, &m_pDataBaseAbbreviations );
      DBMSPtrsManager.Open();

      //if( !CopyFile( L"C:\\SS\\SS\\Database\\NewWordForms1\\",  L"c:\\!my_docs\\MYtemp\\delme\\", FALSE ) )
      //{
      //   cout << GetDWErrorInfo( GetLastError() ) << endl;
      //}

      //TestCOneRoot( cin, cout );
      //TestCMorphoTypesCollection( cin, cout );
      //GetIDictionary(GetLoadManager())->GetLoader()->ToBinary();

      //TestCSource( cin, cout );
	   //TestCBaseDictionary();
		//TestNumericDictionary();
      //TestRomanDigits( ifstream("..\\roman.txt"), cout );
      //TestRomanNumericDictionary();
		//TestSymbolsDictionary();
		//TestCDictionaryGroup(IStr, cout);
		//TestDBMS_Smart( cin, cout );
      //TestCNewWordsCache( cin, cout );
      //FillAbbreviationsTable();
      //TestCAbbreviationsDictionaryBase();
      
      //TestSmartDictionary(InitData::Languages::Russian, CSmartDictionaryAdapter::EMinimalHypothesisTree);
      //TestSmartDictionaryMultiThreaded();
      
      //CreateCompoundFile();
      
      //TestSmartDictionary(InitData::Languages::English, CSmartDictionaryAdapter::EStandart);
      TestIDictionary();
      //TestGetWordForIUnit();
      //TestPerfDictTreeText();
      //TestPerfDictTree();


      //TestIDictionaryMultiThreaded();
      //TestThreads();
      //TestCTestDBMSDeadLocks();
      //TestCSymbolCharacteristic();
      //TestCPartOfWord();
      //TestCDirectoryFileList();
      //TestPerfCWordsCashe();
      //TestFullWordsIDictionary();
      //BuildMinimalHypothesisTree();
      //TestMinimalHypothesisTree();
      //TestUnicodeFileReader();
      //TestNewWordsList();
      //TestSearchParam();
      //TestFormDeleate();
      //TestPerfDictDir();
      

      //CreateUndifinedNewWordsFile("c:\\SS\\SS\\UnitTestsBin\\Core\\dictionary\\RusEng.txtsf", "c:\\SS\\SS\\UnitTestsBin\\Core\\dictionary\\RusEng_UDEFINED_NEWWORDS.txts");
      //Menu();

      //TestCNumberCombinatory();

      //CThreadTestBase< CTestLoadManagerCreater > ThreadTest(10);
      //ThreadTest.ThreadTest();

      //DWORD Rez = CopyDirectory( _T("c:\\SS\\SS\\Database"), _T("c:\\!MY_DOCS\\BACKUP") );

      //if( Rez != ERROR_SUCCESS )
      //   cout << GetDWErrorInfo(Rez) << endl;
      //else
      //   cout << "Копирование прошло удачно!!!" << endl;

      //CLoadManagerCreater LMC;
      //
      //for( int i = 0; i < 1000; ++i )
      //{
      //   CLoadManagerCreater::TAutoPtr pLM1 = LMC.Create();
      //   IDictionary *pD1 = GetIDictionary(pLM1.get());
      //}
    
      //IMinimalHypothesisTreeBuilder* pMHTB = (IMinimalHypothesisTreeBuilder*)
      //   GetLoadManager()->GetInterface(L"./core/linguistics/Dictionaries.dll", CLSID_IMinimalHypothesisTreeBuilder, IID_IMinimalHypothesisTreeBuilder);

      //APL_ERROR( pMHTB != 0 );

      //pMHTB->Init( DATA_TYPE::NAMES::ednENominalName );

      pBaseManager->OpenBase( L"AnyBase" );


      CoUninitialize();
	}			
	APL_CATCH()

	cout << "Выход CTestRunner::Run()" << endl;
}
///////////////////////////////////////////////////////////////////////////////

ULONG CTestRunner::Release()
{
	delete this;
	return 0;
}
///////////////////////////////////////////////////////////////////////////////

} //namespace Dictionary
} //namespace UnitTests
} //namespace SS

#endif //_SS_UNITTESTS
