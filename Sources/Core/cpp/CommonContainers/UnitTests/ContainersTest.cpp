//ContainersTest.cpp

#include "stdafx.h"
#include "ContainersTest.h"

#ifdef _SS_UNITTESTS

#include"../../ASSInterface/ITextBlockEx.h"
#include <iostream>
#include "..\..\ASSInterface\IBlackBoxDictionaryIndex.h"
#include "..\..\ASSInterface\IBlackBoxIndex.h"
#include "../ServiceIndex.h"
#include "../TextFeature.h"
#include "../../BlackBox/DefineTextFeature.h"
using namespace CommonContainers::Define;
#include "..\..\ASSInterface\TDictionaryNames.h"
#include "..\TextBlockCut.h";
#include "../IndexAndPosition.h"
#include "..\..\ASSInterface\TDictionaryManager.h"

namespace SS
{
	namespace UnitTests
	{
		namespace CommonContainers
		{
			//КОНСТРУКТОРЫ
			CContainersTest::CContainersTest(ILoadManager* pLoadManager)  
			{
				SS_TRY
					
					if (pLoadManager)
					{
						m_pLoadManager = pLoadManager;

						m_pIndexationResult = new CIndexationResult();
						m_pSearchResult = new CSearchResult();
						m_pQueryResultEx = new CQueryResultEx();

						m_pIndexationResult->SetLoadManager(m_pLoadManager);
						m_pSearchResult->SetLoadManager(m_pLoadManager);
						m_pQueryResultEx->SetLoadManager(m_pLoadManager);//для Query нет необходимости
						
						//SetString();
						InitConnection();
					}
					else
					{
						std::cout << " CContainersTest::MySetLoadManager,,,no pLoadManager \n ";
					}
					m_pTextIn = NULL;
					m_pTextOut = NULL;
					m_pQueryIn = NULL;
					m_pQueryOut = NULL;

	
				SS_CATCH(L"")
				
			}

			CContainersTest::~CContainersTest()
			{
				SS_TRY
					if (m_pIndexationResult)
					{
						delete m_pIndexationResult;
					}
					if (m_pSearchResult)
					{
						delete m_pSearchResult;
					}
					if (m_pQueryResultEx)
					{
						delete m_pQueryResultEx;
					}
					m_fout.close();
					m_wfout.close();
				SS_CATCH(L"")
			}

			//СОБСТВЕННЫЕ МЕТОДЫ
			void CContainersTest::BuildText()
			{
				SS_TRY
					
					IParagraph*			pParagraph = NULL;
					ISentence*			pSentence = NULL;
					IUnit*				pUnit = NULL;
					IUnit*				pUnitChild = NULL;
					IIndex*				pIndex = NULL;
					IDictionaryIndex*	pDictionaryIndex = NULL;
					IFeature*			pFeature = NULL;
					SS::Dictionary::Types::TDictionaryIndex oMorphoIndex;

					unsigned int iP, iS, iU, iI, i = 16777216;		//при смещении >> 24 дает 1
					
					m_pTextIn = m_pBlackBoxTextStorage->CreateText();
					for (iP = 0; iP < 4; iP++)//1
					{
						pParagraph = m_pBlackBoxTextStorage->CreateParagraph();
						m_pTextIn->AddParagraph(pParagraph);
						for (iS = 0; iS < 8; iS++)//1
						{
							pSentence = m_pBlackBoxTextStorage->CreateSentence();
							pParagraph->AddSentence(pSentence);
							for (iU = 0; iU < 6; iU++)
							{
								pUnit = m_pBlackBoxTextStorage->CreateUnit();
								pSentence->AddUnit(pUnit);
								//установка Word
								pUnit->SetWord(L"Slovo");
								//установка Position
								pUnit->SetPosition(1);
								//установка HighlightType
								pUnit->SetHighlightType(SS::Core::Types::IndexationSearch::hltAnswerWord);
								//установка FirstCharPosition
								pUnit->SetFirstCharPosition(1234);
								//установка IsInAnswerSentence
								pUnit->SetIsInAnswerSentence(true);
								
								/*pUnitChild = m_pBlackBoxTextStorage->CreateUnit();
								pUnitChild->SetWord("ребёнок");
								pUnit->AddUnitVertical(pUnitChild);
								
								pUnit = m_pBlackBoxTextStorage->CreateUnit();
								pUnit->SetWord("справаОтРебёнка");
								pUnitChild->AddUnitHorizontal(pUnit);*/
								
								
								for (iI = 0; iI < 5; iI++)
								{
									pIndex = m_pBlackBoxTextStorage->CreateIndex();
									pUnit->AddIndex(pIndex);
									pDictionaryIndex = pIndex->GetDictionaryIndex();
									oMorphoIndex.SetDictionaryIndex(i);
									pDictionaryIndex->AppendIndex(oMorphoIndex);
									pFeature = m_pBlackBoxTextStorage->CreateFeature();
									pIndex->AddFeature(pFeature);
								}
							}
						}
					}
				
				SS_CATCH(L"")
			}
			
			void CContainersTest::BuildQueryIn()
			{
				SS_TRY
					
					ITransformation*				pTransformation = NULL;
					IGroup*							pGroup = NULL;
					IQueryUnit*						pUnit = NULL;
					IQueryUnitIndexCollection*		pCollection = NULL;
					IQueryUnitIndex*				pIndex = NULL;
					
					unsigned int iT, iG, iU, iC, iI, i, j = 0;
					using namespace SS::Dictionary::DATA_TYPE::NAMES;
					using namespace SS::Interface::Core::BlackBox;
					
					i = 0;//ednSymbols;

					m_pQueryIn = m_pBlackBoxQueryStorage->CreateQuery();
					for (iT = 0; iT < 2; iT++)
					{
						pTransformation = m_pBlackBoxQueryStorage->CreateTransformation();
						m_pQueryIn->AddTransformation(pTransformation);
						for (iG = 0; iG < 2; iG++)
						{
							pGroup = m_pBlackBoxQueryStorage->CreateGroup();
							pTransformation->AddGroup(pGroup);
							for (iU = 0; iU < 2; iU++)
							{
								pUnit = m_pBlackBoxQueryStorage->CreateQueryUnit();
								if (iT < 1) 
								{ pUnit->SetObligatory(); }
								pUnit->SetWord(L"Слово");
								pGroup->AddQueryUnit(pUnit);
								for (iC = 0; iC < 4; iC++)
								{
									pCollection = m_pBlackBoxQueryStorage->CreateQueryUnitIndexCollection();
									switch (iC) 
									{
									case 0:
											pCollection->SetIndexCollectionType(eitMorphoIndex);
											break;
									case 1:
											pCollection->SetIndexCollectionType(eitSyntaxIndex);
											break;
									case 2:
											pCollection->SetIndexCollectionType(eitSemanticIndex);
											break;
									/*case 3:
											pCollection->SetIndexCollectionType(eitSynonymIndex);
											break;*/
									default:
											pCollection->SetIndexCollectionType(eitMorphoIndex);
									}
																
									pUnit->AddQueryUnitIndexCollection(pCollection);
									for (iI = 0; iI < 2; iI++)
									{
										pIndex = m_pBlackBoxQueryStorage->CreateQueryUnitIndex();
										pCollection->AddQueryUnitIndex(pIndex);
										i++;
										pIndex->SetIndex(i);
									}
								}
							}
							pGroup->SetSequenceID(j++);				//установление SequenceID
						}
					}
					m_fout << " Transformation = " << iT << '\n';
					m_fout << " Group = " << iG << '\n';
					m_fout << " QueryUnit = " << iU << '\n';
					m_fout << " Collection = " << iC << '\n';
					m_fout << " Index = " << iI << '\n';
					m_fout << " i = " << i << '\n';

				SS_CATCH(L"")
			}

			void CContainersTest::BuildQueryOut()
			{
				SS_TRY
					IQueryIndex*					pQueryIndex = NULL;

					ITransformation*				pTransformation = NULL;
					IGroup*							pGroup = NULL;
					IQueryUnit*						pUnit = NULL;
					IQueryUnitIndexCollection*		pCollection = NULL;
					IQueryUnitIndex*				pIndex = NULL;
					int iLeftBr;
					
					m_pQueryOut = m_pBlackBoxQueryStorage->CreateQuery();
					
					pQueryIndex = m_pQueryResultEx->GetFirstQueryIndex();
					while (pQueryIndex)
					{
						iLeftBr = pQueryIndex->GetLeftBrackets();
						
						while (iLeftBr >= 0)
						{
							switch(iLeftBr)
							{
							case 0: pIndex = m_pBlackBoxQueryStorage->CreateQueryUnitIndex();
									pIndex->SetIndex(pQueryIndex->GetQueryIndex());
									pCollection->AddQueryUnitIndex(pIndex);
									iLeftBr--;
									break;
							
							case 1: pCollection = m_pBlackBoxQueryStorage->CreateQueryUnitIndexCollection();
									pUnit->AddQueryUnitIndexCollection(pCollection);
									iLeftBr--;
									break;
							
							case 2: pUnit = m_pBlackBoxQueryStorage->CreateQueryUnit();
									pGroup->AddQueryUnit(pUnit);
									pUnit->SetWord(pQueryIndex->GetQueryWord());
									iLeftBr--;
									break;
							
							case 3: pGroup = m_pBlackBoxQueryStorage->CreateGroup();
									pTransformation->AddGroup(pGroup);
									iLeftBr--;
									break;
							
							case 4: pTransformation = m_pBlackBoxQueryStorage->CreateTransformation();
									m_pQueryOut->AddTransformation(pTransformation);
									iLeftBr--;
									break;
							
							case 5: iLeftBr--;
									break;
							
							default: std::cout << " Error \n";
									 iLeftBr--;
							}
						}
						pQueryIndex = m_pQueryResultEx->GetNextQueryIndex();
					}
				SS_CATCH(L"")
			}
			
			void CContainersTest::SetString()
			{
				SS_TRY
					
					std::ifstream fin;
					char Temp[300];
					const Limit = 299;
										
					fin.open("TextData.txt");
					if (!fin.is_open())
					{
						std::cout << " Error open file !!! " << std::endl;
						exit(0);
					}
					else	std::cout << "...OK open file !!!  " << std::endl;
					while (fin.getline(Temp, Limit))
					{
						m_strText += Temp;
						m_strText += '\n';
					}
					if (!fin.eof()) 
					{
						std::cout << " Error\n ";
					}
					fin.close();
							
				SS_CATCH(L"")
			}
			
			void CContainersTest::FillUnits(std::vector<IUnit*> & vUnits, IText* pText)
			{
				SS_TRY
					
					IParagraph*	pParagraph = NULL;
					ISentence*	pSentence = NULL;
					IUnit*		pCurrUnit = NULL;
					IUnit*		pRightUnit = NULL;
					IUnit*		pChildUnit = NULL;
					
					//перебор текста из CommonContainers vUnitsRet
					if (!pText)
					{
						std::cout << " FillUnits -- pText == NULL \n";
						return;
					}
					
					pParagraph = pText->GetFirstParagraph();
					while (pParagraph)								
					{
						pSentence = pParagraph->GetFirstSentence();		//получаем первое предложение
						while (pSentence)
						{
							pCurrUnit = pSentence->GetFirstUnit();		//получаем первый юнит
							//ПЕРЕБОР Unit - ов
							while (pCurrUnit)
							{
								pChildUnit = pCurrUnit->GetLeftChildUnit();
								if (pChildUnit)
								{
									pCurrUnit = pChildUnit;
									continue;	
								}

								vUnits.push_back(pCurrUnit);			//заносим Unit

								pRightUnit = pCurrUnit->GetRightUnit();
								while (pRightUnit == NULL)
								{
									pCurrUnit = pCurrUnit->GetParentUnit();
									if (pCurrUnit == NULL)
									{
										break;
									}

									vUnits.push_back(pCurrUnit);		//заносим Unit

									pRightUnit = pCurrUnit->GetRightUnit();
								}
								pCurrUnit = pRightUnit;
							}
							//КОНЕЦ переб Unit - ов
							pSentence = pSentence->GetNextSentence();
						}
						pParagraph = pParagraph->GetNextParagraph();
					}				
				SS_CATCH(L"")
			}
			
			void CContainersTest::FillIndex(IUnit* pUnit, std::vector<unsigned int> & vIndex)
			{
				SS_TRY
				{
					IIndex* pIndex = NULL;
					IDictionaryIndex* pDictionaryIndex = NULL;
					SS::Dictionary::Types::TDictionaryIndex oMorphoIndex;
				
					for (pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
					{
						if ( !(pDictionaryIndex = pIndex->GetDictionaryIndex()) ) 
						{ std::cout << "Нет IDictionaryIndex." << '\n';	}
						
						if ( pDictionaryIndex->GetFirst(&oMorphoIndex) ) 
						{ vIndex.push_back(oMorphoIndex.GetDictionaryIndex()); }
						else
						{ std::cout << "Нет первого TDictionaryIndex." << '\n';	}
					}
				}
				SS_CATCH(L"")
			}

			//Возвращает true если данные в юнитах одинаковые
			bool CContainersTest::CompareUnitsData(IUnit* pFirstUnit, IUnit* pSecondUnit)
			{
				SS_TRY
				{
					std::wstring wFirstWord = pFirstUnit->GetWord();
					std::wstring wSecondWord = pSecondUnit->GetWord();

					if ( (wFirstWord != wSecondWord) ||
						 (pFirstUnit->GetPosition() != pSecondUnit->GetPosition()) ||
						 (pFirstUnit->GetHighlightType() != pSecondUnit->GetHighlightType()) ||
						 (pFirstUnit->GetFirstCharPosition() != pSecondUnit->GetFirstCharPosition()) ||
						 (pFirstUnit->GetIsInAnswerSentence() != pSecondUnit->GetIsInAnswerSentence()) ) 
					{
						return false;
					}
					else
					{
						return true;
					}
				}
				SS_CATCH(L"")
			}
			
			bool CContainersTest::CompareIText()
			{
				SS_TRY
					
					IUnit*				pUnit = NULL;

					std::vector<IUnit*>		vUnitsOut;
					std::vector<IUnit*>		vUnitsIn;
										
					std::vector<unsigned int> vIndexOut;
					std::vector<unsigned int> vIndexIn;
					std::vector<unsigned int>::iterator IndexIter;	//итератор
					
					int SizeIn = 0;
					int SizeOut = 0;
					int i = 0;
									
					FillUnits(vUnitsIn, m_pTextIn);
					FillUnits(vUnitsOut, m_pTextOut);
					SizeIn = vUnitsIn.size();
					SizeOut = vUnitsOut.size();
					if (SizeOut != SizeIn)
					{
						std::cout << " SizeOut != SizeIn \n";
						std::cout << SizeIn << "\n";
						std::cout << SizeOut << "\n";
						return FALSE;
					}
					std::cout << SizeIn << "\n";
					std::cout << SizeOut << "\n";	
				
					for (i = 0; i < SizeIn; i++)
					{
						pUnit = vUnitsIn[i];
						FillIndex(pUnit, vIndexIn);
						pUnit = vUnitsOut[i];
						FillIndex(pUnit, vIndexOut);
						if ( !CompareUnitsData(vUnitsIn[i], vUnitsOut[i]) ) 
						{ return false; }
					}
					if (vIndexIn != vIndexOut)
					{ return FALSE; }
					
					return TRUE;
				
				SS_CATCH(L"")
			}

			
			void CContainersTest::FillQueryIndex(IQuery* pQuery, std::vector<unsigned int> & vIndex, std::vector<std::wstring> & vWords)
			{
				SS_TRY
					
					ITransformation*				pTransformation = NULL;
					IGroup*							pGroup = NULL;
					IQueryUnit*						pUnit = NULL;
					IQueryUnitIndexCollection*		pCollection = NULL;
					IQueryUnitIndex*				pIndex = NULL;

					pTransformation = pQuery->GetFirstTransformation();
					while (pTransformation)
					{
						pGroup = pTransformation->GetFirstGroup();
						while (pGroup)
						{
							pUnit = pGroup->GetFirstQueryUnit();
							while (pUnit)
							{
								//Заполнение слов
								vWords.push_back(pUnit->GetWord());
								pCollection = pUnit->GetFirstQueryUnitIndexCollection();
								while (pCollection)
								{
									pIndex = pCollection->GetFirstQueryUnitIndex();
									while (pIndex)
									{
										vIndex.push_back(pIndex->GetIndex());
										pIndex = pIndex->GetNextQueryUnitIndex();
									}
									pCollection = pCollection->GetNextQueryUnitIndexCollection();
								}
								pUnit = pUnit->GetNextQueryUnit();
							}
							pGroup = pGroup->GetNextGroup();
						}
						pTransformation = pTransformation->GetNextTransformation();
					}

				SS_CATCH(L"")
			}

			bool CContainersTest::CompareIQuery()
			{
				SS_TRY
					
					std::vector<unsigned int> vIndexIn;
					std::vector<unsigned int> vIndexOut;
					
					std::vector<std::wstring> vWordsIn;
					std::vector<std::wstring> vWordsOut;

					FillQueryIndex(m_pQueryIn, vIndexIn, vWordsIn);
					FillQueryIndex(m_pQueryOut, vIndexOut, vWordsOut);

					if ( (vIndexIn == vIndexOut) && (vWordsIn == vWordsOut) )
					{
						return TRUE;
					}
					else
					{
						return FALSE;
					}
						
				SS_CATCH(L"")
			}
			
			void CContainersTest::InitConnection()			//получение m_pTextIn - а
			{
				SS_TRY
					
					m_pBlackBoxTextStorage = (IBlackBoxTextStorage*) m_pLoadManager->GetInterface(L"./core/linguistics/BlackBox.dll", CLSID_BlackBoxTextStorage, IID_BlackBoxTextStorage);
					if (!m_pBlackBoxTextStorage)
					{
						std::cout << " CContainersTest::InitConnection() --- m_pBlackBoxTextStorage == NULL \n";
					}
					
					m_pBlackBoxQueryStorage = (IBlackBoxQueryStorage*) m_pLoadManager->GetInterface(L"./core/linguistics/BlackBox.dll", CLSID_BlackBoxQueryStorage, IID_BlackBoxQueryStorage);
					if (!m_pBlackBoxQueryStorage)
					{
						std::cout << " CContainersTest::InitConnection() --- m_pBlackBoxQueryStorage == NULL \n";
					}

					m_fout.open("QueryResult.txt");
					if (!m_fout.is_open())
					{
						std::cout << " Error open file !!! " << std::endl;
						exit(0);
					}
					m_wfout.open("XML.xml");
					if (!m_wfout.is_open())
					{
						std::cout << " Error open file !!! " << std::endl;
						exit(0);
					}
		
				SS_CATCH(L"")
			}
			
			//ДРУГИЕ МЕТОДЫ
			void CContainersTest::TestIndexationResult()	// заполнение CommonContainers и сравнение IText - ов
			{
				SS_TRY
				std::cout << "TestIndexationResult\n";
						
					using SS::Interface::Core::CommonContainers::ITextBlockEx;
					using SS::Core::Types::IndexationSearch::TLinguisticProcessorMode;
					ITextBlockEx*	pTextBlockEx;
					bool b = TRUE;
				
					__int64 iStartTick	= 0;
					__int64 iStopTick	= 0;
					__int64 iSumTickIn	= 0;
					__int64 iSumTickOut = 0;
					__int64 iFrequency	= 0;
					double  dbTimeIn	= 0.0;
					double  dbTimeOut	= 0.0;
								
					pTextBlockEx = dynamic_cast <ITextBlockEx*> (m_pIndexationResult->AddTextBlock());
					TLinguisticProcessorMode	oLinguisticProcessorMode;
					
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsMorpho(b);
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsSemantic(b);
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsSynonims(b);
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsSyntax(b);
					
					pTextBlockEx->SetLinguisticProcessorMode(&oLinguisticProcessorMode);
					
					BuildText();	//заполнение TextIn

					QueryPerformanceCounter((LARGE_INTEGER*)&iStartTick);
					pTextBlockEx->SetIText(m_pTextIn);							//заполнение CommonContainers
					QueryPerformanceCounter((LARGE_INTEGER*)&iStopTick);
					iSumTickIn += (iStopTick - iStartTick);

					QueryPerformanceCounter((LARGE_INTEGER*)&iStartTick);
					m_pTextOut = pTextBlockEx->GenerateIText();					//получение m_pTextOut
					QueryPerformanceCounter((LARGE_INTEGER*)&iStopTick);
					iSumTickOut += (iStopTick - iStartTick);
					
					if (!(CompareIText()))										//сравнение In и Out
					{
						std::cout << " ERROR TestIndexationResult -- CompareIText \n";
					}
					else
					{
						std::cout << " Test OK ! \n";
					}
					
					m_pTextIn->ClearText();
					pTextBlockEx->Clear();
					m_pIndexationResult->Clear();

					QueryPerformanceFrequency((LARGE_INTEGER*)&iFrequency);
					dbTimeIn = (double) iSumTickIn / (double) iFrequency;
					dbTimeOut = (double) iSumTickOut / (double) iFrequency;

					std::cout << " Time create CommonContainers = " << dbTimeIn << "\n";
					std::cout << " Time create IText            = " << dbTimeOut << "\n";
					
				SS_CATCH(L"")
			}
			
			void CContainersTest::TestSearchResult()			// тест для проверки сортировки блоков
			{
				SS_TRY
					std::cout << "TestSearchResult\n";
					using SS::Interface::Core::CommonContainers::ITextBlock;
					ITextBlock*	pTextBlock;
					unsigned int i = 0;
					unsigned int iRel = 1;
					bool bSort = TRUE;

					for (i = 0; i < 10; i++)
					{
						pTextBlock = m_pSearchResult->AddTextBlock();
						pTextBlock->SetRelevance(iRel++);
					}
								
					pTextBlock = m_pSearchResult->GetFirstTextBlock();
					while (pTextBlock)
					{
						std::cout << (pTextBlock->GetRelevance()) << '\n';
						pTextBlock = m_pSearchResult->GetNextTextBlock();
					}
					
					m_pSearchResult->SetSortedMode(bSort);
					
					pTextBlock = m_pSearchResult->GetFirstTextBlock();
					while (pTextBlock)
					{
						std::cout << (pTextBlock->GetRelevance()) << '\n';
						pTextBlock = m_pSearchResult->GetNextTextBlock();
					}
					m_pSearchResult->Clear();
				SS_CATCH(L"")
			}

			void CContainersTest::TestSearchResultCommon()
			{
				SS_TRY;
					std::cout << "TestSearchResultCommon\n";
					
					using SS::Interface::Core::CommonContainers::ITextBlock;
					using SS::Interface::Core::CommonContainers::ITextBlockCut;
					
					ITextBlock*	pTextBlock;
					ITextBlockCut* pTextBlockCut;

					int iCount;
					const int ciCountBlocks = 3;
					bool bError = false;

					m_pSearchResult->Clear();
					
					//добавление блоков
					for (int i = 0; i < ciCountBlocks; i++)
					{
						m_pSearchResult->AddTextBlock();
					}
					//проверка метода Count()					
					iCount = m_pSearchResult->Count();
					if (iCount != ciCountBlocks) bError = true;
						
					//получение блоков по номеру (без сортировки)
					unsigned int uiNumber;
					std::cout << "Block Count = " << iCount << '\n';
					std::cout << "Input Number: ";
											
					while (std::cin >> uiNumber) 
					{
						pTextBlock = m_pSearchResult->GetTextBlock(uiNumber);
						std::cout << "Adres:" << pTextBlock << '\n' << '\n';
						std::cout << "Input Number:";
					}
					//ОСНОВНОЕ ТЕЛО ТЕСТА
					
					//1 - обычный вызов
					pTextBlock = m_pSearchResult->GetFirstTextBlock();
					if (pTextBlock == NULL) bError = true;
					pTextBlock = m_pSearchResult->GetNextTextBlock();
					if (pTextBlock == NULL) bError = true;
					//2 - после сортировки GetNext без вызова GetFirst должен вернуть NULL
					m_pSearchResult->SetSortedMode(true);
					pTextBlock = m_pSearchResult->GetNextTextBlock();
					if (pTextBlock != NULL) bError = true;
					//3 - обычный вызов после сортировки
					pTextBlock = m_pSearchResult->GetFirstTextBlock();
					if (pTextBlock == NULL) bError = true;
					pTextBlock = m_pSearchResult->GetNextTextBlock();
					if (pTextBlock == NULL) bError = true;
					//4 - вызов по номеру после сортировки (должен быть добавлен хотя бы один блок)
					m_pSearchResult->SetSortedMode(true);
					pTextBlock = m_pSearchResult->GetTextBlock(0);
					if (pTextBlock == NULL) bError = true;
					//
					m_pSearchResult->SetSortedMode(false);
					pTextBlock = m_pSearchResult->GetTextBlock(0);
					if (pTextBlock == NULL) bError = true;

					//проверка метода Clear()
					m_pSearchResult->Clear();
					iCount = m_pSearchResult->Count();
					if (iCount != 0) bError = true;
								
					//КОНЕЦ ОСНОВНОГО ТЕЛА
					if (bError)
					{
						std::cout << "Error Test\n";
					}
					else
					{
						std::cout << "Test OK\n";
					}

				SS_CATCH(L"");
			}
			
			void CContainersTest::OnConsole(IQueryIndex* pQueryIndex)
			{
				SS_TRY
										
					unsigned int iCountLeftBr = 0;
					unsigned int iCountRightBr = 0;
					unsigned int i = 0;

					iCountLeftBr = pQueryIndex->GetLeftBrackets();
					iCountRightBr = pQueryIndex->GetRightBrackets();

					if (iCountLeftBr)
					{
						for (i = 0; i < iCountLeftBr; i++)
						{
							//std::cout << '(';
							m_fout << '(';
						}
					}
					
					//std::cout << (pQueryIndex->GetQueryIndex());
					m_fout << (pQueryIndex->GetQueryIndex());
					
					if (iCountRightBr)
					{
						for (i = 0; i < iCountRightBr; i++)
						{
							//std::cout << ')';
							m_fout << ')';
						}
					}
					
					//std::cout << (pQueryIndex->GetOperator());
					m_fout << (pQueryIndex->GetOperator());
						
				SS_CATCH(L"")
			}

			void CContainersTest::TestQueryResult()		//вывод сложного выражения со скобками	
			{
				SS_TRY
				std::cout << "TestQueryResult\n";
					
					using SS::Core::Types::IndexationSearch::TLinguisticProcessorMode;
					IQueryIndex*	pQueryIndex = NULL;
					bool b = true;

					TLinguisticProcessorMode	oLinguisticProcessorMode;
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsMorpho(b);
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsSemantic(b);
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsSynonims(b);
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsSyntax(b);

					BuildQueryIn();		//заполнение IQuery
					m_pQueryResultEx->SetLinguisticProcessorMode(&oLinguisticProcessorMode);
					
					m_pQueryResultEx->SetIQuery(m_pQueryIn);
					BuildQueryOut();
					
					
					if (!CompareIQuery())
					{
						std::cout << " Error CompareQuery \n";
					}
					else
					{
						std::cout << " Test OK ! \n";
					}

					pQueryIndex = m_pQueryResultEx->GetFirstQueryIndex();
					while (pQueryIndex)
					{
						OnConsole(pQueryIndex);
						pQueryIndex = m_pQueryResultEx->GetNextQueryIndex();
					}
						
					//std::cout << '\n';
					/*pQueryIndex = m_pQueryResultEx->GetFirstQueryIndex();
					while (pQueryIndex)
					{
						std::cout << pQueryIndex->GetAttributes().m_SequenceID << '\n';
						pQueryIndex = m_pQueryResultEx->GetNextQueryIndex();
					}*/
					m_pQueryResultEx->Clear();
				SS_CATCH(L"")
			}

			//Проверка работы буфера для ServiceIndex
			void CContainersTest::TestServiceIndex()
			{
				SS_TRY
				{
					std::cout << "TestServiceIndex:\n";
					m_pSearchResult->Clear();

					SS::Core::Types::IndexationSearch::TLinguisticProcessorMode oLinguisticProcessorMode;
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsMorpho(true);
					ITextBlockEx* pTextBlockEx = dynamic_cast<ITextBlockEx*> (m_pSearchResult->AddTextBlock());
					pTextBlockEx->SetLinguisticProcessorMode(&oLinguisticProcessorMode);
					IServiceIndex* pServiceIndex = pTextBlockEx->AddBlockCut()->AddIndexAndPosition()->GetServiceIndex();
										
					SS::Core::Types::IndexationSearch::TViewIndex  oViewIndex;
					unsigned int iSizeViewIndex = sizeof(TViewIndex);
					
					//Формирование и установка буфера для Сервисного индекса
					const int SizeBuff = 20;
					const int Count = SizeBuff/iSizeViewIndex;
					char Buff[SizeBuff];
					
					int iPos = 0;
					int iIndex = 6;
					int iForm = 61;
					for (int i = 0; i < Count; i++)
					{
						oViewIndex.m_DictionaryIndex = iIndex;
						oViewIndex.m_IDForm = iForm;
						memcpy(Buff + iPos, &oViewIndex, iSizeViewIndex);
						iPos += iSizeViewIndex;
						iForm++;
						if (i > 1) 
						{ iIndex++; }
					}
					pServiceIndex->SetInfoBuff(Buff, SizeBuff);

					//Получение буфера и просмотр Сервисных индексов
					char* pBuff;
					unsigned int iSize;
					pServiceIndex->GetInfoBuff(&pBuff, &iSize);
										
					iPos = 0;
					for (int i = 0; i < Count; i++)
					{
						oViewIndex.m_DictionaryIndex = 0;
						oViewIndex.m_IDForm = 0;
						memcpy(&oViewIndex, Buff + iPos,  iSizeViewIndex);
						iPos += iSizeViewIndex;
					}
				
					m_pSearchResult->Clear();
					std::cout << "Tect OK!\n";
				}
				SS_CATCH(L"")
			}

			//Проверка работы буфера для TextFeature
			void CContainersTest::TestTextFeature()
			{
				SS_TRY
					std::cout << "TestTextFeature\n\n";
					
					using SS::Core::CommonContainers::CTextFeature;

					SS::Interface::Core::CommonContainers::ITitle* pTitleFirst = NULL;
					SS::Interface::Core::CommonContainers::ITitle* pTitleSecond = NULL;
					CTextFeature	oTextFeatureFirst;
					CTextFeature	oTextFeatureSecond;
					oTextFeatureFirst.SetLoadManager(m_pLoadManager);
					oTextFeatureSecond.SetLoadManager(m_pLoadManager);
					
					const int		iCountTitle = 3;
					
					unsigned int	iSizeBuf	= 0;
					char*			pBuff		= NULL;
					int				i			= 0;
					bool			bError		= false;
							
					FILETIME	TextFileTime;
					TextFileTime.dwHighDateTime = 2;
					TextFileTime.dwLowDateTime = 3;
								
					oTextFeatureFirst.SetTextPath(L"TextPath");
					oTextFeatureFirst.SetTextName(L"TextName");
					oTextFeatureFirst.SetAccessRights("AccessRights");
		       		oTextFeatureFirst.SetFirstSentenceAbsNumber(1);
					oTextFeatureFirst.SetTextNumber(2);
					oTextFeatureFirst.SetTextSize(3);
					oTextFeatureFirst.SetTextFileSize(4);
					oTextFeatureFirst.SetPluginID(5);
					oTextFeatureFirst.SetTextFileTime(&TextFileTime);
					//установка тайтлов
					for (i = 0; i < iCountTitle; i++)
					{
						oTextFeatureFirst.AddTitle(i, i, i);
					}
							
					oTextFeatureFirst.GetTextFeatureBuff(&pBuff, &iSizeBuf);
					oTextFeatureFirst.SetTextFeatureBuff(pBuff, iSizeBuf);
					oTextFeatureFirst.GetTextFeatureBuff(&pBuff, &iSizeBuf);
					
					oTextFeatureSecond.SetTextFeatureBuff(pBuff, iSizeBuf);
					oTextFeatureSecond.GetTextFeatureBuff(&pBuff, &iSizeBuf);
					oTextFeatureSecond.SetTextFeatureBuff(pBuff, iSizeBuf);

					//сравнение 

					if ( (wcscmp(oTextFeatureFirst.GetTextPath(), oTextFeatureSecond.GetTextPath())) ||
						 (wcscmp(oTextFeatureFirst.GetTextName(), oTextFeatureSecond.GetTextName())) ||
						 (strcmp(oTextFeatureFirst.GetAccessRights(), oTextFeatureSecond.GetAccessRights())) ) 
					{
						bError = true;
					}					
					
					if ( (oTextFeatureFirst.GetFirstSentenceAbsNumber() != oTextFeatureSecond.GetFirstSentenceAbsNumber()) ||
						 (oTextFeatureFirst.GetTextNumber() != oTextFeatureSecond.GetTextNumber()) ||
						 (oTextFeatureFirst.GetTextSize() != oTextFeatureSecond.GetTextSize()) ||
						 (oTextFeatureFirst.GetTextFileSize() != oTextFeatureSecond.GetTextFileSize()) ||
						 (oTextFeatureFirst.GetPluginID() != oTextFeatureSecond.GetPluginID()) )
					{
						bError = true;
					}

					if ( (oTextFeatureFirst.GetTextFileTime().dwHighDateTime != oTextFeatureSecond.GetTextFileTime().dwHighDateTime) ||
						 (oTextFeatureFirst.GetTextFileTime().dwLowDateTime != oTextFeatureSecond.GetTextFileTime().dwLowDateTime) )
					{
						bError = true;
					}

					pTitleFirst = oTextFeatureFirst.GetFirstTitle();
					pTitleSecond = oTextFeatureSecond.GetFirstTitle();

					for (i = 0; i < iCountTitle; i++)
					{
						if ( (pTitleFirst->GetFirstWord() != pTitleSecond->GetFirstWord()) || 
							 (pTitleFirst->GetLastWord() != pTitleSecond->GetLastWord()) ||
							 (pTitleFirst->GetType() != pTitleSecond->GetType()) ) 
						{
							bError = true;
						}
				
						pTitleFirst = pTitleFirst->GetNextTitle();
						pTitleSecond = pTitleSecond->GetNextTitle();
					}

					if (bError)
					{
						std::cout << "ERRORS , NO TEST ! \n";
					}
					else
					{
						std::cout << "TEST OK ! \n";
					}
									
					//вывод результатов
					
					//std::wcout << oTextFeatureSecond.GetTextPath() << '\n';
					//std::wcout << oTextFeatureSecond.GetTextName() << '\n';
					//std::cout << oTextFeatureSecond.GetAccessRights() << '\n';
					//
					//std::cout << oTextFeatureSecond.GetFirstSentenceAbsNumber() << '\n';
					//std::cout << oTextFeatureSecond.GetTextNumber() << '\n';
					//std::cout << oTextFeatureSecond.GetTextSize() << '\n';
					//std::cout << oTextFeatureSecond.GetTextFileSize() << '\n';
					//std::cout << oTextFeatureSecond.GetPluginID() << '\n';

					//std::cout << oTextFeatureSecond.GetTextFileTime().dwHighDateTime << '\n';
					//std::cout << oTextFeatureSecond.GetTextFileTime().dwLowDateTime << '\n';
					////вывод тайтлов
					//pTitle = oTextFeatureSecond.GetFirstTitle();
					//for (i = 0; i < iCountTitle; i++)
					//{
					//	std::cout << pTitle->GetFirstWord() << '\n';
					//	std::cout << pTitle->GetLastWord() << '\n';
					//	std::cout << pTitle->GetType() << '\n';
					//	
					//	pTitle = pTitle->GetNextTitle();
					//}


				SS_CATCH(L"")
			}

			void CContainersTest::TestTextBlockXML()
			{
				SS_TRY
					std::cout << "TestTextBlockXML \n";
									
					using SS::Interface::Core::CommonContainers::ITextBlockEx;
					using SS::Core::Types::IndexationSearch::TLinguisticProcessorMode;
					ITextBlockEx*	pTextBlockEx;
					bool b = TRUE;
					
					pTextBlockEx = dynamic_cast <ITextBlockEx*> (m_pIndexationResult->AddTextBlock());
					TLinguisticProcessorMode	oLinguisticProcessorMode;
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsMorpho(b);
					pTextBlockEx->SetLinguisticProcessorMode(&oLinguisticProcessorMode);
					BuildText();	//заполнение TextIn
					pTextBlockEx->SetIText(m_pTextIn);				//заполнение CommonContainers

					std::wcout << (*(pTextBlockEx->GetXML())) << '\n';
					m_wfout << (*(pTextBlockEx->GetXML()));
																		
					m_pTextIn->ClearText();
					pTextBlockEx->Clear();
					m_pIndexationResult->Clear();
				SS_CATCH(L"")
			}

			void CContainersTest::TestQueryIndexAttributes()
			{
				SS_TRY;
					std::cout << "TestQueryIndexAttributes\n";
					
					using SS::Core::Types::IndexationSearch::TLinguisticProcessorMode;
					IQueryIndex*	pQueryIndex = NULL;
					bool b = TRUE;
					
					TLinguisticProcessorMode	oLinguisticProcessorMode;
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsMorpho(b);
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsSemantic(b);
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsSynonims(b);
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsSyntax(b);

					BuildQueryIn();		//заполнение IQuery
					m_pQueryResultEx->SetLinguisticProcessorMode(&oLinguisticProcessorMode);
					m_pQueryResultEx->SetIQuery(m_pQueryIn);
					
					pQueryIndex = m_pQueryResultEx->GetFirstQueryIndex();
					while (pQueryIndex)
					{
						OnConsole(pQueryIndex);
						pQueryIndex = m_pQueryResultEx->GetNextQueryIndex();
					}
					
					pQueryIndex = m_pQueryResultEx->GetFirstQueryIndex();
					while (pQueryIndex)
					{
						m_fout << '\n';
						m_fout << (pQueryIndex->GetQueryIndex() >> 24) << '\n';
						m_fout << "m_TransformationID = " << pQueryIndex->GetAttributes().m_TransformationID << '\n';
						m_fout << "m_IndexPosition = " << pQueryIndex->GetAttributes().m_IndexPosition << '\n';
						m_fout << "m_SequenceID = " << pQueryIndex->GetAttributes().m_SequenceID << '\n';
						m_fout << "m_IsObligatory = " << pQueryIndex->GetAttributes().m_IndexAbsPosition << '\n';
						
						pQueryIndex = m_pQueryResultEx->GetNextQueryIndex();
					}

					m_pQueryResultEx->Clear();
			
				SS_CATCH(L"");
			}

			void CContainersTest::TestBlockCut()
			{
				SS_TRY
				{
					Core::CommonContainers::ITextBlockCut* pTextBlockCut = m_pIndexationResult->AddTextBlock()->AddBlockCut();
					wchar_t* wszPlugin = L"Cool";
					const wchar_t* wszRetPlugin = NULL;
					pTextBlockCut->SetPluginName(wszPlugin);
					wszRetPlugin = pTextBlockCut->GetPluginName();
					pTextBlockCut->SetPluginName(wszPlugin);
					wszRetPlugin = pTextBlockCut->GetPluginName();
					pTextBlockCut->SetPluginName(wszPlugin);
					wszRetPlugin = pTextBlockCut->GetPluginName();
					pTextBlockCut->SetPluginName(wszPlugin);
					wszRetPlugin = pTextBlockCut->GetPluginName();
					pTextBlockCut->SetLevel(10);
					unsigned int iLevel = pTextBlockCut->GetLevel();
					pTextBlockCut->SetNumber(20);
					unsigned int iNumber = pTextBlockCut->GetNumber();

					int iCount = 10;
					for (int i = 0; i < iCount; i++)
					{ pTextBlockCut->AddIndexAndPosition();	}

					IIndexAndPosition* pIndexAndPosition = NULL;
					iCount = 0;
					for ( pIndexAndPosition = pTextBlockCut->GetFirstIndexAndPosition(); pIndexAndPosition; 
						  pIndexAndPosition = pTextBlockCut->GetNextIndexAndPosition() )
					{ iCount++; }
					
					m_pIndexationResult->Clear();

					std::cout << "Test OK!\n";
				}
				SS_CATCH(L"")
			}

			//Тест работы контейнеров при поиске
			void CContainersTest::TotalTestSearch()
			{
				SS_TRY
				{
					std::cout << "TotalTestSearch:\n";
					m_pSearchResult->Clear();
				
					ITextBlockEx*		pTextBlockEx = NULL;
					ITextBlockCut*		pTextBlockCut = NULL;
					IIndexAndPosition*	pIndexAndPosition = NULL;
					IIndexationIndex*	pIndexationIndex = NULL;
					IServiceIndex*		pServiceIndex = NULL;
					IText*				pText = NULL;
										
					SS::Core::Types::IndexationSearch::TViewIndex* pViewIndex = NULL;
					SS::Core::Types::IndexationSearch::TViewIndex  oViewIndex;
					unsigned int iSizeViewIndex = sizeof(TViewIndex);

					SS::Core::Types::IndexationSearch::TLinguisticProcessorMode oLinguisticProcessorMode;
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsMorpho(true);

					pTextBlockEx = dynamic_cast<ITextBlockEx*> (m_pSearchResult->AddTextBlock());
					pTextBlockCut = pTextBlockEx->AddBlockCut();
					pIndexAndPosition = pTextBlockCut->AddIndexAndPosition();
					pIndexationIndex = pIndexAndPosition->GetIndexationIndex();
					pServiceIndex = pIndexAndPosition->GetServiceIndex();

					//Формирование буфера для Сервисного индекса
					const int SizeBuff = 20;
					const int Count = SizeBuff/iSizeViewIndex;
					char Buff[SizeBuff];
					char* pBuff;
					unsigned int iSize;
					
					int iPos = 0;
					for (int i = 0; i < Count; i++)
					{
						oViewIndex.m_DictionaryIndex = 10;
						oViewIndex.m_IDForm = 11;
						memcpy(Buff + iPos, &oViewIndex, iSizeViewIndex);
						iPos += iSizeViewIndex;
					}
					
					pServiceIndex->SetInfoBuff(Buff, SizeBuff);
					
					pTextBlockEx->SetLinguisticProcessorMode(&oLinguisticProcessorMode);
					pText = pTextBlockEx->GenerateIText();
					pTextBlockEx->Clear();
					pTextBlockEx->SetIText(pText);
					
					//Перебор контейнера
					for (pTextBlockCut = pTextBlockEx->GetFirstBlockCut(); pTextBlockCut; pTextBlockCut = pTextBlockEx->GetNextBlockCut())
					{
						for ( pIndexAndPosition = pTextBlockCut->GetFirstIndexAndPosition(); pIndexAndPosition; 
							  pIndexAndPosition = pTextBlockCut->GetNextIndexAndPosition() )
						{
							pServiceIndex = pIndexAndPosition->GetServiceIndex();
							pViewIndex = pServiceIndex->GetViewIndex();
							pServiceIndex->GetInfoBuff(&pBuff, &iSize);
						}
					}

					iPos = 0;
					for (int i = 0; i < Count; i++)
					{
						oViewIndex.m_DictionaryIndex = 0;
						oViewIndex.m_IDForm = 0;
						memcpy(&oViewIndex, Buff + iPos, iSizeViewIndex);
						iPos += iSizeViewIndex;
					}

					m_pSearchResult->Clear();

					std::cout << "Test OK!\n";
				}
				SS_CATCH(L"")
			}

			//Тест работы контейнеров при индексации
			void CContainersTest::TotalTestIndexation()
			{
				SS_TRY
				{
					std::cout << "TotalTestIndexation:\n";
					m_pIndexationResult->Clear();
									
					ITextBlockEx* pTextBlockEx = dynamic_cast <ITextBlockEx*>(m_pIndexationResult->AddTextBlock());
									
					SS::Core::Types::IndexationSearch::TLinguisticProcessorMode oLinguisticProcessorMode;
					bool b = true;
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsMorpho(b);
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsSemantic(b);
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsSynonims(b);
					oLinguisticProcessorMode.GetAnalyseDepthParams()->SetIsSyntax(b);

					pTextBlockEx->SetLinguisticProcessorMode(&oLinguisticProcessorMode);
					BuildText();
					pTextBlockEx->SetIText(m_pTextIn);
					//Перебор контейнера
					int Count = 0;
					for ( ITextBlockCut* pTextBlockCut = pTextBlockEx->GetFirstBlockCut(); pTextBlockCut; 
						  pTextBlockCut = pTextBlockEx->GetNextBlockCut() )
					{
						for ( IIndexAndPosition* pIndexAndPosition = pTextBlockCut->GetFirstIndexAndPosition(); pIndexAndPosition; 
							  pIndexAndPosition = pTextBlockCut->GetNextIndexAndPosition() )
						{
							Count++;
							IServiceIndex* pServiceIndex = pIndexAndPosition->GetServiceIndex();
							pServiceIndex->GetViewIndex();
							IIndexationIndex* pIndexationIndex = pIndexAndPosition->GetIndexationIndex();
						}
					}
				
					m_pIndexationResult->Clear();

					std::cout << "Test OK!\n";
				}
				SS_CATCH(L"")
			}

			//Тест проверки создания текст блоков
			void CContainersTest::TestAddTextBlock()
			{
				SS_TRY
				{
					std::cout << "TotalTestIndexation:\n";
					m_pIndexationResult->Clear();
			
					for (int i = 0; i < 10; i++)
					{ m_pIndexationResult->AddTextBlock(); }
					
					int Count = 0;
					for ( ITextBlock* pTextBlock = m_pIndexationResult->GetFirstTextBlock(); pTextBlock; 
						  pTextBlock = m_pIndexationResult->GetNextTextBlock() )
					{ Count++; }

					std::cout << Count << '\n';

					Count = 0;
					for ( ITextBlock* pTextBlock = m_pIndexationResult->GetFirstTextBlock(); pTextBlock; 
						  pTextBlock = m_pIndexationResult->GetNextTextBlock() )
					{ Count++; }
					
					std::cout << Count << '\n';

					m_pIndexationResult->Clear();
				}
				SS_CATCH(L"")
			}

		}
	}
}

#endif //_SS_UNITTESTS

























